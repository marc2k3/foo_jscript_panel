#pragma once

class GdiFont : public JSDisposableImpl<IGdiFont>
{
protected:
	GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont, bool managed = true);
	~GdiFont();

	void FinalRelease() override;

public:
	STDMETHODIMP get__HFONT(HFONT* p) override;
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP get_Height(UINT* p) override;
	STDMETHODIMP get_Name(BSTR* p) override;
	STDMETHODIMP get_Size(float* p) override;
	STDMETHODIMP get_Style(int* p) override;

private:
	HFONT m_hFont = nullptr;
	bool m_managed = true;
	std::unique_ptr<Gdiplus::Font> m_font;
};
