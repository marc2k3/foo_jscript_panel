#pragma once
#include <ScintillaImpl.h>

class CDialogFindReplace;

class CEditorCtrl : public CScintillaImpl<CEditorCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CEditorCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(static_cast<int>(Notification::UpdateUI), OnUpdateUI)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(static_cast<int>(Notification::CharAdded), OnCharAdded)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(static_cast<int>(Notification::Zoom), OnZoom)
		REFLECTED_COMMAND_CODE_HANDLER_EX(static_cast<int>(FocusChange::Change), OnChange)
	END_MSG_MAP()

	bool Find(bool next);
	string8 GetContent();
	void Init();
	void Replace();
	void ReplaceAll();
	void SetContent(jstring text);

private:
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
		size_t len = 0;
	};

	struct EditorStyle
	{
		Colour back = INT_MAX, fore = INT_MAX;
		bool bold = false, italic = false;
		int size = 0;
		std::string font;
	};

	struct Range
	{
		Position start;
		Position end;
	};

	struct StyleAndWords
	{
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
	bool FindBraceMatchPos(Position& braceAtCaret, Position& braceOpposite);
	bool Includes(const StyleAndWords& symbols, const std::string& value);
	bool IsNumeric(const std::string& str);
	bool RangeIsAllWhitespace(Position start, Position end);
	bool StringComparePartial(jstring s1, jstring s2, size_t len);
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
