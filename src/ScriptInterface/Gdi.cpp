#include "stdafx.h"
#include "Gdi.h"
#include "AsyncImageTask.h"

Gdi::Gdi() {}
Gdi::~Gdi() {}

STDMETHODIMP Gdi::CreateImage(int w, int h, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	auto bitmap = std::make_unique<Gdiplus::Bitmap>(w, h, PixelFormat32bppPARGB);
	*pp = ensure_gdiplus_object(bitmap) ? new ComObjectImpl<GdiBitmap>(std::move(bitmap)) : nullptr;
	return S_OK;
}

STDMETHODIMP Gdi::Font(BSTR name, float pxSize, int style, IGdiFont** pp)
{
	if (!pp) return E_POINTER;

	*pp = nullptr;

	auto font = std::make_unique<Gdiplus::Font>(name, pxSize, style, Gdiplus::UnitPixel);
	if (ensure_gdiplus_object(font))
	{
		HFONT hFont = create_font(name, pxSize, style);
		if (hFont != nullptr)
		{
			*pp = new ComObjectImpl<GdiFont>(std::move(font), hFont);
		}
	}
	return S_OK;
}

STDMETHODIMP Gdi::Image(BSTR path, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	*pp = helpers::load_image(path);
	return S_OK;
}

STDMETHODIMP Gdi::LoadImageAsync(UINT window_id, BSTR path, UINT* p)
{
	if (!p) return E_POINTER;

	auto task = std::make_unique<AsyncImageTask>(reinterpret_cast<HWND>(window_id), path);
	*p = task->m_cookie;

	SimpleThreadPool::instance().add_task(std::move(task));
	return S_OK;
}
