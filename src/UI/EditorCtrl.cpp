#include "stdafx.h"
#include "Config.h"
#include "DialogFindReplace.h"
#include "DialogGoto.h"
#include "EditorCtrl.h"

// Large portions taken from SciTE
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>

enum
{
	ESF_NONE = 0,
	ESF_FONT = 1 << 0,
	ESF_SIZE = 1 << 1,
	ESF_FORE = 1 << 2,
	ESF_BACK = 1 << 3,
	ESF_BOLD = 1 << 4,
	ESF_ITALICS = 1 << 5,
	ESF_UNDERLINED = 1 << 6,
	ESF_CASEFORCE = 1 << 7,
};

struct StringComparePartialNC
{
	StringComparePartialNC(size_t len) : m_len(len) {}

	int operator()(const std::string& s1, const std::string& s2) const
	{
		const size_t len1 = pfc::strlen_max_t(s1.c_str(), m_len);
		const size_t len2 = pfc::strlen_max_t(s2.c_str(), m_len);

		return pfc::stricmp_ascii_ex(s1.c_str(), len1, s2.c_str(), len2);
	}

	size_t m_len;
};

static constexpr const char* js_keywords = "abstract boolean break byte case catch char class const continue"
	" debugger default delete do double else enum export extends false final"
	" finally float for function goto if implements import in instanceof int"
	" interface long native new null package private protected public return"
	" short static super switch synchronized this throw throws transient true"
	" try typeof var void while with enum byvalue cast future generic inner"
	" operator outer rest Array Math RegExp window fb gdi utils plman console";

static constexpr std::array<const int, 21> ctrlcodes = { 'Q', 'W', 'E', 'R', 'I', 'O', 'P', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'B', 'N', 'M', 186, 187, 226 };

struct Style
{
	int id;
	const char* name;
};

static const std::vector<Style> styles =
{
	{ STYLE_DEFAULT, "style.default" },
	{ STYLE_LINENUMBER, "style.linenumber" },
	{ STYLE_BRACELIGHT, "style.bracelight" },
	{ STYLE_BRACEBAD, "style.bracebad" },
	{ SCE_C_COMMENT, "style.comment" },
	{ SCE_C_COMMENTLINE, "style.comment" },
	{ SCE_C_COMMENTDOC, "style.comment" },
	{ SCE_C_COMMENTLINEDOC, "style.comment" },
	{ SCE_C_COMMENTDOCKEYWORD, "style.comment" },
	{ SCE_C_COMMENTDOCKEYWORDERROR, "style.comment" },
	{ SCE_C_WORD, "style.keyword" },
	{ SCE_C_IDENTIFIER, "style.indentifier" },
	{ SCE_C_NUMBER, "style.number" },
	{ SCE_C_STRING, "style.string" },
	{ SCE_C_CHARACTER, "style.string" },
	{ SCE_C_OPERATOR, "style.operator" }
};

CEditorCtrl::CEditorCtrl() {}

CEditorCtrl::Colour CEditorCtrl::ParseHex(const std::string& hex)
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
		else
		{
			return 0;
		}
	};

	const auto int_from_hex_byte = [int_from_hex_digit](const char* hex_byte)
	{
		return (int_from_hex_digit(hex_byte[0]) << 4) | (int_from_hex_digit(hex_byte[1]));
	};

	if (hex.length() > 7) return 0;

	const int r = int_from_hex_byte(hex.c_str() + 1);
	const int g = int_from_hex_byte(hex.c_str() + 3);
	const int b = int_from_hex_byte(hex.c_str() + 5);

	return RGB(r, g, b);
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

LRESULT CEditorCtrl::OnChange(UINT, int, CWindow)
{
	AutoMarginWidth();
	return 0;
}

LRESULT CEditorCtrl::OnCharAdded(LPNMHDR pnmh)
{
	const Range range = GetSelection();
	const SCNotification* notification = reinterpret_cast<SCNotification*>(pnmh);
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
	const int modifiers = (IsKeyPressed(VK_SHIFT) ? SCMOD_SHIFT : 0) | (IsKeyPressed(VK_CONTROL) ? SCMOD_CTRL : 0) | (IsKeyPressed(VK_MENU) ? SCMOD_ALT : 0);

	if (modifiers == SCMOD_CTRL)
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
	else if (wParam == VK_F3 && (modifiers == 0 || modifiers == SCMOD_SHIFT))
	{
		if (!DlgFindReplace || DlgFindReplace->m_find_text.is_empty())
		{
			OpenFindDialog();
		}
		else
		{
			if (modifiers == 0) // Next
			{
				Find(true);
			}
			else if (modifiers == SCMOD_SHIFT) // Previous
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

CEditorCtrl::Position CEditorCtrl::GetCaretInLine()
{
	return GetCurrentPos() - PositionFromLine(GetCurrentLineNumber());
}

CEditorCtrl::Range CEditorCtrl::GetSelection()
{
	Range range;
	range.start = GetSelectionStart();
	range.end = GetSelectionEnd();
	return range;
}

Strings CEditorCtrl::GetLinePartsInStyle(Line line, const StyleAndWords& saw)
{
	Strings sv;
	const Position thisLineStart = PositionFromLine(line);
	const Position nextLineStart = PositionFromLine(line + 1);
	const bool separateCharacters = saw.IsSingleChar();
	std::string s;

	for (Position pos = thisLineStart; pos < nextLineStart; ++pos)
	{
		if (GetStyleAt(pos) == saw.styleNumber)
		{
			if (separateCharacters)
			{
				if (s.length() > 0)
				{
					sv.emplace_back(s);
				}
				s = "";
			}
			s += GetCharAt(pos);
		}
		else if (s.length() > 0)
		{
			sv.emplace_back(s);
			s = "";
		}
	}

	if (s.length() > 0)
	{
		sv.emplace_back(s);
	}

	return sv;
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

	const int pos = GetCurrentPos();
	bool isInside = false;

	braceAtCaret = -1;
	braceOpposite = -1;

	int charBefore = 0;
	const int lengthDoc = GetLength();

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

	bool isAfter = true;

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

		if (braceOpposite > braceAtCaret)
			isInside = isAfter;
		else
			isInside = !isAfter;
	}

	return isInside;
}

bool CEditorCtrl::Find(bool next)
{
	Position pos = 0;
	const int flags = DlgFindReplace->m_flags;
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

bool CEditorCtrl::GetPropertyEx(const std::string& key, std::string& out)
{
	out.clear();
	const int len = GetPropertyExpanded(key.c_str(), nullptr);
	if (len == 0) return false;

	out.assign(len, '\0');
	GetPropertyExpanded(key.c_str(), out.data());
	return true;
}

bool CEditorCtrl::Includes(const StyleAndWords& symbols, const std::string& value)
{
	if (symbols.IsEmpty())
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
	}
	else
	{
		return Contains(value, symbols.words[0]);
	}
	return false;
}

bool CEditorCtrl::ParseStyle(const std::string& definition, EditorStyle& style)
{
	if (definition.empty()) return false;

	for (const std::string& value : helpers::split_string(definition, ","))
	{
		Strings tmp = helpers::split_string(value, ":");
		std::string primary = tmp[0];
		std::string secondary = tmp.size() == 2 ? tmp[1] : "";

		if (primary.compare("italics") == 0)
		{
			style.flags |= ESF_ITALICS;
			style.italics = true;
		}
		else if (primary.compare("notitalics") == 0)
		{
			style.flags |= ESF_ITALICS;
			style.italics = false;
		}
		else if (primary.compare("bold") == 0)
		{
			style.flags |= ESF_BOLD;
			style.bold = true;
		}
		else if (primary.compare("notbold") == 0)
		{
			style.flags |= ESF_BOLD;
			style.bold = false;
		}
		else if (primary.compare("font") == 0)
		{
			style.flags |= ESF_FONT;
			style.font = secondary;
		}
		else if (primary.compare("fore") == 0)
		{
			style.flags |= ESF_FORE;
			style.fore = ParseHex(secondary);
		}
		else if (primary.compare("back") == 0)
		{
			style.flags |= ESF_BACK;
			style.back = ParseHex(secondary);
		}
		else if (primary.compare("size") == 0 && secondary.length())
		{
			style.flags |= ESF_SIZE;
			style.size = atoi(secondary.c_str());
		}
		else if (primary.compare("underlined") == 0)
		{
			style.flags |= ESF_UNDERLINED;
			style.underlined = true;
		}
		else if (primary.compare("notunderlined") == 0)
		{
			style.flags |= ESF_UNDERLINED;
			style.underlined = false;
		}
		else if (primary.compare("case") == 0)
		{
			style.flags |= ESF_CASEFORCE;
			style.case_force = SC_CASE_MIXED;

			if (secondary.length())
			{
				if (secondary.at(0) == 'u')
					style.case_force = SC_CASE_UPPER;
				else if (secondary.at(0) == 'l')
					style.case_force = SC_CASE_LOWER;
			}
		}
	}
	return true;
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

CEditorCtrl::Line CEditorCtrl::GetCurrentLineNumber()
{
	return LineFromPosition(GetCurrentPos());
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
	auto it = FIND_IF(apis, [=](const API& item) { return StringComparePartialNC(searchLen)(wordStart, item.text) == 0; });
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
	auto it = FIND_IF(apis, [=](const API& item) { return StringComparePartialNC(searchLen)(wordStart, item.text) == 0; });
	for (; it < apis.end(); ++it)
	{
		if (StringComparePartialNC(searchLen)(wordStart, it->text) != 0)
		{
			break;
		}

		if (words.length()) words.append(" ");
		words.append(it->text, 0, it->len);
	}
	return words;
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

	SetCodePage(SC_CP_UTF8);
	SetEOLMode(SC_EOL_CRLF);
	SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT | SC_PERFORMED_UNDO | SC_PERFORMED_REDO);
	UsePopUp(SC_POPUP_TEXT);

	for (const auto& ctrlcode : ctrlcodes)
	{
		ClearCmdKey(MAKELONG(ctrlcode, SCMOD_CTRL));
	}

	for (int i = 48; i < 122; ++i)
	{
		ClearCmdKey(MAKELONG(i, SCMOD_CTRL | SCMOD_SHIFT));
	}

	// Shortcut keys
	AssignCmdKey(MAKELONG(SCK_NEXT, SCMOD_CTRL), SCI_PARADOWN);
	AssignCmdKey(MAKELONG(SCK_PRIOR, SCMOD_CTRL), SCI_PARAUP);
	AssignCmdKey(MAKELONG(SCK_NEXT, (SCMOD_CTRL | SCMOD_SHIFT)), SCI_PARADOWNEXTEND);
	AssignCmdKey(MAKELONG(SCK_PRIOR, (SCMOD_CTRL | SCMOD_SHIFT)), SCI_PARAUPEXTEND);
	AssignCmdKey(MAKELONG(SCK_HOME, SCMOD_NORM), SCI_VCHOMEWRAP);
	AssignCmdKey(MAKELONG(SCK_END, SCMOD_NORM), SCI_LINEENDWRAP);
	AssignCmdKey(MAKELONG(SCK_HOME, SCMOD_SHIFT), SCI_VCHOMEWRAPEXTEND);
	AssignCmdKey(MAKELONG(SCK_END, SCMOD_SHIFT), SCI_LINEENDWRAPEXTEND);

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
	SetMarginTypeN(0, SC_MARGIN_NUMBER);

	// Load properties
	for (const auto& [key, value] : g_config.m_data)
	{
		SetProperty(key, value);
	}

	// Clear and reset all styles
	ClearDocumentStyle();
	StyleResetDefault();

	// Style
	SetCaretLineBackAlpha(GetPropertyInt("style.caret.line.back.alpha", SC_ALPHA_NOALPHA));
	SetCaretLineVisible(false);
	SetCaretWidth(GetPropertyInt("style.caret.width", 1));
	SetSelAlpha(GetPropertyInt("style.selection.alpha", SC_ALPHA_NOALPHA));
	SetSelFore(false, 0);

	std::string colour;

	if (GetPropertyEx("style.selection.fore", colour))
	{
		SetSelFore(true, ParseHex(colour));
		SetSelBack(true, RGB(0xc0, 0xc0, 0xc0));
	}

	if (GetPropertyEx("style.selection.back", colour))
	{
		SetSelBack(true, ParseHex(colour));
	}

	if (GetPropertyEx("style.caret.fore", colour))
	{
		SetCaretFore(ParseHex(colour));
	}

	if (GetPropertyEx("style.caret.line.back", colour))
	{
		SetCaretLineVisible(true);
		SetCaretLineBack(ParseHex(colour));
	}

	for (const auto& [id, name] : styles)
	{
		std::string value;
		if (GetPropertyEx(name, value))
		{
			EditorStyle style;
			if (!ParseStyle(value, style)) continue;

			if (style.flags & ESF_FONT) StyleSetFont(id, style.font.c_str());
			if (style.flags & ESF_SIZE) StyleSetSize(id, style.size);
			if (style.flags & ESF_FORE) StyleSetFore(id, style.fore);
			if (style.flags & ESF_BACK) StyleSetBack(id, style.back);
			if (style.flags & ESF_ITALICS) StyleSetItalic(id, style.italics);
			if (style.flags & ESF_BOLD) StyleSetBold(id, style.bold);
			if (style.flags & ESF_UNDERLINED) StyleSetUnderline(id, style.underlined);
			if (style.flags & ESF_CASEFORCE) StyleSetCase(id, style.case_force);
		}
		if (id == STYLE_DEFAULT) StyleClearAll();
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
	CDialogGoto dlg(std::to_string(GetCurrentLineNumber() + 1).c_str());
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
	std::string content = helpers::get_resource_text(IDR_API).get_ptr();

	for (const std::string& line : helpers::split_string(content, CRLF))
	{
		if (line.empty()) continue;
		API item = { line, std::min({line.find('('), line.find(' '), line.length()}) };
		apis.emplace_back(item);
	}

	std::sort(apis.begin(), apis.end(), [](const API& a, const API& b) -> bool
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

void CEditorCtrl::SetContent(stringp text)
{
	SetText(text);
	ConvertEOLs(SC_EOL_CRLF);
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

	line.at(current) = '\0';
	CurrentCallTipWord = line.c_str() + StartCalltipWord;
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
