#include "stdafx.h"
#include "GdiRawBitmap.h"

GdiRawBitmap::GdiRawBitmap(Gdiplus::Bitmap* bitmap) : m_width(bitmap->GetWidth()), m_height(bitmap->GetHeight())
{
	bitmap->GetHBITMAP(Gdiplus::Color::Black, &m_hbmp);
}

STDMETHODIMP GdiRawBitmap::get__HBITMAP(HBITMAP* out)
{
	if (!m_hbmp || !out) return E_POINTER;

	*out = m_hbmp;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get_Height(UINT* out)
{
	if (!m_hbmp || !out) return E_POINTER;

	*out = m_height;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get_Width(UINT* out)
{
	if (!m_hbmp || !out) return E_POINTER;

	*out = m_width;
	return S_OK;
}

void GdiRawBitmap::FinalRelease()
{
	if (m_hbmp)
	{
		DeleteBitmap(m_hbmp);
		m_hbmp = nullptr;
	}
}
