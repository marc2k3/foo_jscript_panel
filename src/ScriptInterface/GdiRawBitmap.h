#pragma once

class GdiRawBitmap : public JSDisposableImpl<IGdiRawBitmap>
{
protected:
	GdiRawBitmap(Gdiplus::Bitmap* bitmap);
	~GdiRawBitmap();

	void FinalRelease() override;

public:
	STDMETHODIMP get__HBITMAP(HBITMAP* p) override;
	STDMETHODIMP get_Height(UINT* p) override;
	STDMETHODIMP get_Width(UINT* p) override;

private:
	HBITMAP m_hbmp;
	size_t m_height, m_width;
};
