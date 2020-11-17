#pragma once

class MeasureStringInfo : public JSDispatchImpl<IMeasureStringInfo>
{
protected:
	MeasureStringInfo(const Gdiplus::RectF& rect, int chars, int lines);
	~MeasureStringInfo();

public:
	STDMETHODIMP get_chars(int* p) override;
	STDMETHODIMP get_height(float* p) override;
	STDMETHODIMP get_lines(int* p) override;
	STDMETHODIMP get_width(float* p) override;
	STDMETHODIMP get_x(float* p) override;
	STDMETHODIMP get_y(float* p) override;

private:
	Gdiplus::RectF m_rect;
	int m_lines, m_chars;
};
