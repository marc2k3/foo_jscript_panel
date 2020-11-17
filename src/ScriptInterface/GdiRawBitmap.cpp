#include "stdafx.h"
#include "GdiRawBitmap.h"

GdiRawBitmap::GdiRawBitmap(Gdiplus::Bitmap* bitmap) : m_width(bitmap->GetWidth()), m_height(bitmap->GetHeight())
{
	bitmap->GetHBITMAP(Gdiplus::Color::Black, &m_hbmp);
}

GdiRawBitmap::~GdiRawBitmap() {}

STDMETHODIMP GdiRawBitmap::get__HBITMAP(HBITMAP* p)
{
	if (!m_hbmp || !p) return E_POINTER;

	*p = m_hbmp;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get_Height(UINT* p)
{
	if (!m_hbmp || !p) return E_POINTER;

	*p = m_height;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get_Width(UINT* p)
{
	if (!m_hbmp || !p) return E_POINTER;

	*p = m_width;
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
