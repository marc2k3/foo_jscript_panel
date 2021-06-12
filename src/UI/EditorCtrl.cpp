#include "stdafx.h"
#include "Config.h"
#include "DialogFindReplace.h"
#include "DialogGoto.h"
#include "EditorCtrl.h"

#include <ILexer.h>
#include <Lexilla.h>
#include <SciLexer.h>
#include <Scintilla.h>
#include <ScintillaStructures.h>

// Large portions taken from SciTE
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>

static const std::map<std::string, std::set<int>> styles =
{
	{ "style.default", { STYLE_DEFAULT } },
	{ "style.comment", { SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOCKEYWORD, SCE_C_COMMENTDOCKEYWORDERROR } },
	{ "style.linenumber", { STYLE_LINENUMBER } },
	{ "style.bracelight", { STYLE_BRACELIGHT } },
	{ "style.bracebad" , { STYLE_BRACEBAD } },
	{ "style.keyword", { SCE_C_WORD } },
	{ "style.identifier", { SCE_C_IDENTIFIER } },
	{ "style.number", { SCE_C_NUMBER } },
	{ "style.string", { SCE_C_STRING, SCE_C_CHARACTER } },
	{ "style.operator", { SCE_C_OPERATOR } },
};

static constexpr const char* js_keywords = "abstract boolean break byte case catch char class const continue"
	" debugger default delete do double else enum export extends false final"
	" finally float for function goto if implements import in instanceof int"
	" interface long native new null package private protected public return"
	" short static super switch synchronized this throw throws transient true"
	" try typeof var void while with enum byvalue cast future generic inner"
	" operator outer rest Array Math RegExp window fb gdi utils plman console";

static constexpr std::array<const int, 21> ctrlcodes = { 'Q', 'W', 'E', 'R', 'I', 'O', 'P', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'B', 'N', 'M', 186, 187, 226 };

CEditorCtrl::EditorStyle CEditorCtrl::ParseStyle(const std::string& str)
{
	EditorStyle style;

	for (const std::string& value : split_string(str, ","))
	{
		Strings tmp = split_string(value, ":");
		std::string primary = tmp[0];
		std::string secondary = tmp.size() == 2 ? tmp[1] : "";

		if (primary == "font") style.font = secondary;
		else if (primary == "size" && IsNumeric(secondary)) style.size = std::stoi(secondary);
		else if (primary == "fore") style.fore = ParseHex(secondary);
		else if (primary == "back") style.back = ParseHex(secondary);
		else if (primary == "bold") style.bold = true;
		else if (primary == "italics") style.italic = true;
	}
	return style;
}

CEditorCtrl::IndentationStatus CEditorCtrl::GetIndentState(Line line)
{
	IndentationStatus indentState = IndentationStatus::isNone;
	for (const std::string& sIndent : GetLinePartsInStyle(line, StatementIndent))
	{
		if (Includes(StatementIndent, sIndent))
			indentState = IndentationStatus::isKeyWordStart;
	}
	for (const std::string& sEnd : GetLinePartsInStyle(line, StatementEnd))
	{
		if (Includes(StatementEnd, sEnd))
			indentState = IndentationStatus::isNone;
	}
	for (const std::string& sBlock : GetLinePartsInStyle(line, BlockEnd))
	{
		if (Includes(BlockEnd, sBlock))
			indentState = IndentationStatus::isBlockEnd;
		if (Includes(BlockStart, sBlock))
			indentState = IndentationStatus::isBlockStart;
	}
	return indentState;
}

Colour CEditorCtrl::ParseHex(const std::string& hex)
{
	const auto int_from_hex_digit = [](int ch)
	{
		if (ch >= '0' && ch <= '9')
		{
			return ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			return ch - 'A' + 10;
		}
		else if (ch >= 'a' && ch <= 'f')
		{
			return ch - 'a' + 10;
		}
		return 0;
	};

	if (hex.length() != 7) return 0;
	const int r = int_from_hex_digit(hex.at(1)) << 4 | int_from_hex_digit(hex.at(2));
	const int g = int_from_hex_digit(hex.at(3)) << 4 | int_from_hex_digit(hex.at(4));
	const int b = int_from_hex_digit(hex.at(5)) << 4 | int_from_hex_digit(hex.at(6));
	return RGB(r, g, b);;
}

Line CEditorCtrl::GetCurrentLineNumber()
{
	return LineFromPosition(GetCurrentPos());
}

LRESULT CEditorCtrl::OnChange(UINT, int, CWindow)
{
	AutoMarginWidth();
	return 0;
}

LRESULT CEditorCtrl::OnCharAdded(LPNMHDR pnmh)
{
	const Range range = GetSelection();
	const NotificationData* notification = reinterpret_cast<NotificationData*>(pnmh);
	const int ch = notification->ch;

	if (range.start == range.end && range.start > 0)
	{
		if (CallTipActive())
		{
			switch (ch)
			{
			case ')':
				BraceCount--;
				if (BraceCount < 1)
					CallTipCancel();
				else
					StartCallTip();
				break;

			case '(':
				BraceCount++;
				StartCallTip();
				break;

			default:
				ContinueCallTip();
				break;
			}
		}
		else if (AutoCActive())
		{
			if (ch == '(')
			{
				BraceCount++;
				StartCallTip();
			}
			else if (ch == ')')
			{
				BraceCount--;
			}
			else if (!Contains(WordCharacters, ch))
			{
				AutoCCancel();

				if (ch == '.')
					StartAutoComplete();
			}
		}
		else
		{
			if (ch == '(')
			{
				BraceCount = 1;
				StartCallTip();
			}
			else
			{
				AutomaticIndentation(ch);

				if (Contains(WordCharacters, ch) || ch == '.')
					StartAutoComplete();
			}
		}
	}

	return 0;
}

LRESULT CEditorCtrl::OnKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	const KeyMod modifiers = (IsKeyPressed(VK_SHIFT) ? KeyMod::Shift : KeyMod::Norm) | (IsKeyPressed(VK_CONTROL) ? KeyMod::Ctrl : KeyMod::Norm) | (IsKeyPressed(VK_MENU) ? KeyMod::Alt : KeyMod::Norm);

	if (modifiers == KeyMod::Ctrl)
	{
		switch (wParam)
		{
		case 'F':
			OpenFindDialog();
			break;
		case 'H':
			OpenReplaceDialog();
			break;
		case 'G':
			fb2k::inMainThread([&]()
				{
					OpenGotoDialog();
				});
			break;
		case 'S':
			GetParent().PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BTN_APPLY, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
			break;
		}
	}
	else if (wParam == VK_F3 && (modifiers == KeyMod::Norm || modifiers == KeyMod::Shift))
	{
		if (!DlgFindReplace || DlgFindReplace->m_find_text.is_empty())
		{
			OpenFindDialog();
		}
		else
		{
			if (modifiers == KeyMod::Norm) // Next
			{
				Find(true);
			}
			else if (modifiers == KeyMod::Shift) // Previous
			{
				Find(false);
			}
		}
	}

	bHandled = FALSE;
	return 1;
}

LRESULT CEditorCtrl::OnUpdateUI(LPNMHDR)
{
	Position braceAtCaret = -1;
	Position braceOpposite = -1;
	FindBraceMatchPos(braceAtCaret, braceOpposite);

	if (braceAtCaret != -1 && braceOpposite == -1)
	{
		BraceBadLight(braceAtCaret);
		SetHighlightGuide(0);
	}
	else
	{
		BraceHighlight(braceAtCaret, braceOpposite);

		const int columnAtCaret = GetColumn(braceAtCaret);
		const int columnOpposite = GetColumn(braceOpposite);

		SetHighlightGuide(std::min(columnAtCaret, columnOpposite));
	}
	return 0;
}

LRESULT CEditorCtrl::OnZoom(LPNMHDR)
{
	AutoMarginWidth();
	return 0;
}

CEditorCtrl::Range CEditorCtrl::GetSelection()
{
	Range range;
	range.start = GetSelectionStart();
	range.end = GetSelectionEnd();
	return range;
}

Position CEditorCtrl::GetCaretInLine()
{
	return GetCurrentPos() - PositionFromLine(GetCurrentLineNumber());
}

Strings CEditorCtrl::GetLinePartsInStyle(Line line, const StyleAndWords& saw)
{
	Strings strings;
	const Position thisLineStart = PositionFromLine(line);
	const Position nextLineStart = PositionFromLine(line + 1);
	const bool separateCharacters = saw.words.length() == 1;
	std::string string;

	for (Position pos = thisLineStart; pos < nextLineStart; ++pos)
	{
		if (GetStyleAt(pos) == saw.styleNumber)
		{
			if (separateCharacters)
			{
				if (!string.empty())
				{
					strings.emplace_back(string);
					string.clear();
				}
			}
			string += GetCharAt(pos);
		}
		else if (!string.empty())
		{
			strings.emplace_back(string);
			string.clear();
		}
	}

	if (!string.empty())
	{
		strings.emplace_back(string);
	}
	return strings;
}

bool CEditorCtrl::Contains(const std::string& str, char ch)
{
	return str.find(ch) != std::string::npos;
}

bool CEditorCtrl::FindBraceMatchPos(Position& braceAtCaret, Position& braceOpposite)
{
	const auto IsBraceChar = [](int ch)
	{
		return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
	};

	const int lengthDoc = GetLength();
	const int pos = GetCurrentPos();
	bool isInside = false;
	bool isAfter = true;
	int charBefore = 0;

	braceAtCaret = -1;
	braceOpposite = -1;

	if (lengthDoc > 0 && pos > 0)
	{
		const Position posBefore = PositionBefore(pos);
		if (posBefore == pos - 1)
		{
			charBefore = GetCharAt(posBefore);
		}
	}

	if (charBefore && IsBraceChar(charBefore))
	{
		braceAtCaret = pos - 1;
	}

	if (lengthDoc > 0 && braceAtCaret < 0 && pos < lengthDoc)
	{
		const int charAfter = GetCharAt(pos);

		if (charAfter && IsBraceChar(charAfter))
		{
			braceAtCaret = pos;
			isAfter = false;
		}
	}

	if (braceAtCaret >= 0)
	{
		braceOpposite = BraceMatch(braceAtCaret, 0);
		isInside = braceOpposite > braceAtCaret ? isAfter : !isAfter;
	}
	return isInside;
}

bool CEditorCtrl::Find(bool next)
{
	Position pos = 0;
	FindOption flags = DlgFindReplace->m_flags;
	const string8 text = DlgFindReplace->m_find_text;

	if (next)
	{
		CharRight();
		SearchAnchor();
		pos = SearchNext(flags, text);
	}
	else
	{
		SearchAnchor();
		pos = SearchPrev(flags, text);
	}

	if (pos != -1)
	{
		// Scroll to view
		ScrollCaret();
		return true;
	}

	string8 msg;
	msg << "Cannot find \"" << text << "\"";
	uMessageBox(DlgFindReplace->m_hWnd, msg, jsp::component_name, MB_SYSTEMMODAL | MB_ICONINFORMATION);
	return false;
}

bool CEditorCtrl::Includes(const StyleAndWords& symbols, const std::string& value)
{
	if (symbols.words.empty())
	{
		return false;
	}
	else if (isalpha(symbols.words[0]))
	{
		const size_t lenVal = value.length();
		const char* symbol = symbols.words.c_str();

		while (symbol)
		{
			const char* symbolEnd = strchr(symbol, ' ');
			size_t lenSymbol = strlen(symbol);

			if (symbolEnd)
				lenSymbol = symbolEnd - symbol;

			if (lenSymbol == lenVal)
			{
				if (strncmp(symbol, value.c_str(), lenSymbol) == 0)
				{
					return true;
				}
			}

			symbol = symbolEnd;

			if (symbol)
				symbol++;
		}
		return false;
	}
	return Contains(value, symbols.words[0]);
}

bool CEditorCtrl::IsNumeric(const std::string& str)
{
	if (str.empty()) return false;
	return std::ranges::all_of(str, [](char c) { return isdigit(c) != 0; });
}

bool CEditorCtrl::RangeIsAllWhitespace(Position start, Position end)
{
	if (start < 0) start = 0;
	end = std::min(end, GetLength());

	for (Position i = start; i < end; ++i)
	{
		const int c = GetCharAt(i);
		if (c != ' ' && c != '\t') return false;
	}
	return true;
}

bool CEditorCtrl::StringComparePartial(jstring s1, jstring s2, size_t len)
{
	return _strnicmp(s1, s2, len) == 0;
}

int CEditorCtrl::IndentOfBlock(Line line)
{
	if (line < 0)
		return 0;

	const int indentSize = GetIndent();
	int indentBlock = GetLineIndentation(line);
	IndentationStatus indentState = IndentationStatus::isNone;
	Line backLine = line;

	Line lineLimit = line - 10;
	if (lineLimit < 0)
		lineLimit = 0;

	while (backLine >= lineLimit && indentState == IndentationStatus::isNone)
	{
		indentState = GetIndentState(backLine);

		if (indentState != IndentationStatus::isNone)
		{
			indentBlock = GetLineIndentation(backLine);

			if (indentState == IndentationStatus::isBlockStart)
			{
				indentBlock += indentSize;
			}

			if (indentState == IndentationStatus::isBlockEnd)
			{
				if (indentBlock < 0)
					indentBlock = 0;
			}

			if (indentState == IndentationStatus::isKeyWordStart && backLine == line)
				indentBlock += indentSize;
		}

		backLine--;
	}

	return indentBlock;
}

std::string CEditorCtrl::GetCurrentLine()
{
	const int len = GetCurLine(0, nullptr);
	std::string value(len, '\0');
	GetCurLine(len, value.data());
	return value;
}

std::string CEditorCtrl::GetNearestWord(const std::string& wordStart, size_t searchLen, int wordIndex)
{
	auto it = std::ranges::find_if(apis, [=](const API& item) { return StringComparePartial(wordStart, item.text, searchLen); });
	for (; it < apis.end(); ++it)
	{
		if (searchLen >= it->text.length() || !Contains(WordCharacters, it->text.at(searchLen)))
		{
			if (wordIndex <= 0)
			{
				return it->text;
			}
			wordIndex--;
		}
	}
	return std::string();
}

std::string CEditorCtrl::GetNearestWords(const std::string& wordStart, size_t searchLen)
{
	std::string words;
	auto it = std::ranges::find_if(apis, [=](const API& item) { return StringComparePartial(wordStart, item.text, searchLen); });
	for (; it < apis.end(); ++it)
	{
		if (!StringComparePartial(wordStart, it->text, searchLen))
		{
			break;
		}

		if (words.length()) words.append(" ");
		words.append(it->text, 0, it->len);
	}
	return words;
}

string8 CEditorCtrl::GetContent()
{
	const int len = GetTextLength();
	std::string value(len + 1, '\0');
	GetText(value.length(), value.data());
	return value.c_str();
}

void CEditorCtrl::AutoMarginWidth()
{
	int linecount = GetLineCount();
	int linenumwidth = 1;
	int marginwidth = 0;

	while (linecount >= 10)
	{
		linecount /= 10;
		++linenumwidth;
	}

	marginwidth = 4 + linenumwidth * (TextWidth(STYLE_LINENUMBER, "9"));

	if (marginwidth != GetMarginWidthN(0))
	{
		SetMarginWidthN(0, marginwidth);
	}
}

void CEditorCtrl::AutomaticIndentation(int ch)
{
	const int selStart = GetSelectionStart();
	const Line curLine = GetCurrentLineNumber();
	const Line prevLine = curLine - 1;
	const Position thisLineStart = PositionFromLine(curLine);
	const int indentSize = GetIndent();
	int indentBlock = IndentOfBlock(prevLine);

	if (curLine > 0)
	{
		bool foundBrace = false;
		int slen = LineLength(prevLine);
		std::string value(slen, '\0');
		GetLine(prevLine, value.data());

		for (int pos = slen - 1; pos >= 0 && value[pos]; --pos)
		{
			const char c = value[pos];

			if (c == '\t' || c == ' ' || c == '\r' || c == '\n')
			{
				continue;
			}
			else if (c == '{' || c == '[' || c == '(')
			{
				foundBrace = true;
			}

			break;
		}

		if (foundBrace)
			indentBlock += indentSize;
	}

	if (ch == '}')
	{
		if (RangeIsAllWhitespace(thisLineStart, selStart - 1))
		{
			SetIndentation(curLine, indentBlock - indentSize);
		}
	}
	else if (ch == '{')
	{
		if (GetIndentState(prevLine) == IndentationStatus::isKeyWordStart)
		{
			if (RangeIsAllWhitespace(thisLineStart, selStart - 1))
			{
				SetIndentation(curLine, indentBlock - indentSize);
			}
		}
	}
	else if ((ch == '\r' || ch == '\n') && selStart == thisLineStart)
	{
		const Strings controlWords = GetLinePartsInStyle(prevLine, BlockEnd);
		if (controlWords.size() && Includes(BlockEnd, controlWords[0]))
		{
			SetIndentation(prevLine, IndentOfBlock(prevLine - 1) - indentSize);
			indentBlock = IndentOfBlock(prevLine);
		}
		SetIndentation(curLine, indentBlock);
	}
}

void CEditorCtrl::ContinueCallTip()
{
	std::string line = GetCurrentLine();
	const Position current = GetCaretInLine();
	int braces = 0;
	int commas = 0;

	for (Position i = StartCalltipWord; i < current; ++i)
	{
		if (line[i] == '(')
			braces++;
		else if (line[i] == ')' && braces > 0)
			braces--;
		else if (braces == 1 && line[i] == ',')
			commas++;
	}

	size_t startHighlight = 0;
	while (startHighlight < FunctionDefinition.length() && FunctionDefinition[startHighlight] != '(')
	{
		startHighlight++;
	}
	if (startHighlight < FunctionDefinition.length() && FunctionDefinition[startHighlight] == '(')
		startHighlight++;

	while (startHighlight < FunctionDefinition.length() && commas > 0)
	{
		if (FunctionDefinition[startHighlight] == ',')
			commas--;
		if (FunctionDefinition[startHighlight] == ')')
			commas = 0;
		else
			startHighlight++;
	}

	if (startHighlight < FunctionDefinition.length() && FunctionDefinition[startHighlight] == ',')
		startHighlight++;

	size_t endHighlight = startHighlight;
	while (endHighlight < FunctionDefinition.length() && FunctionDefinition[endHighlight] != ',' && FunctionDefinition[endHighlight] != ')')
	{
		endHighlight++;
	}

	CallTipSetHlt(startHighlight, endHighlight);
}

void CEditorCtrl::FillFunctionDefinition(Position pos)
{
	if (pos > 0)
	{
		LastPosCallTip = pos;
	}

	std::string words = GetNearestWords(CurrentCallTipWord.c_str(), CurrentCallTipWord.length());
	if (words.length())
	{
		std::string word = GetNearestWord(CurrentCallTipWord.c_str(), CurrentCallTipWord.length(), CurrentCallTip);

		if (word.length())
		{
			FunctionDefinition = word;

			CallTipShow(LastPosCallTip - CurrentCallTipWord.length(), FunctionDefinition.c_str());
			ContinueCallTip();
		}
	}
}

void CEditorCtrl::Init()
{
	SetFnPtr();
	SetTechnology(Technology::DirectWrite);
	SetBufferedDraw(false);

	SetCodePage(CpUtf8);
	SetEOLMode(EndOfLine::CrLf);
	SetModEventMask(ModificationFlags::InsertText | ModificationFlags::DeleteText | ModificationFlags::Undo | ModificationFlags::Redo);
	UsePopUp(PopUp::Text);

	for (const auto& ctrlcode : ctrlcodes)
	{
		ClearCmdKey(MAKELONG(ctrlcode, KeyMod::Ctrl));
	}

	for (int i = 48; i < 122; ++i)
	{
		ClearCmdKey(MAKELONG(i, KeyMod::Ctrl | KeyMod::Shift));
	}

	// Shortcut keys
	AssignCmdKey(MAKELONG(Keys::Next, KeyMod::Ctrl), static_cast<int>(Message::ParaDown));
	AssignCmdKey(MAKELONG(Keys::Prior, KeyMod::Ctrl), static_cast<int>(Message::ParaUp));
	AssignCmdKey(MAKELONG(Keys::Next, (KeyMod::Ctrl | KeyMod::Shift)), static_cast<int>(Message::ParaDownExtend));
	AssignCmdKey(MAKELONG(Keys::Prior, (KeyMod::Ctrl | KeyMod::Shift)), static_cast<int>(Message::ParaUpExtend));
	AssignCmdKey(MAKELONG(Keys::Home, KeyMod::Norm), static_cast<int>(Message::VCHomeWrap));
	AssignCmdKey(MAKELONG(Keys::End, KeyMod::Norm), static_cast<int>(Message::LineEndWrap));
	AssignCmdKey(MAKELONG(Keys::Home, KeyMod::Shift), static_cast<int>(Message::VCHomeWrapExtend));
	AssignCmdKey(MAKELONG(Keys::End, KeyMod::Shift), static_cast<int>(Message::LineEndWrapExtend));

	// Tabs and indentation
	SetUseTabs(true);
	SetTabIndents(true);
	SetBackSpaceUnIndents(true);
	SetTabWidth(4);
	SetIndent(4);

	// Scroll
	SetEndAtLastLine(true);
	SetScrollWidthTracking(true);
	SetScrollWidth(1);

	// Auto complete
	AutoCSetIgnoreCase(true);

	// Lang
	SetILexer(CreateLexer("cpp"));
	SetKeyWords(0, js_keywords);
	ReadAPIs();

	// Enable line numbering
	SetMarginWidthN(1, 0);
	SetMarginWidthN(2, 0);
	SetMarginWidthN(3, 0);
	SetMarginWidthN(4, 0);
	SetMarginTypeN(0, MarginType::Number);

	// Clear and reset all styles
	ClearDocumentStyle();
	StyleResetDefault();

	// Style
	for (const auto& [key, value] : g_config.m_data)
	{
		if (value.empty()) continue;

		if (styles.contains(key))
		{
			EditorStyle style = ParseStyle(value);

			for (const int id : styles.at(key))
			{
				if (style.font.length()) StyleSetFont(id, style.font.c_str());
				if (style.size > 0) StyleSetSize(id, style.size);
				if (style.fore != INT_MAX) StyleSetFore(id, style.fore);
				if (style.back != INT_MAX) StyleSetBack(id, style.back);
				if (style.bold) StyleSetBold(id, style.bold);
				if (style.italic) StyleSetItalic(id, style.italic);
				if (id == STYLE_DEFAULT) StyleClearAll();
			}
		}
		else if (value.starts_with('#'))
		{
			ColourAlpha colour = 0xff000000 | ParseHex(value);
			if (key == "style.caret.fore")
			{
				SetElementColour(Element::Caret, colour);
			}
			else if (key == "style.selection.back")
			{
				SetSelectionLayer(Layer::UnderText);
				SetElementColour(Element::SelectionBack, colour);
				SetElementColour(Element::SelectionAdditionalBack, colour);
				SetElementColour(Element::SelectionInactiveBack, colour);
				SetElementColour(Element::SelectionSecondaryBack, colour);
			}
		}
	}
}

void CEditorCtrl::OpenFindDialog()
{
	if (!DlgFindReplace) DlgFindReplace = fb2k::newDialogEx<CDialogFindReplace>(m_hWnd, this);
	DlgFindReplace->SetMode(CDialogFindReplace::mode::find);
}

void CEditorCtrl::OpenGotoDialog()
{
	modal_dialog_scope scope(m_hWnd);
	CDialogGoto dlg(std::to_string(GetCurrentLineNumber() + 1));
	if (dlg.DoModal(m_hWnd) == IDOK)
	{
		const Line line = std::stoi(dlg.m_line_number.get_ptr()) - 1;
		GotoLine(line);
	}
}

void CEditorCtrl::OpenReplaceDialog()
{
	if (!DlgFindReplace) DlgFindReplace = fb2k::newDialogEx<CDialogFindReplace>(m_hWnd, this);
	DlgFindReplace->SetMode(CDialogFindReplace::mode::replace);
}

void CEditorCtrl::ReadAPIs()
{
	std::string content = Component::get_resource_text(IDR_API);

	for (const std::string& line : split_string(content, CRLF))
	{
		if (line.empty()) continue;
		API item = { line, std::min({line.find('('), line.find(' '), line.length()}) };
		apis.emplace_back(item);
	}

	std::ranges::sort(apis, [](const API& a, const API& b) -> bool
		{
			return _stricmp(a.text.c_str(), b.text.c_str()) < 0;
		});
}

void CEditorCtrl::Replace()
{
	const Range range = GetSelection();
	SetTargetStart(range.start);
	SetTargetEnd(range.end);
	const string8 replace = DlgFindReplace->m_replace_text;
	ReplaceTarget(replace.get_length(), replace);
	SetSel(range.start + replace.get_length(), range.end);
}

void CEditorCtrl::ReplaceAll()
{
	BeginUndoAction();
	SetTargetStart(0);
	SetTargetEnd(0);
	SetSearchFlags(DlgFindReplace->m_flags);
	const string8 find = DlgFindReplace->m_find_text;
	const string8 replace = DlgFindReplace->m_replace_text;

	while (true)
	{
		SetTargetStart(GetTargetEnd());
		SetTargetEnd(GetLength());

		const Position occurance = SearchInTarget(find.get_length(), find);

		if (occurance == -1)
		{
			MessageBeep(MB_ICONINFORMATION);
			break;
		}

		ReplaceTarget(replace.get_length(), replace);
		SetSel(occurance + replace.get_length(), occurance);
	}

	EndUndoAction();
}

void CEditorCtrl::SetContent(jstring text)
{
	SetText(text);
	ConvertEOLs(EndOfLine::CrLf);
	GrabFocus();
	TrackWidth();
}

void CEditorCtrl::SetIndentation(Line line, int indent)
{
	if (indent < 0)
		return;

	Range range = GetSelection();
	const Position posBefore = GetLineIndentPosition(line);
	SetLineIndentation(line, indent);
	const Position posAfter = GetLineIndentPosition(line);
	const Position posDifference = posAfter - posBefore;
	if (posAfter > posBefore)
	{
		if (range.start >= posBefore)
		{
			range.start += posDifference;
		}

		if (range.end >= posBefore)
		{
			range.end += posDifference;
		}
	}
	else if (posAfter < posBefore)
	{
		if (range.start >= posAfter)
		{
			if (range.start >= posBefore)
				range.start += posDifference;
			else
				range.start = posAfter;
		}

		if (range.end >= posAfter)
		{
			if (range.end >= posBefore)
				range.end += posDifference;
			else
				range.end = posAfter;
		}
	}

	SetSel(range.start, range.end);
}

void CEditorCtrl::StartAutoComplete()
{
	std::string line = GetCurrentLine();
	const Position current = GetCaretInLine();

	Position startword = current;

	while (startword > 0 && (Contains(WordCharacters, line[startword - 1]) || line[startword - 1] == '.'))
	{
		startword--;
	}

	std::string root = line.substr(startword, current - startword);

	std::string words = GetNearestWords(root.c_str(), root.length());
	if (words.length())
	{
		AutoCShow(root.length(), words.c_str());
	}
}

void CEditorCtrl::StartCallTip()
{
	CurrentCallTip = 0;
	CurrentCallTipWord = "";
	std::string line = GetCurrentLine();
	Position current = GetCaretInLine();
	Position pos = GetCurrentPos();
	int braces = 0;

	do
	{
		while (current > 0 && (braces || line[current - 1] != '('))
		{
			if (line[current - 1] == '(')
				braces--;
			else if (line[current - 1] == ')')
				braces++;

			current--;
			pos--;
		}

		if (current > 0)
		{
			current--;
			pos--;
		}
		else
		{
			break;
		}

		while (current > 0 && isspace(line[current - 1]))
		{
			current--;
			pos--;
		}
	} while (current > 0 && !Contains(WordCharacters, line[current - 1]));

	if (current <= 0) return;

	StartCalltipWord = current - 1;

	while (StartCalltipWord > 0 && (Contains(WordCharacters, line[StartCalltipWord - 1]) || line[StartCalltipWord - 1] == '.'))
	{
		--StartCalltipWord;
	}

	CurrentCallTipWord = line.substr(StartCalltipWord, current - StartCalltipWord);
	FunctionDefinition = "";
	FillFunctionDefinition(pos);
}

void CEditorCtrl::TrackWidth()
{
	int max_width = 1;

	for (int i = 0; i < GetLineCount(); ++i)
	{
		const Position pos = GetLineEndPosition(i);
		const int width = PointXFromPosition(pos);

		if (width > max_width)
			max_width = width;
	}

	SetScrollWidth(max_width);
}
