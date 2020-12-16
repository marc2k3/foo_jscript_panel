#include "stdafx.h"
#include "GdiGraphics.h"
#include "EstimateLineWrap.h"

GdiGraphics::GdiGraphics() {}
GdiGraphics::~GdiGraphics() {}

STDMETHODIMP GdiGraphics::get__ptr(void** out)
{
	*out = m_graphics;
	return S_OK;
}

STDMETHODIMP GdiGraphics::put__ptr(void* g)
{
	m_graphics = reinterpret_cast<Gdiplus::Graphics*>(g);
	return S_OK;
}

STDMETHODIMP GdiGraphics::CalcTextHeight(BSTR str, IGdiFont* font, UINT* out)
{
	if (!m_graphics || !out) return E_POINTER;

	HFONT hFont = nullptr;
	font->get__HFONT(&hFont);
	HDC dc = m_graphics->GetHDC();

	{
		SelectObjectScope scope(dc, hFont);
		SIZE size;
		GetTextExtentPoint32(dc, str, SysStringLen(str), &size);
		*out = to_uint(size.cy);
	}

	m_graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::CalcTextWidth(BSTR str, IGdiFont* font, UINT* out)
{
	if (!m_graphics || !out) return E_POINTER;

	HFONT hFont = nullptr;
	font->get__HFONT(&hFont);
	HDC dc = m_graphics->GetHDC();

	{
		SelectObjectScope scope(dc, hFont);
		SIZE size;
		GetTextExtentPoint32(dc, str, SysStringLen(str), &size);
		*out = to_uint(size.cx);
	}

	m_graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawEllipse(float x, float y, float w, float h, float line_width, __int64 colour)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::Pen pen(to_uint(colour), line_width);
	m_graphics->DrawEllipse(&pen, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawImage(IGdiBitmap* image, float dstX, float dstY, float dstW, float dstH, float srcX, float srcY, float srcW, float srcH, float angle, UINT8 alpha)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::Bitmap* bitmap = nullptr;
	GET_PTR(image, bitmap);

	const Gdiplus::RectF rect(dstX, dstY, dstW, dstH);
	Gdiplus::ImageAttributes ia;
	if (alpha < UINT8_MAX)
	{
		Gdiplus::ColorMatrix cm = { 0.f };
		cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.f;
		cm.m[3][3] = static_cast<float>(alpha) / UINT8_MAX;
		ia.SetColorMatrix(&cm);
	}

	if (angle != 0.f)
	{
		Gdiplus::Matrix m, old_m;
		m.RotateAt(angle, Gdiplus::PointF(dstX + (dstW / 2), dstY + (dstH / 2)));
		m_graphics->GetTransform(&old_m);
		m_graphics->SetTransform(&m);
		m_graphics->DrawImage(bitmap, rect, srcX, srcY, srcW, srcH, Gdiplus::UnitPixel, &ia);
		m_graphics->SetTransform(&old_m);
	}
	else
	{
		m_graphics->DrawImage(bitmap, rect, srcX, srcY, srcW, srcH, Gdiplus::UnitPixel, &ia);
	}

	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawLine(float x1, float y1, float x2, float y2, float line_width, __int64 colour)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::Pen pen(to_uint(colour), line_width);
	m_graphics->DrawLine(&pen, x1, y1, x2, y2);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawPolygon(__int64 colour, float line_width, VARIANT points)
{
	if (!m_graphics) return E_POINTER;

	ComArrayReader reader;
	std::vector<Gdiplus::PointF> point_array;
	if (!reader.convert(points, point_array)) return E_INVALIDARG;

	Gdiplus::Pen pen(to_uint(colour), line_width);
	m_graphics->DrawPolygon(&pen, point_array.data(), point_array.size());
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawRect(float x, float y, float w, float h, float line_width, __int64 colour)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::Pen pen(to_uint(colour), line_width);
	m_graphics->DrawRectangle(&pen, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, float line_width, __int64 colour)
{
	if (!m_graphics) return E_POINTER;
	if (arc_width * 2 > w || arc_height * 2 > h) return E_INVALIDARG;

	Gdiplus::Pen pen(to_uint(colour), line_width);
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);

	Gdiplus::GraphicsPath gp;
	GetRoundRectPath(Gdiplus::RectF(x, y, w, h), arc_width, arc_height, gp);
	m_graphics->DrawPath(&pen, &gp);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawString(BSTR str, IGdiFont* font, __int64 colour, float x, float y, float w, float h, int flags)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::Font* fn = nullptr;
	GET_PTR(font, fn);

	Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
	SetFormat(flags, fmt);

	Gdiplus::SolidBrush brush(to_uint(colour));
	m_graphics->DrawString(str, -1, fn, Gdiplus::RectF(x, y, w, h), &fmt, &brush);
	return S_OK;
}

STDMETHODIMP GdiGraphics::EstimateLineWrap(BSTR str, IGdiFont* font, UINT width, VARIANT* out)
{
	if (!m_graphics || !out) return E_POINTER;

	EstimateLineWrap::WrappedItems result;

	HFONT hFont = nullptr;
	font->get__HFONT(&hFont);
	HDC dc = m_graphics->GetHDC();

	{
		SelectObjectScope scope(dc, hFont);
		::EstimateLineWrap(dc, width).wrap(str, result);
	}

	m_graphics->ReleaseHDC(dc);

	const size_t count = result.size();
	ComArrayWriter writer;
	if (!writer.create(count * 2)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		_variant_t var;
		var.vt = VT_UI4;
		var.ulVal = result[i].width;
		if (!writer.put_item(i * 2, result[i].text)) return E_OUTOFMEMORY;
		if (!writer.put_item((i * 2) + 1, var)) return E_OUTOFMEMORY;
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillEllipse(float x, float y, float w, float h, __int64 colour)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::SolidBrush brush(to_uint(colour));
	m_graphics->FillEllipse(&brush, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillGradRect(float x, float y, float w, float h, float angle, __int64 colour1, __int64 colour2, float focus)
{
	if (!m_graphics) return E_POINTER;

	const Gdiplus::RectF rect(x, y, w, h);
	Gdiplus::LinearGradientBrush brush(rect, to_uint(colour1), to_uint(colour2), angle, TRUE);
	brush.SetBlendTriangularShape(focus);
	m_graphics->FillRectangle(&brush, rect);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillPolygon(__int64 colour, UINT fillmode, VARIANT points)
{
	if (!m_graphics) return E_POINTER;

	ComArrayReader reader;
	std::vector<Gdiplus::PointF> point_array;
	if (!reader.convert(points, point_array)) return E_INVALIDARG;

	Gdiplus::SolidBrush brush(to_uint(colour));
	m_graphics->FillPolygon(&brush, point_array.data(), point_array.size(), static_cast<Gdiplus::FillMode>(fillmode));
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, __int64 colour)
{
	if (!m_graphics) return E_POINTER;
	if (arc_width * 2 > w || arc_height * 2 > h) return E_INVALIDARG;

	Gdiplus::SolidBrush brush(to_uint(colour));
	Gdiplus::GraphicsPath gp;
	GetRoundRectPath(Gdiplus::RectF(x, y, w, h), arc_width, arc_height, gp);
	m_graphics->FillPath(&brush, &gp);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillSolidRect(float x, float y, float w, float h, __int64 colour)
{
	if (!m_graphics) return E_POINTER;

	Gdiplus::SolidBrush brush(to_uint(colour));
	m_graphics->FillRectangle(&brush, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiAlphaBlend(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH, UINT8 alpha)
{
	if (!m_graphics) return E_POINTER;

	HBITMAP hbmp = nullptr;
	bitmap->get__HBITMAP(&hbmp);
	HDC dc = m_graphics->GetHDC();

	{
		CDC src_dc = CreateCompatibleDC(dc);
		SelectObjectScope scope(src_dc, hbmp);

		BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
		::GdiAlphaBlend(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, srcW, srcH, bf);
	}

	m_graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiDrawBitmap(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH)
{
	if (!m_graphics) return E_POINTER;

	HBITMAP hbmp = nullptr;
	bitmap->get__HBITMAP(&hbmp);
	HDC dc = m_graphics->GetHDC();

	{
		CDC src_dc = CreateCompatibleDC(dc);
		SelectObjectScope scope(src_dc, hbmp);

		if (dstW == srcW && dstH == srcH)
		{
			BitBlt(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, SRCCOPY);
		}
		else
		{
			SetStretchBltMode(dc, HALFTONE);
			SetBrushOrgEx(dc, 0, 0, nullptr);
			StretchBlt(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, srcW, srcH, SRCCOPY);
		}
	}

	m_graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiDrawText(BSTR str, IGdiFont* font, __int64 colour, int x, int y, int w, int h, int format)
{
	if (!m_graphics) return E_POINTER;
	if (format & DT_MODIFYSTRING) return E_INVALIDARG;

	HFONT hFont = nullptr;
	font->get__HFONT(&hFont);
	HDC dc = m_graphics->GetHDC();

	{
		CRect rect(x, y, x + w, y + h);
		SelectObjectScope scope(dc, hFont);

		SetTextColor(dc, to_colorref(colour));
		SetBkMode(dc, TRANSPARENT);
		SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);

		if (format & DT_CALCRECT)
		{
			CRect rect_calc;
			rect_calc.CopyRect(&rect);
			DrawText(dc, str, -1, &rect_calc, format);
			auto nh = rect_calc.Height();

			format &= ~DT_CALCRECT;

			if (format & DT_VCENTER)
			{
				rect.top += ((h - nh) >> 1);
				rect.bottom = rect.top + nh;
			}
			else if (format & DT_BOTTOM)
			{
				rect.top = rect.bottom - nh;
			}
		}

		DRAWTEXTPARAMS dpt = { sizeof(DRAWTEXTPARAMS), 4, 0, 0, 0 };
		DrawTextEx(dc, str, -1, &rect, format, &dpt);
	}

	m_graphics->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::MeasureString(BSTR str, IGdiFont* font, float x, float y, float w, float h, int flags, IMeasureStringInfo** out)
{
	if (!m_graphics || !out) return E_POINTER;

	Gdiplus::Font* fn = nullptr;
	GET_PTR(font, fn);

	Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
	SetFormat(flags, fmt);

	Gdiplus::RectF rect;
	int chars, lines;

	m_graphics->MeasureString(str, -1, fn, Gdiplus::RectF(x, y, w, h), &fmt, &rect, &chars, &lines);

	*out = new ComObjectImpl<MeasureStringInfo>(rect, chars, lines);
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetInterpolationMode(int mode)
{
	if (!m_graphics) return E_POINTER;

	m_graphics->SetInterpolationMode(static_cast<Gdiplus::InterpolationMode>(mode));
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetSmoothingMode(int mode)
{
	if (!m_graphics) return E_POINTER;

	m_graphics->SetSmoothingMode(static_cast<Gdiplus::SmoothingMode>(mode));
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetTextRenderingHint(UINT mode)
{
	if (!m_graphics) return E_POINTER;

	m_graphics->SetTextRenderingHint(static_cast<Gdiplus::TextRenderingHint>(mode));
	return S_OK;
}

void GdiGraphics::GetRoundRectPath(const Gdiplus::RectF& rect, float arc_width, float arc_height, Gdiplus::GraphicsPath& gp)
{
	arc_width *= 2;
	arc_height *= 2;
	Gdiplus::RectF corner(rect.X, rect.Y, arc_width, arc_height);

	gp.Reset();
	gp.AddArc(corner, 180, 90);

	corner.X += (rect.Width - arc_width);
	gp.AddArc(corner, 270, 90);

	corner.Y += (rect.Height - arc_height);
	gp.AddArc(corner, 0, 90);

	corner.X -= (rect.Width - arc_width);
	gp.AddArc(corner, 90, 90);

	gp.CloseFigure();
}

void GdiGraphics::SetFormat(int flags, Gdiplus::StringFormat& fmt)
{
	if (flags != 0)
	{
		fmt.SetAlignment(static_cast<Gdiplus::StringAlignment>((flags >> 28) & 0x3));
		fmt.SetLineAlignment(static_cast<Gdiplus::StringAlignment>((flags >> 24) & 0x3));
		fmt.SetTrimming(static_cast<Gdiplus::StringTrimming>((flags >> 20) & 0x7));
		fmt.SetFormatFlags(static_cast<Gdiplus::StringFormatFlags>(flags & 0x7FFF));
	}
}
