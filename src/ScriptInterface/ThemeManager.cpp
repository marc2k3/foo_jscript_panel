#include "stdafx.h"
#include "ThemeManager.h"

ThemeManager::ThemeManager(HTHEME theme) : m_theme(theme) {}

STDMETHODIMP ThemeManager::DrawThemeBackground(IGdiGraphics* gr, int x, int y, int w, int h, int clip_x, int clip_y, int clip_w, int clip_h)
{
	if (!m_theme) return E_POINTER;

	Gdiplus::Graphics* graphics = nullptr;
	GET_PTR(gr, graphics);

	CRect rect(x, y, x + w, y + h);
	HDC dc = graphics->GetHDC();

	if (clip_x == 0 && clip_y == 0 && clip_w == 0 && clip_h == 0)
	{
		::DrawThemeBackground(m_theme, dc, m_partid, m_stateid, &rect, nullptr);
	}
	else
	{
		CRect rect_clip(clip_x, clip_y, clip_x + clip_w, clip_y + clip_h);
		::DrawThemeBackground(m_theme, dc, m_partid, m_stateid, &rect, rect_clip);
	}

	graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP ThemeManager::IsThemePartDefined(int partid, int stateid, VARIANT_BOOL* out)
{
	if (!m_theme || !out) return E_POINTER;

	*out = to_variant_bool(::IsThemePartDefined(m_theme, partid, stateid));
	return S_OK;
}

STDMETHODIMP ThemeManager::SetPartAndStateID(int partid, int stateid)
{
	if (!m_theme) return E_POINTER;

	m_partid = partid;
	m_stateid = stateid;
	return S_OK;
}

void ThemeManager::FinalRelease()
{
	CloseThemeData(m_theme);
	m_theme = nullptr;
}
