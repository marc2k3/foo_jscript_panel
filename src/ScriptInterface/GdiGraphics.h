#pragma once

class GdiGraphics : public JSDispatchImpl<IGdiGraphics>
{
protected:
	GdiGraphics();
	~GdiGraphics();

public:
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP put__ptr(void* g) override;
	STDMETHODIMP CalcTextHeight(BSTR str, IGdiFont* font, UINT* out) override;
	STDMETHODIMP CalcTextWidth(BSTR str, IGdiFont* font, UINT* out) override;
	STDMETHODIMP DrawEllipse(float x, float y, float w, float h, float line_width, __int64 colour) override;
	STDMETHODIMP DrawImage(IGdiBitmap* image, float dstX, float dstY, float dstW, float dstH, float srcX, float srcY, float srcW, float srcH, float angle, UINT8 alpha) override;
	STDMETHODIMP DrawLine(float x1, float y1, float x2, float y2, float line_width, __int64 colour) override;
	STDMETHODIMP DrawPolygon(__int64 colour, float line_width, VARIANT points) override;
	STDMETHODIMP DrawRect(float x, float y, float w, float h, float line_width, __int64 colour) override;
	STDMETHODIMP DrawRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, float line_width, __int64 colour) override;
	STDMETHODIMP DrawString(BSTR str, IGdiFont* font, __int64 colour, float x, float y, float w, float h, int flags) override;
	STDMETHODIMP EstimateLineWrap(BSTR str, IGdiFont* font, UINT width, VARIANT* out) override;
	STDMETHODIMP FillEllipse(float x, float y, float w, float h, __int64 colour) override;
	STDMETHODIMP FillGradRect(float x, float y, float w, float h, float angle, __int64 colour1, __int64 colour2, float focus) override;
	STDMETHODIMP FillPolygon(__int64 colour, UINT fillmode, VARIANT points) override;
	STDMETHODIMP FillRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, __int64 colour) override;
	STDMETHODIMP FillSolidRect(float x, float y, float w, float h, __int64 colour) override;
	STDMETHODIMP GdiAlphaBlend(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH, UINT8 alpha) override;
	STDMETHODIMP GdiDrawBitmap(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH) override;
	STDMETHODIMP GdiDrawText(BSTR str, IGdiFont* font, __int64 colour, int x, int y, int w, int h, int format) override;
	STDMETHODIMP MeasureString(BSTR str, IGdiFont* font, float x, float y, float w, float h, int flags, IMeasureStringInfo** out) override;
	STDMETHODIMP SetInterpolationMode(int mode) override;
	STDMETHODIMP SetSmoothingMode(int mode) override;
	STDMETHODIMP SetTextRenderingHint(UINT mode) override;

private:
	void GetRoundRectPath(const Gdiplus::RectF& rect, float arc_width, float arc_height, Gdiplus::GraphicsPath& gp);
	void SetFormat(int flags, Gdiplus::StringFormat& out);

	Gdiplus::Graphics* m_graphics = nullptr;
};
