#pragma once

class GdiBitmap : public JSDisposableImpl<IGdiBitmap>
{
protected:
	GdiBitmap(std::unique_ptr<Gdiplus::Bitmap> bitmap);
	~GdiBitmap();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP ApplyAlpha(UINT8 alpha, IGdiBitmap** out) override;
	STDMETHODIMP ApplyMask(IGdiBitmap* image, VARIANT_BOOL* out) override;
	STDMETHODIMP Clone(float x, float y, float w, float h, IGdiBitmap** out) override;
	STDMETHODIMP CreateRawBitmap(IGdiRawBitmap** out) override;
	STDMETHODIMP GetColourSchemeJSON(UINT count, BSTR* out) override;
	STDMETHODIMP GetGraphics(IGdiGraphics** out) override;
	STDMETHODIMP InvertColours(IGdiBitmap** out) override;
	STDMETHODIMP ReleaseGraphics(IGdiGraphics* gr) override;
	STDMETHODIMP Resize(UINT w, UINT h, int interpolation_mode, IGdiBitmap** out) override;
	STDMETHODIMP RotateFlip(UINT mode) override;
	STDMETHODIMP SaveAs(BSTR path, BSTR format, VARIANT_BOOL* out) override;
	STDMETHODIMP StackBlur(UINT8 radius) override;
	STDMETHODIMP get_Height(UINT* out) override;
	STDMETHODIMP get_Width(UINT* out) override;

private:
	std::unique_ptr<Gdiplus::Bitmap> resize(uint32_t w, uint32_t h, Gdiplus::InterpolationMode interpolation_mode);

	std::unique_ptr<Gdiplus::Bitmap> m_bitmap;
};
