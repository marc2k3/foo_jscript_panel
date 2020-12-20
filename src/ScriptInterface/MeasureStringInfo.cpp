#include "stdafx.h"
#include "MeasureStringInfo.h"

MeasureStringInfo::MeasureStringInfo(const Gdiplus::RectF& rect, int chars, int lines) : m_rect(rect), m_chars(chars), m_lines(lines) {}
MeasureStringInfo::~MeasureStringInfo() {}

STDMETHODIMP MeasureStringInfo::get_chars(int* out)
{
	if (!out) return E_POINTER;

	*out = m_chars;
	return S_OK;
}

STDMETHODIMP MeasureStringInfo::get_height(float* out)
{
	if (!out) return E_POINTER;

	*out = m_rect.Height;
	return S_OK;
}

STDMETHODIMP MeasureStringInfo::get_lines(int* out)
{
	if (!out) return E_POINTER;

	*out = m_lines;
	return S_OK;
}

STDMETHODIMP MeasureStringInfo::get_width(float* out)
{
	if (!out) return E_POINTER;

	*out = m_rect.Width;
	return S_OK;
}

STDMETHODIMP MeasureStringInfo::get_x(float* out)
{
	if (!out) return E_POINTER;

	*out = m_rect.X;
	return S_OK;
}

STDMETHODIMP MeasureStringInfo::get_y(float* out)
{
	if (!out) return E_POINTER;

	*out = m_rect.Y;
	return S_OK;
}
