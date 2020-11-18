#include "stdafx.h"
#include "Config.h"
#include "DialogProperties.h"

static const CDialogResizeHelper::Param resize_data[] =
{
	{ IDC_LIST_PROPERTIES, 0, 0, 1, 1 },
	{ IDC_BTN_IMPORT, 0, 1, 0, 1 },
	{ IDC_BTN_EXPORT, 0, 1, 0, 1 },
	{ IDC_BTN_CLEAR, 0, 1, 0, 1 },
	{ IDOK, 1, 1, 1, 1 },
	{ IDCANCEL, 1, 1, 1, 1 },
	{ IDC_BTN_APPLY, 1, 1, 1, 1 },
};

static const CRect resize_min_max(620, 400, 0, 0);

CDialogProperties::CDialogProperties(PanelWindow* panel) : m_panel(panel), m_resizer(resize_data, resize_min_max) {}

BOOL CDialogProperties::OnInitDialog(CWindow, LPARAM)
{
	// Set caption text
	m_caption << jsp::component_name << " Properties (id:" << m_panel->m_id << ")";
	pfc::setWindowText(m_hWnd, m_caption);

	// Apply window placement
	if (g_config.m_property_wndpl.length == 0)
	{
		g_config.m_property_wndpl.length = sizeof(WINDOWPLACEMENT);
		memset(&g_config.m_property_wndpl, 0, sizeof(WINDOWPLACEMENT));
	}
	else
	{
		SetWindowPlacement(&g_config.m_property_wndpl);
	}

	m_btn_clear = GetDlgItem(IDC_BTN_CLEAR);
	m_btn_export = GetDlgItem(IDC_BTN_EXPORT);

	m_properties.CreateInDialog(*this, IDC_LIST_PROPERTIES);
	LoadProperties();
	return FALSE;
}

void CDialogProperties::Apply()
{
	m_dup_prop_map.clear();

	for (const PropertyList::ListItem& item : m_properties.m_data)
	{
		if (item.is_bool)
		{
			m_dup_prop_map.emplace(item.key, _variant_t(item.bool_value));
		}
		else
		{
			_variant_t source(to_wide(item.value).data());
			_variant_t dest;

			if (!item.is_string && SUCCEEDED(VariantChangeType(&dest, &source, 0, VT_R8)))
			{
				m_dup_prop_map.emplace(item.key, dest);
			}
			else
			{
				m_dup_prop_map.emplace(item.key, source);
			}
		}
	}

	m_panel->m_config->m_properties->m_map = m_dup_prop_map;
	m_panel->update_script();
	LoadProperties();
}

void CDialogProperties::LoadProperties(bool reload)
{
	m_properties.m_data.clear();

	if (reload)
	{
		m_dup_prop_map = m_panel->m_config->m_properties->m_map;
	}

	for (const auto& [key, value] : m_dup_prop_map)
	{
		PropertyList::ListItem item;
		item.key = key;

		_variant_t var;

		switch (value.vt)
		{
		case VT_BOOL:
			item.is_bool = true;
			item.bool_value = to_bool(value.boolVal);
			break;

		case VT_I1:
		case VT_UI1:
		case VT_I2:
		case VT_UI2:
		case VT_I4:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
			var.ChangeType(VT_I4, &value);
			item.value = std::to_string(var.lVal).c_str();
			break;

		case VT_BSTR:
			item.is_string = true;
			var.ChangeType(VT_BSTR, &value);
			item.value = string_utf8_from_wide(var.bstrVal);
			break;

		default:
			var.ChangeType(VT_BSTR, &value);
			item.value = string_utf8_from_wide(var.bstrVal);
			break;
		}

		m_properties.m_data.emplace_back(item);
	}

	m_properties.ReloadData();

	m_btn_clear.EnableWindow(m_properties.m_data.size());
	m_btn_export.EnableWindow(m_properties.m_data.size());
}

void CDialogProperties::OnClearBnClicked(UINT, int, CWindow)
{
	m_properties.SelectAll();
	m_properties.RequestRemoveSelection();
}

void CDialogProperties::OnCloseCmd(UINT, int nID, CWindow)
{
	GetWindowPlacement(&g_config.m_property_wndpl);

	switch (nID)
	{
	case IDOK:
		Apply();
		break;

	case IDC_BTN_APPLY:
		Apply();
		return;
	}
	EndDialog(nID);
}

void CDialogProperties::OnExportBnClicked(UINT, int, CWindow)
{
	string8 path;

	if (uGetOpenFileName(m_hWnd, "Property files|*.wsp", 0, "wsp", "Save as", nullptr, path, TRUE))
	{
		try
		{
			file_ptr io;
			filesystem::g_open_write_new(io, path, fb2k::noAbort);
			PanelProperties::g_get(m_dup_prop_map, io.get_ptr(), fb2k::noAbort);
		}
		catch (...) {}
	}
}

void CDialogProperties::OnImportBnClicked(UINT, int, CWindow)
{
	string8 path;

	if (uGetOpenFileName(m_hWnd, "Property files|*.wsp|All files|*.*", 0, "wsp", "Import from", nullptr, path, FALSE))
	{
		try
		{
			file_ptr io;
			filesystem::g_open_read(io, path, fb2k::noAbort);
			PanelProperties::g_set(m_dup_prop_map, io.get_ptr(), fb2k::noAbort);
			LoadProperties(false);
		}
		catch (...) {}
	}
}