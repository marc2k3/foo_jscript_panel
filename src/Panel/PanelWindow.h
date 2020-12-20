#pragma once
#include "PanelConfig.h"
#include "ScriptHost.h"

class DropTargetImpl;

class PanelWindow
{
public:
	virtual IGdiFont* get_font_ui(size_t type) = 0;
	virtual int get_colour_ui(size_t type) = 0;
	virtual void notify_size_limit_changed() = 0;

public:
	bool is_transparent();
	void destroy_tooltip();
	void repaint();
	void repaint_rect(int x, int y, int w, int h);
	void show_configure_popup(HWND parent);
	void show_property_popup(HWND parent);
	void unload_script();
	void update_script();

	CFontHandle m_tooltip_font;
	CPoint m_min, m_max;
	CRect m_rect{};
	CWindow m_hwnd, m_tooltip;
	PanelConfig m_config;
	bool m_dragdrop = false;
	bool m_grabfocus = false;
	bool m_is_default_ui = true;
	bool m_supports_transparency = false;
	pfc::com_ptr_t<ScriptHost> m_script_host;
	size_t m_id = 0;

protected:
	PanelWindow();
	virtual ~PanelWindow();

	bool handle_message(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void build_context_menu(HMENU menu, int id_base);
	void execute_context_menu_command(int id, int id_base);
	void on_context_menu(LPARAM lp);

	HDC m_hdc = nullptr;

private:
	void create_context();
	void delete_context();
	void load_script();
	void on_paint();
	void on_size();
	void refresh_background();
	void redraw(bool refreshbk);
	void reset_min_max();

	HBITMAP m_gr_bmp = nullptr;
	HBITMAP m_gr_bmp_bk = nullptr;
	bool m_is_mouse_tracked = false;
	bool m_refreshbk = true;
	pfc::com_ptr_t<DropTargetImpl> m_drop_target;
	pfc::com_ptr_t<GdiGraphics> m_gr_wrap;
	ui_selection_holder::ptr m_selection_holder;
};
