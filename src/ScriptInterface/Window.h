#pragma once

class PanelWindow;

class Window : public JSDispatchImpl<IWindow>
{
protected:
	Window(PanelWindow* panel);
	~Window();

public:
	STDMETHODIMP ClearInterval(UINT intervalID) override;
	STDMETHODIMP ClearTimeout(UINT timeoutID) override;
	STDMETHODIMP CreatePopupMenu(IMenuObj** pp) override;
	STDMETHODIMP CreateThemeManager(BSTR classid, IThemeManager** pp) override;
	STDMETHODIMP CreateTooltip(BSTR name, float pxSize, int style, ITooltip** pp) override;
	STDMETHODIMP GetColourCUI(UINT type, int* p) override;
	STDMETHODIMP GetColourDUI(UINT type, int* p) override;
	STDMETHODIMP GetFontCUI(UINT type, IGdiFont** pp) override;
	STDMETHODIMP GetFontDUI(UINT type, IGdiFont** pp) override;
	STDMETHODIMP GetProperty(BSTR name, VARIANT defaultval, VARIANT* p) override;
	STDMETHODIMP NotifyOthers(BSTR name, VARIANT info) override;
	STDMETHODIMP Reload(VARIANT_BOOL clear_properties) override;
	STDMETHODIMP Repaint() override;
	STDMETHODIMP RepaintRect(int x, int y, int w, int h) override;
	STDMETHODIMP SetCursor(UINT id) override;
	STDMETHODIMP SetInterval(IDispatch* func, int delay, UINT* p) override;
	STDMETHODIMP SetProperty(BSTR name, VARIANT val) override;
	STDMETHODIMP SetTimeout(IDispatch* func, int delay, UINT* p) override;
	STDMETHODIMP SetTooltipFont(BSTR name, float pxSize, int style) override;
	STDMETHODIMP ShowConfigure() override;
	STDMETHODIMP ShowProperties() override;
	STDMETHODIMP get_Height(int* p) override;
	STDMETHODIMP get_ID(UINT* p) override;
	STDMETHODIMP get_InstanceType(UINT* p) override;
	STDMETHODIMP get_IsTransparent(VARIANT_BOOL* p) override;
	STDMETHODIMP get_IsVisible(VARIANT_BOOL* p) override;
	STDMETHODIMP get_MaxHeight(UINT* p) override;
	STDMETHODIMP get_MaxWidth(UINT* p) override;
	STDMETHODIMP get_MinHeight(UINT* p) override;
	STDMETHODIMP get_MinWidth(UINT* p) override;
	STDMETHODIMP get_Name(BSTR* p) override;
	STDMETHODIMP get_Width(int* p) override;
	STDMETHODIMP put_MaxHeight(UINT height) override;
	STDMETHODIMP put_MaxWidth(UINT width) override;
	STDMETHODIMP put_MinHeight(UINT height) override;
	STDMETHODIMP put_MinWidth(UINT width) override;

private:
	PanelWindow* m_panel;
};
