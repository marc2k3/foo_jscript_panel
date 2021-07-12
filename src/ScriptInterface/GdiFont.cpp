#include "stdafx.h"
#include "GdiFont.h"

GdiFont::GdiFont(std::unique_ptr<Gdiplus::Font> font, HFONT hFont) : m_font(std::move(font)), m_hFont(hFont) {}

STDMETHODIMP GdiFont::get__HFONT(HFONT* out)
{
	if (!m_font || !out) return E_POINTER;

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

	*out = SysAllocString(FontHelpers::get_name(m_font).data());
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

	if (m_hFont)
	{
		m_hFont.DeleteObject();
	}
}
