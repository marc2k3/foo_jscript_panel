#include "stdafx.h"
#include "DialogFindReplace.h"

static const std::map<int, int> flags =
{
	{ IDC_CHECK_MATCHCASE, SCFIND_MATCHCASE },
	{ IDC_CHECK_WHOLEWORD, SCFIND_WHOLEWORD },
	{ IDC_CHECK_WORDSTART, SCFIND_WORDSTART },
	{ IDC_CHECK_REGEXP, SCFIND_REGEXP | SCFIND_CXX11REGEX }
};

static constexpr std::array ids =
{
	IDC_EDIT_FIND,
	IDC_EDIT_REPLACE,
	IDC_CHECK_MATCHCASE,
	IDC_CHECK_WHOLEWORD,
	IDC_CHECK_WORDSTART,
	IDC_CHECK_REGEXP,
	IDC_BTN_NEXT,
	IDC_BTN_PREVIOUS,
	IDC_BTN_REPLACE,
	IDC_BTN_REPLACE_ALL,
	IDC_LABEL_REPLACE,
	IDCANCEL
};

KeyHack::KeyHack() {}

BOOL KeyHack::SubclassWindow(CWindow hwnd, int cmd)
{
	m_cmd = cmd;
	return __super::SubclassWindow(hwnd);
}

LRESULT KeyHack::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	switch (wParam)
	{
	case '\n':
	case '\r':
	case '\t':
	case '\x1b':
		return 0;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT KeyHack::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	switch (wParam)
	{
	case VK_RETURN:
		GetParent().PostMessage(WM_COMMAND, MAKEWPARAM(m_cmd, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
		return 0;
	case VK_ESCAPE:
		GetParent().PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
		return 0;
	case VK_TAB:
		GetParent().PostMessage(WM_NEXTDLGCTL, IsKeyPressed(VK_SHIFT) ? 1 : 0);
		return 0;
	}
	return DefWindowProc(uMsg, wParam, lParam);
}

CDialogFindReplace::CDialogFindReplace(CEditorCtrl* parent) : m_parent(parent)
{
	m_hacks.assign(ids.size(), KeyHack());
}

BOOL CDialogFindReplace::OnInitDialog(CWindow, LPARAM)
{
	for (size_t i = 0; i < ids.size(); ++i)
	{
		const int id = ids[i];
		CWindow hwnd = GetDlgItem(id);
		m_window.emplace(id, hwnd);
		m_hacks[i].SubclassWindow(hwnd, id >= IDC_EDIT_FIND && id <= IDC_BTN_NEXT ? IDC_BTN_NEXT : id);
	}
	return TRUE;
}

void CDialogFindReplace::OnCancel(UINT, int, CWindow)
{
	ShowWindow(SW_HIDE);
}

void CDialogFindReplace::OnFindNext(UINT, int, CWindow)
{
	m_havefound = m_parent->Find(true);
}

void CDialogFindReplace::OnFindPrevious(UINT, int, CWindow)
{
	m_parent->Find(false);
}

void CDialogFindReplace::OnFindTextChange(UINT, int, CWindow)
{
	m_find_text = pfc::getWindowText(m_window.at(IDC_EDIT_FIND));
	const bool enabled = m_find_text.get_length() > 0;
	m_window.at(IDC_BTN_NEXT).EnableWindow(enabled);
	m_window.at(IDC_BTN_PREVIOUS).EnableWindow(enabled);
	m_window.at(IDC_BTN_REPLACE).EnableWindow(enabled);
	m_window.at(IDC_BTN_REPLACE_ALL).EnableWindow(enabled);
}

void CDialogFindReplace::OnFlagCommand(UINT, int nID, CWindow)
{
	if (uButton_GetCheck(m_hWnd, nID))
		m_flags |= flags.at(nID);
	else
		m_flags &= ~flags.at(nID);
}

void CDialogFindReplace::OnReplace(UINT, int, CWindow)
{
	if (m_havefound)
	{
		m_parent->Replace();
		m_havefound = false;
	}

	OnFindNext(0, 0, nullptr);
}

void CDialogFindReplace::OnReplaceAll(UINT, int, CWindow)
{
	m_parent->ReplaceAll();
}

void CDialogFindReplace::OnReplaceTextChange(UINT, int, CWindow)
{
	m_replace_text = pfc::getWindowText(m_window.at(IDC_EDIT_REPLACE));
}

void CDialogFindReplace::SetMode(mode m)
{
	const bool find = m == mode::find;
	const int cmd = find ? SW_HIDE : SW_SHOW;
	m_window.at(IDC_LABEL_REPLACE).ShowWindow(cmd);
	m_window.at(IDC_EDIT_REPLACE).ShowWindow(cmd);
	m_window.at(IDC_BTN_REPLACE).ShowWindow(cmd);
	m_window.at(IDC_BTN_REPLACE_ALL).ShowWindow(cmd);

	pfc::setWindowText(m_hWnd, find ? "Find" : "Replace");
	ShowWindow(SW_SHOW);
	SetFocus();
}
