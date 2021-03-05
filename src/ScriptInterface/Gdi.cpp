#include "stdafx.h"
#include "Gdi.h"
#include "AsyncImageTask.h"

STDMETHODIMP Gdi::CreateImage(int w, int h, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	auto bitmap = std::make_unique<Gdiplus::Bitmap>(w, h, PixelFormat32bppPARGB);
	*out = ensure_gdiplus_object(bitmap) ? new ComObjectImpl<GdiBitmap>(std::move(bitmap)) : nullptr;
	return S_OK;
}

STDMETHODIMP Gdi::Font(BSTR name, float pxSize, int style, IGdiFont** out)
{
	if (!out) return E_POINTER;

	*out = nullptr;

	auto font = std::make_unique<Gdiplus::Font>(name, pxSize, style, Gdiplus::UnitPixel);
	if (ensure_gdiplus_object(font))
	{
		HFONT hFont = create_font(name, pxSize, style);
		if (hFont != nullptr)
		{
			*out = new ComObjectImpl<GdiFont>(std::move(font), hFont);
		}
	}
	return S_OK;
}

STDMETHODIMP Gdi::Image(BSTR path, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	*out = ImageHelper(path).load();
	return S_OK;
}

STDMETHODIMP Gdi::LoadImageAsync(UINT window_id, BSTR path, UINT* out)
{
	if (!out) return E_POINTER;

	auto task = std::make_unique<AsyncImageTask>(reinterpret_cast<HWND>(window_id), path);
	*out = task->m_cookie;

	SimpleThreadPool::instance().add_task(std::move(task));
	return S_OK;
}
