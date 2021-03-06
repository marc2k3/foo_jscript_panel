#include "stdafx.h"
#include "Config.h"

#include <libPPUI/CListControlOwnerData.h>
#include <libPPUI/Controls.h>

namespace
{
	class CDialogPreferences : public CDialogImpl<CDialogPreferences>, public preferences_page_instance, private IListControlOwnerDataSource
	{
	public:
		CDialogPreferences(preferences_page_callback::ptr callback) : m_callback(callback), m_list(this)
		{
			CSeparator::Register();
		}

		BEGIN_MSG_MAP_EX(CDialogPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_BTN_IMPORT, BN_CLICKED, OnImportBnClicked)
			COMMAND_HANDLER_EX(IDC_BTN_EXPORT, BN_CLICKED, OnExportBnClicked)
			COMMAND_HANDLER_EX(IDC_BTN_PRESETS, BN_CLICKED, OnPresetsBnClicked)
		END_MSG_MAP()

		enum { IDD = IDD_PREFERENCES };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			CreatePreferencesHeaderFont(m_font, *this);
			GetDlgItem(IDC_HEADER).SetFont(m_font.m_hFont);

			m_list.CreateInDialog(*this, IDC_LIST_PREFERENCES);
			const SIZE DPI = m_list.GetDPI();
			m_list.AddColumn("Name", MulDiv(150, DPI.cx, 96));
			m_list.AddColumn("Value", MulDiv(310, DPI.cx, 96));

			m_btn_presets = GetDlgItem(IDC_BTN_PRESETS);
			return FALSE;
		}

		HWND get_wnd() override
		{
			return m_hWnd;
		}

		uint32_t get_state() override
		{
			return preferences_state::resettable;
		}

		void OnExportBnClicked(UINT, int, CWindow)
		{
			string8 filename;
			if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg", 0, "cfg", "Save as", nullptr, filename, TRUE))
			{
				g_config.save(filename);
			}
		}

		void OnImportBnClicked(UINT, int, CWindow)
		{
			string8 filename;
			if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg|All files|*.*", 0, "cfg", "Import from", nullptr, filename, FALSE))
			{
				g_config.load(FileHelper(filename).read());
				m_list.ReloadData();
			}
		}

		void OnPresetsBnClicked(UINT, int, CWindow)
		{
			CMenu menu = CreatePopupMenu();
			menu.AppendMenu(MF_STRING, IDR_BRIGHT, L"Bright");
			menu.AppendMenu(MF_STRING, IDR_DARK, L"Dark");
			menu.AppendMenu(MF_STRING, IDR_RUBY, L"Ruby Blue");

			CRect rect;
			m_btn_presets.GetWindowRect(&rect);
			const int idx = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rect.left, rect.bottom, m_hWnd, nullptr);
			if (idx > 0)
			{
				g_config.load(Component::get_resource_text(idx));
				m_list.ReloadData();
			}
		}

		void apply() override
		{
			m_callback->on_state_changed();
		}

		void reset() override
		{
			g_config.init_data();
			m_list.ReloadData();
		}

	private:
		bool listCanSelectItem(ctx_t, size_t) override
		{
			return false;
		}

		bool listIsColumnEditable(ctx_t, size_t column) override
		{
			return column == 1;
		}

		size_t listGetItemCount(ctx_t) override
		{
			return g_config.m_data.size();
		}

		string8 listGetSubItemText(ctx_t, size_t row, size_t column) override
		{
			switch (column)
			{
			case 0:
				return g_config.m_data[row].key.c_str();
			case 1:
				return g_config.m_data[row].value.c_str();
			}
			return "";
		}

		void listSetEditField(ctx_t, size_t row, size_t column, const char* value) override
		{
			if (column == 1)
			{
				g_config.m_data[row].value = value;
			}
		}

		void listSubItemClicked(ctx_t, size_t row, size_t column) override
		{
			if (column == 1)
			{
				m_list.TableEdit_Start(row, column);
			}
		}

		CButton m_btn_presets;
		CFont m_font;
		CListControlOwnerData m_list;
		preferences_page_callback::ptr m_callback;
	};

	class PreferencesPageImpl : public preferences_page_impl<CDialogPreferences>
	{
	public:
		GUID get_guid() override
		{
			return guids::preferences_page;
		}

		GUID get_parent_guid() override
		{
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base& out) override
		{
			out.set_string("https://github.com/marc2k3/foo_jscript_panel");
			return true;
		}

		const char* get_name() override
		{
			return jsp::component_name;
		}
	};

	FB2K_SERVICE_FACTORY(PreferencesPageImpl);
}
