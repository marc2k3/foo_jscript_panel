#include "stdafx.h"
#include "Config.h"
#include "DialogConfigure.h"
#include "PanelWindow.h"

static const CDialogResizeHelper::Param resize_data[] =
{
	{ IDC_SCINTILLA, 0, 0, 1, 1 },
	{ IDC_BTN_RESET, 0, 1, 0, 1 },
	{ IDC_LABEL_EDGE, 0, 1, 0, 1 },
	{ IDC_COMBO_EDGE, 0, 1, 0, 1 },
	{ IDC_CHECK_PSEUDO_TRANSPARENT, 0, 1, 0, 1 },
	{ IDOK, 1, 1, 1, 1 },
	{ IDCANCEL, 1, 1, 1, 1 },
	{ IDC_BTN_APPLY, 1, 1, 1, 1 },
};

static const CRect resize_min_max(620, 400, 0, 0);

CDialogConfigure::CDialogConfigure(PanelWindow* panel) : m_panel(panel), m_resizer(resize_data, resize_min_max) {}

BOOL CDialogConfigure::OnInitDialog(CWindow, LPARAM)
{
	// Set caption text
	m_caption << jsp::component_name << " Configuration";
	if (m_panel->m_id != 0) m_caption << " (id:" << m_panel->m_id << ")";
	pfc::setWindowText(m_hWnd, m_caption);

	// Init
	BuildMenu();
	m_combo_edge = GetDlgItem(IDC_COMBO_EDGE);
	m_check_transparent = GetDlgItem(IDC_CHECK_PSEUDO_TRANSPARENT);

	// Apply window placement
	if (g_config.m_wndpl.length == 0)
	{
		g_config.m_wndpl.length = sizeof(WINDOWPLACEMENT);
		memset(&g_config.m_wndpl, 0, sizeof(WINDOWPLACEMENT));
	}
	else
	{
		SetWindowPlacement(&g_config.m_wndpl);
	}

	// Edge Style
	m_combo_edge.AddString(L"None");
	m_combo_edge.AddString(L"Sunken");
	m_combo_edge.AddString(L"Grey");

	if (m_panel->m_is_default_ui)
	{
		m_combo_edge.SetCurSel(0);
		m_combo_edge.EnableWindow(false);
	}
	else
	{
		m_combo_edge.SetCurSel(m_panel->m_config.m_style);
	}

	// Pseudo Transparent
	if (m_panel->m_supports_transparency)
	{
		m_check_transparent.SetCheck(m_panel->m_config.m_transparent);
	}
	else
	{
		m_check_transparent.SetCheck(false);
		m_check_transparent.EnableWindow(false);
	}

	// Edit Control
	m_editorctrl.SubclassWindow(GetDlgItem(IDC_SCINTILLA));
	m_editorctrl.Init();
	m_editorctrl.SetContent(m_panel->m_config.m_code);
	m_editorctrl.EmptyUndoBuffer();
	m_editorctrl.SetSavePoint();

	// Disable Apply button if spawned from CUI layout prefs
	if (m_panel->m_hwnd == nullptr)
	{
		GetDlgItem(IDC_BTN_APPLY).EnableWindow(false);
	}

	return FALSE;
}

LRESULT CDialogConfigure::OnNotify(int, LPNMHDR pnmh)
{
	switch (pnmh->code)
	{
	case SCN_SAVEPOINTLEFT:
		pfc::setWindowText(m_hWnd, PFC_string_formatter() << "*" << m_caption);
		break;
	case SCN_SAVEPOINTREACHED:
		pfc::setWindowText(m_hWnd, m_caption);
		break;
	}
	SetMsgHandled(FALSE);
	return 0;
}

void CDialogConfigure::Apply()
{
	// Save panel settings
	m_panel->m_config.m_style = m_combo_edge.GetCurSel();
	m_panel->m_config.m_transparent = m_check_transparent.IsChecked();

	// Get script text
	m_panel->m_config.m_code = m_editorctrl.GetContent();
	m_panel->update_script();

	// Save point
	m_editorctrl.SetSavePoint();
}

void CDialogConfigure::BuildMenu()
{
	CMenu menu = GetMenu();
	CMenu samples = CreateMenu();
	CMenu docs = CreateMenu();
	const std::wstring component_folder = Component::get_path();

	for (size_t i = ID_SAMPLES_BEGIN; const std::wstring& folder : FileHelper(component_folder + L"samples").list_folders())
	{
		CMenu sub = CreatePopupMenu();
		for (const std::filesystem::path& path : FileHelper(folder).list_files())
		{
			m_samples.emplace_back(path.wstring());
			sub.AppendMenu(MF_STRING, i++, path.stem().wstring().data());
		}
		samples.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(sub.m_hMenu), folder.substr(folder.find_last_of(L"\\") + 1).data());
	}

	for (size_t i = ID_DOCS_BEGIN; const std::filesystem::path& path : FileHelper(component_folder + L"docs").list_files())
	{
		m_docs.emplace_back(path.wstring());
		docs.AppendMenu(MF_STRING, i++, path.stem().wstring().data());
	}

	menu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(samples.m_hMenu), L"Samples");
	menu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(docs.m_hMenu), L"Docs");
}

void CDialogConfigure::OnCloseCmd(UINT, int nID, CWindow)
{
	GetWindowPlacement(&g_config.m_wndpl);

	switch (nID)
	{
	case IDOK:
		Apply();
		break;
	case IDC_BTN_APPLY:
		Apply();
		return;
	case IDCANCEL:
		if (m_editorctrl.GetModify() && uMessageBox(m_hWnd, "Unsaved changes will be lost. Are you sure?", jsp::component_name, MB_ICONWARNING | MB_SETFOREGROUND | MB_YESNO) != IDYES)
		{
			return;
		}
	}
	EndDialog(nID);
}

void CDialogConfigure::OnDocs(UINT, int nID, CWindow)
{
	ShellExecute(nullptr, L"open", m_docs[nID - ID_DOCS_BEGIN].data(), nullptr, nullptr, SW_SHOW);
}

void CDialogConfigure::OnFileImport(UINT, int, CWindow)
{
	string8 filename;
	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|JScript files|*.js|All files|*.*", 0, "txt", "Import from", nullptr, filename, FALSE))
	{
		m_editorctrl.SetContent(FileHelper(filename).read());
	}
}

void CDialogConfigure::OnFileExport(UINT, int, CWindow)
{
	string8 filename;
	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|All files|*.*", 0, "txt", "Save as", nullptr, filename, TRUE))
	{
		FileHelper(filename).write(m_editorctrl.GetContent());
	}
}

void CDialogConfigure::OnReset(UINT, int, CWindow)
{
	m_combo_edge.SetCurSel(0);
	m_check_transparent.SetCheck(false);
	m_editorctrl.SetContent(Component::get_resource_text(IDR_SCRIPT));
}

void CDialogConfigure::OnSamples(UINT, int nID, CWindow)
{
	m_editorctrl.SetContent(FileHelper(m_samples[nID - ID_SAMPLES_BEGIN]).read());
}
