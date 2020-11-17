#pragma once

class GdiBitmap : public JSDisposableImpl<IGdiBitmap>
{
protected:
	GdiBitmap(std::unique_ptr<Gdiplus::Bitmap> bitmap);
	~GdiBitmap();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP ApplyAlpha(UINT8 alpha, IGdiBitmap** pp) override;
	STDMETHODIMP ApplyMask(IGdiBitmap* image, VARIANT_BOOL* p) override;
	STDMETHODIMP Clone(float x, float y, float w, float h, IGdiBitmap** pp) override;
	STDMETHODIMP CreateRawBitmap(IGdiRawBitmap** pp) override;
	STDMETHODIMP GetColourSchemeJSON(UINT count, BSTR* p) override;
	STDMETHODIMP GetGraphics(IGdiGraphics** pp) override;
	STDMETHODIMP InvertColours(IGdiBitmap** pp) override;
	STDMETHODIMP ReleaseGraphics(IGdiGraphics* p) override;
	STDMETHODIMP Resize(UINT w, UINT h, int interpolation_mode, IGdiBitmap** pp) override;
	STDMETHODIMP RotateFlip(UINT mode) override;
	STDMETHODIMP SaveAs(BSTR path, BSTR format, VARIANT_BOOL* p) override;
	STDMETHODIMP StackBlur(UINT8 radius) override;
	STDMETHODIMP get_Height(UINT* p) override;
	STDMETHODIMP get_Width(UINT* p) override;

private:
	std::unique_ptr<Gdiplus::Bitmap> resize(uint32_t w, uint32_t h, Gdiplus::InterpolationMode interpolation_mode);

	std::unique_ptr<Gdiplus::Bitmap> m_bitmap;
};
