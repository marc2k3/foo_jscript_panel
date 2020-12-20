#pragma once

class GdiFont : public JSDisposableImpl<IGdiFont>
{
protected:
	GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont, bool managed = true);
	~GdiFont();

	void FinalRelease() override;

public:
	STDMETHODIMP get__HFONT(HFONT* out) override;
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP get_Height(UINT* out) override;
	STDMETHODIMP get_Name(BSTR* out) override;
	STDMETHODIMP get_Size(float* out) override;
	STDMETHODIMP get_Style(int* out) override;

private:
	HFONT m_hFont = nullptr;
	bool m_managed = true;
	std::unique_ptr<Gdiplus::Font> m_font;
};
