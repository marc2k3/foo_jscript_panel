#pragma once

class CInputBox : public CDialogImpl<CInputBox>
{
public:
	CInputBox(stringp prompt, stringp caption, stringp value) : m_prompt(prompt), m_caption(caption), m_value(value) {}

	BEGIN_MSG_MAP_EX(CInputBox)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_INPUT };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_label_prompt = GetDlgItem(IDC_LABEL_PROMPT);
		m_edit_value = GetDlgItem(IDC_EDIT_VALUE);

		pfc::setWindowText(m_hWnd, m_caption);
		pfc::setWindowText(m_label_prompt, m_prompt);
		pfc::setWindowText(m_edit_value, m_value);
		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		m_value = pfc::getWindowText(m_edit_value);
		EndDialog(nID);
	}

	CEdit m_edit_value;
	CWindow m_label_prompt;
	string8 m_caption, m_prompt, m_value;
};
