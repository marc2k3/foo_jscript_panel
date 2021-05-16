#include "stdafx.h"
#include "DialogConfigure.h"
#include "DialogProperties.h"
#include "DropTargetImpl.h"
#include "PanelManager.h"
#include "PanelTimerDispatcher.h"
#include "PanelWindow.h"

static const std::map<size_t, CallbackID> wm_msg_map =
{
	{ WM_LBUTTONDOWN, CallbackID::on_mouse_lbtn_down },
	{ WM_MBUTTONDOWN, CallbackID::on_mouse_mbtn_down },
	{ WM_RBUTTONDOWN, CallbackID::on_mouse_rbtn_down },
	{ WM_LBUTTONUP, CallbackID::on_mouse_lbtn_up },
	{ WM_MBUTTONUP, CallbackID::on_mouse_mbtn_up },
	{ WM_RBUTTONUP, CallbackID::on_mouse_rbtn_up },
	{ WM_LBUTTONDBLCLK, CallbackID::on_mouse_lbtn_dblclk },
	{ WM_MBUTTONDBLCLK, CallbackID::on_mouse_mbtn_dblclk },
	{ WM_RBUTTONDBLCLK, CallbackID::on_mouse_rbtn_dblclk },
	{ WM_MOUSEWHEEL, CallbackID::on_mouse_wheel },
	{ WM_MOUSEHWHEEL, CallbackID::on_mouse_wheel_h },
	{ WM_SYSKEYDOWN, CallbackID::on_key_down },
	{ WM_KEYDOWN, CallbackID::on_key_down },
	{ WM_KEYUP, CallbackID::on_key_up },
	{ WM_CHAR, CallbackID::on_char },
};

PanelWindow::PanelWindow() : m_script_host(new ScriptHost(this)) {}
PanelWindow::~PanelWindow() {}

bool PanelWindow::handle_message(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
		m_hwnd = hwnd;
		m_id = reinterpret_cast<UINT_PTR>(hwnd);
		m_hdc = m_hwnd.GetDC();
		create_context();
		m_gr_wrap = new ImplementCOMRefCounter<GdiGraphics>();
		PanelManager::instance().add_window(m_hwnd);
		load_script();
		return true;
	case WM_DESTROY:
		unload_script();
		PanelManager::instance().remove_window(m_hwnd);
		if (m_gr_wrap.is_valid()) m_gr_wrap.release();
		delete_context();
		m_hwnd.ReleaseDC(m_hdc);
		return true;
	case WM_DISPLAYCHANGE:
	case WM_THEMECHANGED:
		update_script();
		return true;
	case WM_PAINT:
		if (is_transparent() && m_refreshbk) refresh_background();
		else on_paint();
		return true;
	case WM_SIZE:
		m_hwnd.GetClientRect(&m_rect);
		on_size();
		return true;
	case WM_GETMINMAXINFO:
		{
			auto info = reinterpret_cast<LPMINMAXINFO>(lp);
			info->ptMaxTrackSize = m_max;
			info->ptMinTrackSize = m_min;
			return true;
		}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			if (m_grabfocus) m_hwnd.SetFocus();
			m_hwnd.SetCapture();
			VariantArgs args = { wp, GET_Y_LPARAM(lp), GET_X_LPARAM(lp) };
			m_script_host->InvokeCallback(wm_msg_map.at(msg), args);
			return false;
		}
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		{
			pfc::onLeaving scope([] { ReleaseCapture(); });
			VariantArgs args = { wp, GET_Y_LPARAM(lp), GET_X_LPARAM(lp) };

			if (msg == WM_RBUTTONUP)
			{
				if (IsKeyPressed(VK_LSHIFT) && IsKeyPressed(VK_LWIN)) return false;
				return m_script_host->InvokeMouseRBtnUp(args);
			}
			
			m_script_host->InvokeCallback(wm_msg_map.at(msg), args);
			return false;
		}
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		{
			VariantArgs args = { wp, GET_Y_LPARAM(lp), GET_X_LPARAM(lp) };
			m_script_host->InvokeCallback(wm_msg_map.at(msg), args);
			return false;
		}
	case WM_MOUSEMOVE:
		{
			if (!m_is_mouse_tracked)
			{
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hwnd, HOVER_DEFAULT };
				TrackMouseEvent(&tme);
				m_is_mouse_tracked = true;
				SetCursor(LoadCursor(nullptr, IDC_ARROW));
			}

			VariantArgs args = { wp, GET_Y_LPARAM(lp), GET_X_LPARAM(lp) };
			m_script_host->InvokeCallback(CallbackID::on_mouse_move, args);
			return false;
		}
	case WM_MOUSELEAVE:
		m_is_mouse_tracked = false;
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
		m_script_host->InvokeCallback(CallbackID::on_mouse_leave);
		return false;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		{
			VariantArgs args = { GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 1 : -1 };
			m_script_host->InvokeCallback(wm_msg_map.at(msg), args);
			return false;
		}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		{
			VariantArgs args = { wp };
			m_script_host->InvokeCallback(wm_msg_map.at(msg), args);
			return true;
		}
	case WM_SETFOCUS:
		{
			m_selection_holder = ui_selection_manager::get()->acquire();

			VariantArgs args = { true };
			m_script_host->InvokeCallback(CallbackID::on_focus, args);
			return false;
		}
	case WM_KILLFOCUS:
		{
			m_selection_holder.release();

			VariantArgs args = { false };
			m_script_host->InvokeCallback(CallbackID::on_focus, args);
			return false;
		}
	case jsp::uwm_refreshbk:
		redraw(true);
		return true;
	case jsp::uwm_timer:
		PanelTimerDispatcher::instance().invoke_message(wp);
		return true;
	case jsp::uwm_unload:
		unload_script();
		return true;
	}

	if (msg < WM_USER) return false;

	CallbackID id = static_cast<CallbackID>(msg);

	switch (id)
	{
	case CallbackID::on_dsp_preset_changed:
	case CallbackID::on_output_device_changed:
	case CallbackID::on_playback_dynamic_info:
	case CallbackID::on_playlist_items_selection_change:
	case CallbackID::on_playlist_switch:
	case CallbackID::on_playlists_changed:
	case CallbackID::on_selection_changed:
		m_script_host->InvokeCallback(id);
		return true;
	case CallbackID::on_main_menu:
	case CallbackID::on_playback_dynamic_info_track:
	case CallbackID::on_playback_order_changed:
	case CallbackID::on_playback_queue_changed:
	case CallbackID::on_playback_stop:
	case CallbackID::on_playlist_items_added:
	case CallbackID::on_playlist_items_reordered:
	case CallbackID::on_replaygain_mode_changed:
		{
			VariantArgs args = { wp };
			m_script_host->InvokeCallback(id, args);
			return true;
		}
	case CallbackID::on_always_on_top_changed:
	case CallbackID::on_cursor_follow_playback_changed:
	case CallbackID::on_playback_follow_cursor_changed:
	case CallbackID::on_playback_pause:
	case CallbackID::on_playlist_stop_after_current_changed:
		{
			VariantArgs args = { wp == 1 };
			m_script_host->InvokeCallback(id, args);
			return true;
		}
	case CallbackID::on_item_focus_change:
	case CallbackID::on_notify_data:
	case CallbackID::on_playback_seek:
	case CallbackID::on_playback_starting:
	case CallbackID::on_playback_time:
	case CallbackID::on_playlist_item_ensure_visible:
	case CallbackID::on_playlist_items_removed:
	case CallbackID::on_volume_change:
		{
			auto data = CallbackDataReleaser<CallbackData>(wp);

			m_script_host->InvokeCallback(id, data->m_args);
			return true;
		}
	case CallbackID::on_item_played:
	case CallbackID::on_playback_edited:
	case CallbackID::on_playback_new_track:
		{
			auto data = CallbackDataReleaser<MetadbCallbackData>(wp);
			auto handle = new ComObjectImpl<MetadbHandle>(data->m_handles[0]);

			VariantArgs args = { handle };
			m_script_host->InvokeCallback(id, args);

			if (handle) handle->Release();
			return true;
		}
	case CallbackID::on_library_items_added:
	case CallbackID::on_library_items_changed:
	case CallbackID::on_library_items_removed:
	case CallbackID::on_locations_added:
	case CallbackID::on_metadb_changed:
		{
			auto data = CallbackDataReleaser<MetadbCallbackData>(wp);
			auto handles = new ComObjectImpl<MetadbHandleList>(data->m_handles);

			VariantArgs args = { handles };
			if (id == CallbackID::on_locations_added) args.emplace_back(lp); // cookie
			m_script_host->InvokeCallback(id, args);

			if (handles) handles->Release();
			return true;
		}
	case CallbackID::on_get_album_art_done:
		{
			auto data = reinterpret_cast<AsyncArtData*>(wp);

			VariantArgs args = { data->m_path, data->m_bitmap,  data->m_art_id, data->m_handle };
			m_script_host->InvokeCallback(id, args);
			return true;
		}
	case CallbackID::on_load_image_done:
		{
			auto data = reinterpret_cast<AsyncImageData*>(wp);

			VariantArgs args = { data->m_path, data->m_bitmap, data->m_cookie };
			m_script_host->InvokeCallback(id, args);
			return true;
		}
	}
	return false;
}

bool PanelWindow::is_transparent()
{
	return m_supports_transparency && m_config.m_transparent;
}

void PanelWindow::build_context_menu(HMENU menu, int id_base)
{
	AppendMenu(menu, MF_STRING, id_base + 1, L"&Reload");
	AppendMenu(menu, MF_SEPARATOR, 0, 0);
	AppendMenu(menu, MF_STRING, id_base + 2, L"&Properties");
	AppendMenu(menu, MF_STRING, id_base + 3, L"&Configure...");
}

void PanelWindow::create_context()
{
	delete_context();

	m_gr_bmp = CreateCompatibleBitmap(m_hdc, m_rect.Width(), m_rect.Height());

	if (is_transparent())
	{
		m_gr_bmp_bk = CreateCompatibleBitmap(m_hdc, m_rect.Width(), m_rect.Height());
	}
}

void PanelWindow::delete_context()
{
	if (m_gr_bmp)
	{
		DeleteBitmap(m_gr_bmp);
		m_gr_bmp = nullptr;
	}

	if (m_gr_bmp_bk)
	{
		DeleteBitmap(m_gr_bmp_bk);
		m_gr_bmp_bk = nullptr;
	}
}

void PanelWindow::destroy_tooltip()
{
	if (m_tooltip.IsWindow()) m_tooltip.DestroyWindow();
	if (!m_tooltip_font.IsNull()) m_tooltip_font.DeleteObject();
}

void PanelWindow::execute_context_menu_command(int id, int id_base)
{
	switch (id - id_base)
	{
	case 1:
		update_script();
		break;
	case 2:
		show_property_popup(m_hwnd);
		break;
	case 3:
		show_configure_popup(m_hwnd);
		break;
	}
}

void PanelWindow::load_script()
{
	Timer timer;

	if (!m_is_default_ui)
	{
		DWORD extstyle = m_hwnd.GetWindowLongPtr(GWL_EXSTYLE);
		extstyle &= ~WS_EX_CLIENTEDGE & ~WS_EX_STATICEDGE;
		extstyle |= m_config.get_edge_style();
		m_hwnd.SetWindowLongPtr(GWL_EXSTYLE, extstyle);
	}

	m_hwnd.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	reset_min_max();
	notify_size_limit_changed();
	m_dragdrop = false;
	m_grabfocus = false;

	m_script_host->m_info.update(m_id, m_config.m_code);
	if (FAILED(m_script_host->Initialise()))
	{
		return;
	}

	if (m_dragdrop)
	{
		m_drop_target = new DropTargetImpl(this);
		RegisterDragDrop(m_hwnd, m_drop_target.get_ptr());
	}

	on_size();

	FB2K_console_formatter() << m_script_host->m_info.m_build_string << ": initialised in " << timer.query() << " ms";
}

void PanelWindow::on_context_menu(LPARAM lp)
{
	CPoint pt(lp);
	if (pt.x == -1 && pt.y == -1)
	{
		DWORD dwPos = GetMessagePos();
		pt.SetPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));

		CRect rect;
		m_hwnd.GetWindowRect(&rect);

		if (!rect.PtInRect(pt))
		{
			pt.SetPoint(0, 0);
			m_hwnd.MapWindowPoints(nullptr, &pt, 1);
			pt.Offset(m_rect.Width() / 2, m_rect.Height() / 2);
		}
	}

	CMenu menu = CreatePopupMenu();
	constexpr int base_id = 0;
	build_context_menu(menu, base_id);
	const int idx = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, m_hwnd, nullptr);
	execute_context_menu_command(idx, base_id);
}

void PanelWindow::on_paint()
{
	if (!m_gr_bmp || m_gr_wrap.is_empty()) return;

	CPaintDC paintdc(m_hwnd);
	CDC memdc = CreateCompatibleDC(paintdc);
	SelectObjectScope scope_bmp(memdc, m_gr_bmp);

	if (m_script_host->HasError())
	{
		CFont font = create_font(L"Segoe UI", 24, Gdiplus::FontStyleBold);
		LOGBRUSH lbBack = { BS_SOLID, RGB(225, 60, 45), 0 };
		CBrush hBack = CreateBrushIndirect(&lbBack);
		SelectObjectScope scope(memdc, font);

		memdc.FillRect(&m_rect, hBack);
		memdc.SetBkMode(TRANSPARENT);
		memdc.SetTextColor(RGB(255, 255, 255));
		memdc.DrawText(L"JavaScript error", -1, &m_rect, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
	}
	else
	{
		if (is_transparent())
		{
			CDC bkdc = CreateCompatibleDC(paintdc);
			SelectObjectScope scope_bk(bkdc, m_gr_bmp_bk);
			memdc.BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), bkdc, m_rect.left, m_rect.top, SRCCOPY);
		}
		else
		{
			memdc.FillRect(&m_rect, reinterpret_cast<HBRUSH>(COLOR_WINDOWFRAME));
		}

		if (m_script_host->Ready())
		{
			Gdiplus::Graphics gr(memdc);
			const Gdiplus::Rect rect(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());
			gr.SetClip(rect);
			m_gr_wrap->put__ptr(&gr);

			VariantArgs args = { m_gr_wrap.get_ptr() };
			m_script_host->InvokeCallback(CallbackID::on_paint, args);
			m_gr_wrap->put__ptr(nullptr);
		}
	}
	paintdc.BitBlt(0, 0, m_rect.Width(), m_rect.Height(), memdc, 0, 0, SRCCOPY);
	m_refreshbk = true;
}

void PanelWindow::on_size()
{
	if (m_rect.IsRectEmpty()) return;
	create_context();
	m_script_host->InvokeCallback(CallbackID::on_size);
	if (is_transparent()) m_hwnd.PostMessage(jsp::uwm_refreshbk);
	else repaint();
}

void PanelWindow::redraw(bool refreshbk)
{
	m_refreshbk = refreshbk;
	m_hwnd.RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

void PanelWindow::refresh_background()
{
	CWindow wnd_parent = GetAncestor(m_hwnd, GA_PARENT);

	if (!wnd_parent || IsIconic(core_api::get_main_window()) || !m_hwnd.IsWindowVisible())
		return;

	CPoint pt(0, 0);
	m_hwnd.ClientToScreen(&pt);
	wnd_parent.ScreenToClient(&pt);

	m_hwnd.SetWindowRgn(CreateRectRgn(0, 0, 0, 0));
	wnd_parent.RedrawWindow(CRect(pt, m_rect.Size()), nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW);

	{
		CClientDC dc_parent(wnd_parent);
		CDC hdc_bk = CreateCompatibleDC(dc_parent);
		SelectObjectScope scope(hdc_bk, m_gr_bmp_bk);
		hdc_bk.BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), dc_parent, pt.x, pt.y, SRCCOPY);
	}

	m_hwnd.SetWindowRgn(nullptr);
	if (m_config.m_style != 0) m_hwnd.SendMessage(WM_NCPAINT, 1);
	redraw(false);
}

void PanelWindow::repaint()
{
	m_refreshbk = false;
	m_hwnd.InvalidateRect(nullptr, FALSE);
}

void PanelWindow::repaint_rect(int x, int y, int w, int h)
{
	m_refreshbk = false;
	m_hwnd.InvalidateRect(CRect(x, y, x + w, y + h), FALSE);
}

void PanelWindow::reset_min_max()
{
	m_min.SetPoint(0, 0);
	m_max.SetPoint(INT_MAX, INT_MAX);
}

void PanelWindow::show_configure_popup(HWND parent)
{
	modal_dialog_scope scope;
	if (scope.can_create())
	{
		scope.initialize(parent);
		CDialogConfigure dlg(this);
		dlg.DoModal(parent);
	}
}

void PanelWindow::show_property_popup(HWND parent)
{
	modal_dialog_scope scope;
	if (scope.can_create())
	{
		scope.initialize(parent);
		CDialogProperties dlg(this);
		dlg.DoModal(parent);
	}
}

void PanelWindow::unload_script()
{
	m_script_host->InvokeCallback(CallbackID::on_script_unload);
	destroy_tooltip();

	PanelTimerDispatcher::instance().request_stop_multi(m_hwnd);
	m_script_host->Stop();
	m_selection_holder.release();

	if (m_dragdrop)
	{
		m_drop_target.release();
		RevokeDragDrop(m_hwnd);
	}
}

void PanelWindow::update_script()
{
	if (m_hwnd == nullptr) return;

	unload_script();
	load_script();
}
