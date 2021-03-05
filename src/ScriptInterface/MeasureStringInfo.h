#pragma once

class MeasureStringInfo : public JSDispatchImpl<IMeasureStringInfo>
{
public:
	MeasureStringInfo(const Gdiplus::RectF& rect, int chars, int lines);

	STDMETHODIMP get_chars(int* out) override;
	STDMETHODIMP get_height(float* out) override;
	STDMETHODIMP get_lines(int* out) override;
	STDMETHODIMP get_width(float* out) override;
	STDMETHODIMP get_x(float* out) override;
	STDMETHODIMP get_y(float* out) override;

private:
	Gdiplus::RectF m_rect;
	int m_lines, m_chars;
};
