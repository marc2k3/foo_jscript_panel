#pragma once
#include <ScintillaImpl.h>

class CDialogFindReplace;

class CEditorCtrl : public CScintillaImpl<CEditorCtrl>
{
public:
	CEditorCtrl();

	BEGIN_MSG_MAP_EX(CEditorCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(SCN_UPDATEUI, OnUpdateUI)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(SCN_CHARADDED, OnCharAdded)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(SCN_ZOOM, OnZoom)
		REFLECTED_COMMAND_CODE_HANDLER_EX(SCEN_CHANGE, OnChange)
	END_MSG_MAP()

	bool Find(bool next);
	string8 GetContent();
	void Init();
	void Replace();
	void ReplaceAll();
	void SetContent(stringp text);

private:
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

	enum class IndentationStatus
	{
		isNone,
		isBlockStart,
		isBlockEnd,
		isKeyWordStart
	};

	struct API
	{
		std::string text;
		size_t len;
	};

	struct EditorStyle
	{
		Colour back = 0, fore = 0;
		bool bold = false, italics = false, underlined = false;
		int case_force = 0, size = 0;
		size_t flags = 0;
		std::string font;
	};

	struct Range
	{
		Position start;
		Position end;
	};

	struct StringComparePartial
	{
		StringComparePartial(size_t len) : m_len(len) {}

		int operator()(const std::string& s1, const std::string& s2) const
		{
			const size_t len1 = pfc::strlen_max_t(s1.c_str(), m_len);
			const size_t len2 = pfc::strlen_max_t(s2.c_str(), m_len);

			return pfc::stricmp_ascii_ex(s1.c_str(), len1, s2.c_str(), len2);
		}

		size_t m_len;
	};

	struct StyleAndWords
	{
		bool IsEmpty() const { return words.length() == 0; }
		bool IsSingleChar() const { return words.length() == 1; }

		int styleNumber = 0;
		std::string words;
	};

	using APIs = std::vector<API>;

	Colour ParseHex(const std::string& hex);
	EditorStyle ParseStyle(const std::string& str);
	IndentationStatus GetIndentState(Line line);
	Line GetCurrentLineNumber();
	LRESULT OnChange(UINT, int, CWindow);
	LRESULT OnCharAdded(LPNMHDR);
	LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnUpdateUI(LPNMHDR);
	LRESULT OnZoom(LPNMHDR);
	Position GetCaretInLine();
	Range GetSelection();
	Strings GetLinePartsInStyle(Line line, const StyleAndWords& saw);
	bool Contains(const std::string& str, char ch);
	bool Includes(const StyleAndWords& symbols, const std::string& value);
	bool FindBraceMatchPos(Position& braceAtCaret, Position& braceOpposite);
	bool RangeIsAllWhitespace(Position start, Position end);
	int IndentOfBlock(Line line);
	std::string GetCurrentLine();
	std::string GetNearestWord(const std::string& wordStart, size_t searchLen, int wordIndex);
	std::string GetNearestWords(const std::string& wordStart, size_t searchLen);
	void AutoMarginWidth();
	void AutomaticIndentation(int ch);
	void ContinueCallTip();
	void FillFunctionDefinition(Position pos);
	void OpenFindDialog();
	void OpenGotoDialog();
	void OpenReplaceDialog();
	void ReadAPIs();
	void SetIndentation(Line line, int indent);
	void StartAutoComplete();
	void StartCallTip();
	void TrackWidth();

	APIs apis{};
	CDialogFindReplace* DlgFindReplace = nullptr;
	Position LastPosCallTip = 0;
	Position StartCalltipWord = 0;
	StyleAndWords BlockEnd;
	StyleAndWords BlockStart;
	StyleAndWords StatementEnd;
	StyleAndWords StatementIndent;
	int BraceCount = 0;
	int CurrentCallTip = 0;
	std::string CurrentCallTipWord;
	std::string FunctionDefinition;
	std::string WordCharacters = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
};
