#pragma once

class Tooltip : public JSDispatchImpl<ITooltip>
{
protected:
	Tooltip(CWindow tooltip, CWindow parent);
	~Tooltip();

public:
	STDMETHODIMP Activate() override;
	STDMETHODIMP Deactivate() override;
	STDMETHODIMP GetDelayTime(int type, int* p) override;
	STDMETHODIMP SetDelayTime(int type, int time) override;
	STDMETHODIMP SetMaxWidth(int width) override;
	STDMETHODIMP TrackPosition(int x, int y) override;
	STDMETHODIMP get_Text(BSTR* p) override;
	STDMETHODIMP put_Text(BSTR text) override;
	STDMETHODIMP put_TrackActivate(VARIANT_BOOL activate) override;

private:
	CWindow m_parent, m_tooltip;
	TOOLINFO m_ti;
	std::wstring m_tip_buffer;
};
