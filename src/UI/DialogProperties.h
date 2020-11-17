#pragma once
#include "PanelWindow.h"
#include "PropertyCtrl.h"

#include <libPPUI/CDialogResizeHelper.h>

class CDialogProperties : public CDialogImpl<CDialogProperties>
{
public:
	CDialogProperties(PanelWindow* panel);

	BEGIN_MSG_MAP_EX(CDialogProperties)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDC_BTN_APPLY, OnCloseCmd)
		COMMAND_HANDLER_EX(IDC_BTN_CLEAR, BN_CLICKED, OnClearBnClicked)
		COMMAND_HANDLER_EX(IDC_BTN_EXPORT, BN_CLICKED, OnExportBnClicked)
		COMMAND_HANDLER_EX(IDC_BTN_IMPORT, BN_CLICKED, OnImportBnClicked)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_PROPERTIES };

	BOOL OnInitDialog(CWindow, LPARAM);
	void Apply();
	void LoadProperties(bool reload = true);
	void OnClearBnClicked(UINT, int, CWindow);
	void OnCloseCmd(UINT, int nid, CWindow);
	void OnExportBnClicked(UINT, int, CWindow);
	void OnImportBnClicked(UINT, int, CWindow);

private:
	CButton m_btn_clear, m_btn_export;
	CDialogResizeHelper m_resizer;
	PanelProperties::PropertyMap m_dup_prop_map;
	PanelWindow* m_panel;
	PropertyList m_properties;
	string8 m_caption;
};
