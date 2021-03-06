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

	std::wstring name_checked = FontHelpers::check_name(name) ? name : L"Segoe UI";

	auto font = std::make_unique<Gdiplus::Font>(name_checked.data(), pxSize, style, Gdiplus::UnitPixel);
	if (ensure_gdiplus_object(font))
	{
		CFont hFont = FontHelpers::create(name_checked, pxSize, style);
		*out = new ComObjectImpl<GdiFont>(std::move(font), hFont.Detach());
	}
	return S_OK;
}

STDMETHODIMP Gdi::Image(BSTR path, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	*out = ImageHelpers::load(path);
	return S_OK;
}

STDMETHODIMP Gdi::LoadImageAsync(UINT window_id, BSTR path, UINT* out)
{
	if (!out) return E_POINTER;

	auto task = std::make_unique<AsyncImageTask>(reinterpret_cast<HWND>(window_id), path, ++m_image_cookie);
	SimpleThreadPool::instance().add_task(std::move(task));
	*out = m_image_cookie;
	return S_OK;
}
