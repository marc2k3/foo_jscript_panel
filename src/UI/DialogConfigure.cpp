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
	if (m_panel->m_hwnd != nullptr) m_caption << " (id:" << m_panel->m_id << ")";
	pfc::setWindowText(m_hWnd, m_caption);

	// Init
	m_combo_edge = GetDlgItem(IDC_COMBO_EDGE);
	m_check_transparent = GetDlgItem(IDC_CHECK_PSEUDO_TRANSPARENT);

	BuildMenu();

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
		m_combo_edge.SetCurSel(static_cast<int>(m_panel->m_config->m_style));
	}

	// Pseudo Transparent
	if (m_panel->m_supports_transparency)
	{
		m_check_transparent.SetCheck(m_panel->m_config->m_transparent);
	}
	else
	{
		m_check_transparent.SetCheck(false);
		m_check_transparent.EnableWindow(false);
	}

	// Edit Control
	m_editorctrl.SubclassWindow(GetDlgItem(IDC_SCINTILLA));
	m_editorctrl.Init();
	m_editorctrl.SetContent(m_panel->m_config->m_code);
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

string8 CDialogConfigure::GetText()
{
	const int len = m_editorctrl.GetTextLength();
	std::string value(len + 1, '\0');
	m_editorctrl.GetText(value.length(), value.data());
	return value.c_str();
}

void CDialogConfigure::Apply()
{
	// Save panel settings
	m_panel->m_config->m_style = static_cast<PanelConfig::EdgeStyle>(m_combo_edge.GetCurSel());
	m_panel->m_config->m_transparent = m_check_transparent.IsChecked();

	// Get script text
	m_panel->m_config->m_code = GetText();
	m_panel->update_script();

	// Save point
	m_editorctrl.SetSavePoint();
}

void CDialogConfigure::BuildMenu()
{
	auto display = [](const std::string& path)
	{
		string8 command;
		uFixAmpersandChars_v2(pfc::string_filename(path.c_str()), command);
		return command;
	};

	const std::string component_folder = helpers::get_fb2k_component_path().get_ptr();
	CMenu menu = GetMenu();

	CMenu samples = CreateMenu();
	size_t counter = 0;

	for (const std::string& folder : helpers::list_folders(component_folder + "samples"))
	{
		CMenu sub = CreatePopupMenu();
		for (const std::string& file : helpers::list_files(folder))
		{
			m_samples.emplace_back(file);
			uAppendMenu(sub, MF_STRING, ID_SAMPLES_BEGIN + counter, display(file));
			counter++;
		}
		uAppendMenu(samples, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(sub.m_hMenu), folder.substr(folder.find_last_of("\\") + 1).c_str());
	}
	uAppendMenu(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(samples.m_hMenu), "Samples");

	CMenu docs = CreateMenu();
	m_docs = helpers::list_files(component_folder + "docs");
	for (size_t i = 0; i < m_docs.size(); ++i)
	{
		uAppendMenu(docs, MF_STRING, ID_DOCS_BEGIN + i, display(m_docs[i]));
	}
	uAppendMenu(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(docs.m_hMenu), "Docs");
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
	ShellExecute(nullptr, L"open", to_wide(m_docs[nID - ID_DOCS_BEGIN].c_str()).data(), nullptr, nullptr, SW_SHOW);
}

void CDialogConfigure::OnFileImport(UINT, int, CWindow)
{
	string8 filename;
	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|JScript files|*.js|All files|*.*", 0, "txt", "Import from", nullptr, filename, FALSE))
	{
		m_editorctrl.SetContent(helpers::read_file(filename));
	}
}

void CDialogConfigure::OnFileExport(UINT, int, CWindow)
{
	string8 filename;
	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|All files|*.*", 0, "txt", "Save as", nullptr, filename, TRUE))
	{
		helpers::write_file(filename, GetText());
	}
}

void CDialogConfigure::OnReset(UINT, int, CWindow)
{
	m_combo_edge.SetCurSel(0);
	m_check_transparent.SetCheck(false);
	m_editorctrl.SetContent(helpers::get_resource_text(IDR_SCRIPT));
}

void CDialogConfigure::OnSamples(UINT, int nID, CWindow)
{
	m_editorctrl.SetContent(helpers::read_file(m_samples[nID - ID_SAMPLES_BEGIN].c_str()));
}
