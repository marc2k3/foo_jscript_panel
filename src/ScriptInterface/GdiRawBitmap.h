#pragma once

class GdiRawBitmap : public JSDisposableImpl<IGdiRawBitmap>
{
public:
	GdiRawBitmap(Gdiplus::Bitmap* bitmap);

	STDMETHODIMP get__HBITMAP(HBITMAP* out) override;
	STDMETHODIMP get_Height(UINT* out) override;
	STDMETHODIMP get_Width(UINT* out) override;

protected:
	void FinalRelease() override;

private:
	HBITMAP m_hbmp;
	uint32_t m_height = 0, m_width = 0;
};
