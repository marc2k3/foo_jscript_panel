#pragma once

class CDialogGoto : public CDialogImpl<CDialogGoto>
{
public:
	CDialogGoto(stringp line_number) : m_line_number(line_number) {}

	BEGIN_MSG_MAP_EX(CDialogGoto)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_GOTO };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_edit_line_number = GetDlgItem(IDC_EDIT_LINE_NUMBER);
		pfc::setWindowText(m_edit_line_number, m_line_number);
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		m_line_number = pfc::getWindowText(m_edit_line_number);
		EndDialog(nID);
	}

	CEdit m_edit_line_number;
	string8 m_line_number;
};
