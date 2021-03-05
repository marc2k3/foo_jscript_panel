#pragma once

class PanelWindow;

class Window : public JSDispatchImpl<IWindow>
{
public:
	Window(PanelWindow* panel);

	STDMETHODIMP ClearInterval(UINT id) override;
	STDMETHODIMP ClearTimeout(UINT id) override;
	STDMETHODIMP CreatePopupMenu(IMenuObj** out) override;
	STDMETHODIMP CreateThemeManager(BSTR classid, IThemeManager** out) override;
	STDMETHODIMP CreateTooltip(BSTR name, float pxSize, int style, ITooltip** out) override;
	STDMETHODIMP GetColourCUI(UINT type, int* out) override;
	STDMETHODIMP GetColourDUI(UINT type, int* out) override;
	STDMETHODIMP GetFontCUI(UINT type, IGdiFont** out) override;
	STDMETHODIMP GetFontDUI(UINT type, IGdiFont** out) override;
	STDMETHODIMP GetProperty(BSTR name, VARIANT defaultval, VARIANT* out) override;
	STDMETHODIMP NotifyOthers(BSTR name, VARIANT info) override;
	STDMETHODIMP Reload(VARIANT_BOOL clear_properties) override;
	STDMETHODIMP Repaint() override;
	STDMETHODIMP RepaintRect(int x, int y, int w, int h) override;
	STDMETHODIMP SetCursor(UINT id) override;
	STDMETHODIMP SetInterval(IDispatch* func, int delay, UINT* out) override;
	STDMETHODIMP SetProperty(BSTR name, VARIANT val) override;
	STDMETHODIMP SetTimeout(IDispatch* func, int delay, UINT* out) override;
	STDMETHODIMP SetTooltipFont(BSTR name, float pxSize, int style) override;
	STDMETHODIMP ShowConfigure() override;
	STDMETHODIMP ShowProperties() override;
	STDMETHODIMP get_Height(int* out) override;
	STDMETHODIMP get_ID(UINT* out) override;
	STDMETHODIMP get_InstanceType(UINT* out) override;
	STDMETHODIMP get_IsTransparent(VARIANT_BOOL* out) override;
	STDMETHODIMP get_IsVisible(VARIANT_BOOL* out) override;
	STDMETHODIMP get_MaxHeight(UINT* out) override;
	STDMETHODIMP get_MaxWidth(UINT* out) override;
	STDMETHODIMP get_MinHeight(UINT* out) override;
	STDMETHODIMP get_MinWidth(UINT* out) override;
	STDMETHODIMP get_Name(BSTR* out) override;
	STDMETHODIMP get_Width(int* out) override;
	STDMETHODIMP put_MaxHeight(UINT height) override;
	STDMETHODIMP put_MaxWidth(UINT width) override;
	STDMETHODIMP put_MinHeight(UINT height) override;
	STDMETHODIMP put_MinWidth(UINT width) override;

private:
	PanelWindow* m_panel;
};
