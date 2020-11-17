#include "stdafx.h"
#include "GdiFont.h"

GdiFont::GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont, bool managed) : m_font(std::move(font)), m_hFont(hFont), m_managed(managed) {}
GdiFont::~GdiFont() {}

STDMETHODIMP GdiFont::get__HFONT(HFONT* p)
{
	if (!p) return E_POINTER;

	*p = m_hFont;
	return S_OK;
}

STDMETHODIMP GdiFont::get__ptr(void** pp)
{
	*pp = m_font.get();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Height(UINT* p)
{
	if (!m_font || !p) return E_POINTER;

	auto dpi = static_cast<float>(QueryScreenDPI(nullptr));
	*p = to_uint(m_font->GetHeight(dpi));
	return S_OK;
}

STDMETHODIMP GdiFont::get_Name(BSTR* p)
{
	if (!m_font || !p) return E_POINTER;

	std::array<wchar_t, LF_FACESIZE> name;
	Gdiplus::FontFamily fontFamily;
	m_font->GetFamily(&fontFamily);
	fontFamily.GetFamilyName(name.data(), LANG_NEUTRAL);
	*p = SysAllocString(name.data());
	return S_OK;
}

STDMETHODIMP GdiFont::get_Size(float* p)
{
	if (!m_font || !p) return E_POINTER;

	*p = m_font->GetSize();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Style(int* p)
{
	if (!m_font || !p) return E_POINTER;

	*p = m_font->GetStyle();
	return S_OK;
}

void GdiFont::FinalRelease()
{
	if (m_font)
	{
		m_font.reset();
	}

	if (m_hFont && m_managed)
	{
		DeleteFont(m_hFont);
		m_hFont = nullptr;
	}
}
