#include "stdafx.h"
#include "PanelWindow.h"

namespace
{
	class PanelWindowCUI : public PanelWindow, public uie::container_ui_extension, public cui::fonts::common_callback, public cui::colours::common_callback
	{
	public:
		PanelWindowCUI() : m_colours_helper(pfc::guid_null)
		{
			m_is_default_ui = false;
		}

		HWND create_or_transfer_window(HWND parent, const uie::window_host_ptr& host, const ui_helpers::window_position_t& position) override
		{
			if (m_hwnd)
			{
				m_hwnd.ShowWindow(SW_HIDE);
				m_hwnd.SetParent(parent);
				m_host->relinquish_ownership(m_hwnd);
				m_host = host;

				m_hwnd.SetWindowPos(nullptr, position.x, position.y, position.cx, position.cy, SWP_NOZORDER);
			}
			else
			{
				m_host = host;
				create(parent, this, position);
			}

			string8 name = pfc::getWindowClassName(parent);
			m_supports_transparency = name.equals("PSSWindowContainer") || name.equals("ReBarWindow32");

			return m_hwnd;
		}

		HWND get_wnd() const override
		{
			return m_hwnd;
		}

		IGdiFont* get_font_ui(uint32_t type) override
		{
			LOGFONT lf;
			static_api_ptr_t<cui::fonts::manager>()->get_font(static_cast<cui::fonts::font_type_t>(type), lf);
			CFont hFont = FontHelpers::create(lf);

			auto font = std::make_unique<Gdiplus::Font>(m_hdc, hFont);
			if (ensure_gdiplus_object(font))
			{
				return new ComObjectImpl<GdiFont>(std::move(font), hFont.Detach());
			}
			return nullptr;
		}

		LRESULT on_message(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp) override
		{
			switch (msg)
			{
			case WM_CREATE:
				static_api_ptr_t<cui::colours::manager>()->register_common_callback(this);
				static_api_ptr_t<cui::fonts::manager>()->register_common_callback(this);
				break;
			case WM_DESTROY:
				static_api_ptr_t<cui::colours::manager>()->deregister_common_callback(this);
				static_api_ptr_t<cui::fonts::manager>()->deregister_common_callback(this);
				break;
			case WM_SETCURSOR:
				return 1;
			case WM_ERASEBKGND:
				if (is_transparent()) m_hwnd.PostMessage(jsp::uwm_refreshbk);
				return 1;
			case WM_CONTEXTMENU:
				on_context_menu(lp);
				return 1;
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				if (uie::window::g_process_keydown_keyboard_shortcuts(wp))
					return 0;
				break;
			}

			if (handle_message(hwnd, msg, wp, lp)) return 0;
			return DefWindowProc(hwnd, msg, wp, lp);
		}

		bool have_config_popup() const override
		{
			return true;
		}

		bool is_available(const uie::window_host_ptr&) const override
		{
			return true;
		}

		bool show_config_popup(HWND parent) override
		{
			show_configure_popup(parent);
			return true;
		}

		class_data& get_class_data() const override
		{
			__implement_get_class_data_ex(L"jsp_class_cui", L"", false, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, CS_DBLCLKS);
		}

		const GUID& get_extension_guid() const override
		{
			return guids::window_cui;
		}

		int get_colour_ui(uint32_t type) override
		{
			return to_argb(m_colours_helper.get_colour(static_cast<cui::colours::colour_identifier_t>(type)));
		}

		uint32_t get_type() const override
		{
			return uie::type_toolbar | uie::type_panel;
		}

		void destroy_window() override
		{
			destroy();
			m_host.release();
		}

		void get_category(pfc::string_base& out) const override
		{
			out = "Panels";
		}

		void get_config(stream_writer* writer, abort_callback& abort) const override
		{
			m_config.get(writer, abort);
		}

		void get_name(pfc::string_base& out) const override
		{
			out = jsp::component_name;
		}

		void on_bool_changed(size_t) const override {}

		void on_colour_changed(size_t) const override
		{
			m_script_host->InvokeCallback(CallbackID::on_colours_changed);
		}

		void on_font_changed(size_t) const override
		{
			m_script_host->InvokeCallback(CallbackID::on_font_changed);
		}

		void set_config(stream_reader* reader, size_t size, abort_callback& abort) override
		{
			m_config.set(reader, size, abort);
		}

		void notify_size_limit_changed() override
		{
			if (m_host.is_valid())
			{
				m_host->on_size_limit_change(m_hwnd, uie::size_limit_all);
			}
		}

	private:
		cui::colours::helper m_colours_helper;
		uie::window_host_ptr m_host;
	};

	static uie::window_factory<PanelWindowCUI> g_panel_window_cui;
}
