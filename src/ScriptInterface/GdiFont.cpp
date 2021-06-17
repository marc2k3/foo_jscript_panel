#include "stdafx.h"
#include "GdiFont.h"

GdiFont::GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont, bool managed) : m_font(std::move(font)), m_hFont(hFont), m_managed(managed) {}

STDMETHODIMP GdiFont::get__HFONT(HFONT* out)
{
	if (!out) return E_POINTER;

	*out = m_hFont;
	return S_OK;
}

STDMETHODIMP GdiFont::get__ptr(void** out)
{
	*out = m_font.get();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Height(UINT* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = to_uint(m_font->GetHeight(static_cast<Gdiplus::REAL>(QueryScreenDPI())));
	return S_OK;
}

STDMETHODIMP GdiFont::get_Name(BSTR* out)
{
	if (!m_font || !out) return E_POINTER;

	FontNameArray name;
	Gdiplus::FontFamily fontFamily;
	m_font->GetFamily(&fontFamily);
	fontFamily.GetFamilyName(name.data(), LANG_NEUTRAL);
	*out = SysAllocString(name.data());
	return S_OK;
}

STDMETHODIMP GdiFont::get_Size(float* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = m_font->GetSize();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Style(int* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = m_font->GetStyle();
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
