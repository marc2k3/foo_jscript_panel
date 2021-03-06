#pragma once

class GdiFont : public JSDisposableImpl<IGdiFont>
{
public:
	GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont);

	STDMETHODIMP get__HFONT(HFONT* out) override;
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP get_Height(UINT* out) override;
	STDMETHODIMP get_Name(BSTR* out) override;
	STDMETHODIMP get_Size(float* out) override;
	STDMETHODIMP get_Style(int* out) override;

protected:
	void FinalRelease() override;

private:
	CFont m_hFont;
	std::unique_ptr<Gdiplus::Font> m_font;
};
