#pragma once
#include "EditorCtrl.h"

#include <libPPUI/CDialogResizeHelper.h>

class PanelWindow;

class CDialogConfigure : public CDialogImpl<CDialogConfigure>
{
public:
	CDialogConfigure(PanelWindow* panel);

	BEGIN_MSG_MAP_EX(CDialogConfigure)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_NOTIFY(OnNotify)
		COMMAND_ID_HANDLER_EX(ID_FILE_IMPORT, OnFileImport)
		COMMAND_ID_HANDLER_EX(ID_FILE_EXPORT, OnFileExport)
		COMMAND_ID_HANDLER_EX(IDC_BTN_RESET, OnReset)
		COMMAND_ID_HANDLER_EX(IDC_BTN_APPLY, OnCloseCmd)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
		COMMAND_RANGE_HANDLER_EX(ID_SAMPLES_BEGIN, ID_SAMPLES_END, OnSamples)
		COMMAND_RANGE_HANDLER_EX(ID_DOCS_BEGIN, ID_DOCS_END, OnDocs)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_CONFIGURE };

	BOOL OnInitDialog(CWindow, LPARAM);
	LRESULT OnNotify(int, LPNMHDR pnmh);
	void OnCloseCmd(UINT, int nID, CWindow);
	void OnDocs(UINT, int nID, CWindow);
	void OnFileImport(UINT, int, CWindow);
	void OnFileExport(UINT, int, CWindow);
	void OnReset(UINT, int, CWindow);
	void OnSamples(UINT, int nID, CWindow);

private:
	string8 GetText();
	void Apply();
	void BuildMenu();

	CCheckBox m_check_transparent;
	CComboBox m_combo_edge;
	CDialogResizeHelper m_resizer;
	CEditorCtrl m_editorctrl;
	PanelWindow* m_panel;
	Strings m_docs, m_samples;
	string8 m_caption;
};
