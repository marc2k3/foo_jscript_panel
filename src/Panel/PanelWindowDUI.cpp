#include "stdafx.h"
#include "PanelWindow.h"

namespace
{
	class PanelWindowDUI : public PanelWindow, public ui_element_instance, public CWindowImpl<PanelWindowDUI>
	{
	public:
		DECLARE_WND_CLASS_EX(L"jsp_class_dui", CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, -1);

		PanelWindowDUI(ui_element_config::ptr cfg, ui_element_instance_callback::ptr callback) : m_callback(callback)
		{
			set_configuration(cfg);
		}

		static GUID g_get_guid()
		{
			return guids::window_dui;
		}

		static GUID g_get_subclass()
		{
			return ui_element_subclass_utility;
		}

		static string8 g_get_description()
		{
			return "Customisable panel with JScript scripting support.";
		}

		static ui_element_config::ptr g_get_default_configuration()
		{
			ui_element_config_builder builder;
			return builder.finish(g_get_guid());
		}

		static void g_get_name(pfc::string_base& out)
		{
			out = jsp::component_name;
		}

		BOOL ProcessWindowMessage(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp, LRESULT& lres, DWORD) override
		{
			lres = 0;

			switch (msg)
			{
			case WM_SETCURSOR:
			case WM_ERASEBKGND:
				lres = 1;
				return TRUE;
			case WM_CONTEXTMENU:
				lres = 1;
				if (m_callback->is_edit_mode_enabled()) return FALSE;
				on_context_menu(lp);
				return TRUE;
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
				if (m_callback->is_edit_mode_enabled()) return FALSE;
				break;
			case WM_GETDLGCODE:
				lres = DLGC_WANTALLKEYS;
				return TRUE;
			}

			if (handle_message(hwnd, msg, wp, lp)) return TRUE;
			return FALSE;
		}

		GUID get_guid() override
		{
			return g_get_guid();
		}

		GUID get_subclass() override
		{
			return g_get_subclass();
		}

		HWND get_wnd() override
		{
			return m_hwnd;
		}

		IGdiFont* get_font_ui(uint32_t type) override
		{
			CFontHandle tmp = m_callback->query_font_ex(*guids::fonts[type]);
			LOGFONT lf;
			tmp.GetLogFont(&lf);
			CFont hFont = FontHelpers::create(lf);

			auto font = std::make_unique<Gdiplus::Font>(m_hdc, hFont);
			if (ensure_gdiplus_object(font))
			{
				return new ComObjectImpl<GdiFont>(std::move(font), hFont.Detach());
			}
			return nullptr;
		}

		bool edit_mode_context_menu_test(const POINT&, bool) override
		{
			return true;
		}

		int get_colour_ui(uint32_t type) override
		{
			return to_argb(m_callback->query_std_color(*guids::colours[type]));
		}

		ui_element_config::ptr get_configuration() override
		{
			ui_element_config_builder builder;
			m_config.get(&builder.m_stream, fb2k::noAbort);
			return builder.finish(g_get_guid());
		}

		void edit_mode_context_menu_build(const POINT&, bool, HMENU menu, uint32_t id_base) override
		{
			build_context_menu(menu, id_base);
		}

		void edit_mode_context_menu_command(const POINT&, bool, uint32_t id, uint32_t id_base) override
		{
			execute_context_menu_command(id, id_base);
		}

		void initialize_window(HWND parent)
		{
			Create(parent);
		}

		void notify(const GUID& what, size_t, const void*, size_t) override
		{
			if (what == ui_element_notify_colors_changed)
			{
				m_script_host->InvokeCallback(CallbackID::on_colours_changed);
			}
			else if (what == ui_element_notify_font_changed)
			{
				m_script_host->InvokeCallback(CallbackID::on_font_changed);
			}
		}

		void notify_size_limit_changed() override
		{
			m_callback->on_min_max_info_change();
		}

		void set_configuration(ui_element_config::ptr data) override
		{
			ui_element_config_parser parser(data);
			m_config.set(&parser.m_stream, parser.get_remaining(), fb2k::noAbort);
			update_script();
		}

	private:
		ui_element_instance_callback::ptr m_callback;
	};

	class UIElementImpl : public ui_element_impl<PanelWindowDUI> {};

	FB2K_SERVICE_FACTORY(UIElementImpl);
}
