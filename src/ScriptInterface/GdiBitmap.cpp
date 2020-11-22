#include "stdafx.h"
#include "GdiBitmap.h"
#include "KMeans.h"
#include "StackBlur.h"

GdiBitmap::GdiBitmap(std::unique_ptr<Gdiplus::Bitmap> bitmap) : m_bitmap(std::move(bitmap)) {}
GdiBitmap::~GdiBitmap() {}

STDMETHODIMP GdiBitmap::get__ptr(void** out)
{
	*out = m_bitmap.get();
	return S_OK;
}

STDMETHODIMP GdiBitmap::ApplyAlpha(UINT8 alpha, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	const auto width = m_bitmap->GetWidth();
	const auto height = m_bitmap->GetHeight();
	auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap.get());
	const Gdiplus::Rect rect(0, 0, width, height);

	Gdiplus::ImageAttributes ia;
	Gdiplus::ColorMatrix cm = { 0.f };
	cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.f;
	cm.m[3][3] = static_cast<float>(alpha) / UINT8_MAX;
	ia.SetColorMatrix(&cm);

	g.DrawImage(m_bitmap.get(), rect, 0, 0, width, height, Gdiplus::UnitPixel, &ia);

	*out = new ComObjectImpl<GdiBitmap>(std::move(bitmap));
	return S_OK;
}

STDMETHODIMP GdiBitmap::ApplyMask(IGdiBitmap* image, VARIANT_BOOL* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	Gdiplus::Bitmap* bitmap_mask = nullptr;
	GET_PTR(image, bitmap_mask);

	if (bitmap_mask->GetHeight() != m_bitmap->GetHeight() || bitmap_mask->GetWidth() != m_bitmap->GetWidth())
	{
		return E_INVALIDARG;
	}

	*out = VARIANT_FALSE;

	const Gdiplus::Rect rect(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	Gdiplus::BitmapData bmpdata_dst, bmpdata_mask;

	if (bitmap_mask->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata_mask) == Gdiplus::Ok)
	{
		if (m_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpdata_dst) == Gdiplus::Ok)
		{
			size_t* mask = static_cast<size_t*>(bmpdata_mask.Scan0);
			size_t* dst = static_cast<size_t*>(bmpdata_dst.Scan0);
			const size_t* mask_end = mask + (rect.Width * rect.Height);
			size_t alpha;

			while (mask < mask_end)
			{
				alpha = (((~*mask & UINT8_MAX) * (*dst >> 24)) << 16) & 0xff000000;
				*dst = alpha | (*dst & 0xffffff);
				++mask;
				++dst;
			}

			m_bitmap->UnlockBits(&bmpdata_dst);
			*out = VARIANT_TRUE;
		}
		bitmap_mask->UnlockBits(&bmpdata_mask);
	}
	return S_OK;
}

STDMETHODIMP GdiBitmap::Clone(float x, float y, float w, float h, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	std::unique_ptr<Gdiplus::Bitmap> bitmap(m_bitmap->Clone(x, y, w, h, PixelFormat32bppPARGB));
	*out = ensure_gdiplus_object(bitmap) ? new ComObjectImpl<GdiBitmap>(std::move(bitmap)) : nullptr;
	return S_OK;
}

STDMETHODIMP GdiBitmap::CreateRawBitmap(IGdiRawBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = new ComObjectImpl<GdiRawBitmap>(m_bitmap.get());
	return S_OK;
}

STDMETHODIMP GdiBitmap::GetColourSchemeJSON(UINT count, BSTR* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	const int w = std::min<int>(m_bitmap->GetWidth(), 220);
	const int h = std::min<int>(m_bitmap->GetHeight(), 220);
	const Gdiplus::Rect rect(0, 0, w, h);
	Gdiplus::BitmapData bmpdata;

	auto bitmap = resize(w, h, Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear);
	if (bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata) != Gdiplus::Ok) return E_POINTER;

	const size_t colours_length = bmpdata.Width * bmpdata.Height;
	const size_t* colours = static_cast<const size_t*>(bmpdata.Scan0);
	static constexpr std::array shifts = { RED_SHIFT, GREEN_SHIFT, BLUE_SHIFT };
	std::map<ColourValues, size_t> colour_counters;

	for (size_t i = 0; i < colours_length; ++i)
	{
		ColourValues values;
		std::transform(shifts.begin(), shifts.end(), values.begin(), [colour = colours[i]](const auto& shift)
			{
				uint8_t value = (colour >> shift) & UINT8_MAX;
				return static_cast<double>(value > 251 ? UINT8_MAX : (value + 4) & 0xf8);
			});

		++colour_counters[values];
	}

	bitmap->UnlockBits(&bmpdata);

	size_t id = 0;
	KPoints points;
	for (const auto& [values, count] : colour_counters)
	{
		points.emplace_back(KPoint(id++, values, count));
	}

	KMeans kmeans(points, count);
	Clusters clusters = kmeans.run();

	json j = json::array();
	for (Cluster& cluster: clusters)
	{
		j.push_back(
			{
				{ "col", cluster.get_colour() },
				{ "freq", cluster.get_frequency(colours_length) },
			});
	}
	*out = to_bstr(j.dump().c_str());
	return S_OK;
}

STDMETHODIMP GdiBitmap::GetGraphics(IGdiGraphics** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = new ComObjectImpl<GdiGraphics>();
	(*out)->put__ptr(new Gdiplus::Graphics(m_bitmap.get()));
	return S_OK;
}

STDMETHODIMP GdiBitmap::InvertColours(IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	Gdiplus::ImageAttributes ia;
	Gdiplus::ColorMatrix cm = { 0.f };
	cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = -1.f;
	cm.m[3][3] = cm.m[4][0] = cm.m[4][1] = cm.m[4][2] = cm.m[4][4] = 1.f;
	ia.SetColorMatrix(&cm);

	const Gdiplus::Rect rect(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	auto bitmap = std::make_unique<Gdiplus::Bitmap>(rect.Width, rect.Height, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap.get());
	g.DrawImage(m_bitmap.get(), rect, 0, 0, rect.Width, rect.Height, Gdiplus::UnitPixel, &ia);

	*out = new ComObjectImpl<GdiBitmap>(std::move(bitmap));
	return S_OK;
}

STDMETHODIMP GdiBitmap::ReleaseGraphics(IGdiGraphics* gr)
{
	Gdiplus::Graphics* graphics = nullptr;
	GET_PTR(gr, graphics);
	gr->put__ptr(nullptr);
	if (graphics) delete graphics;
	return S_OK;
}

STDMETHODIMP GdiBitmap::Resize(UINT w, UINT h, int interpolation_mode, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = new ComObjectImpl<GdiBitmap>(resize(w, h, static_cast<Gdiplus::InterpolationMode>(interpolation_mode)));
	return S_OK;
}

STDMETHODIMP GdiBitmap::RotateFlip(UINT mode)
{
	if (!m_bitmap) return E_POINTER;

	m_bitmap->RotateFlip(static_cast<Gdiplus::RotateFlipType>(mode));
	return S_OK;
}

STDMETHODIMP GdiBitmap::SaveAs(BSTR path, BSTR format, VARIANT_BOOL* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = VARIANT_FALSE;

	size_t num = 0, size = 0;
	if (Gdiplus::GetImageEncodersSize(&num, &size) == Gdiplus::Ok && size > 0)
	{
		pfc::ptrholder_t<Gdiplus::ImageCodecInfo, pfc::releaser_free> pImageCodecInfo = new Gdiplus::ImageCodecInfo[size];
		if (pImageCodecInfo.is_valid() && Gdiplus::GetImageEncoders(num, size, pImageCodecInfo.get_ptr()) == Gdiplus::Ok)
		{
			for (size_t i = 0; i < num; ++i)
			{
				if (wcscmp(pImageCodecInfo.get_ptr()[i].MimeType, format) == 0)
				{
					m_bitmap->Save(path, &pImageCodecInfo.get_ptr()[i].Clsid);
					*out = to_variant_bool(m_bitmap->GetLastStatus() == Gdiplus::Ok);
					break;
				}
			}
		}
	}
	return S_OK;
}

STDMETHODIMP GdiBitmap::StackBlur(UINT8 radius)
{
	if (!m_bitmap) return E_POINTER;

	radius = std::clamp<uint8_t>(radius, 2, 254);
	const Gdiplus::Rect rect(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	Gdiplus::BitmapData bmpdata;

	if (m_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bmpdata) == Gdiplus::Ok)
	{
		uint8_t* src = static_cast<uint8_t*>(bmpdata.Scan0);
		const uint32_t cores = std::max(1U, std::thread::hardware_concurrency());
		const uint32_t div = (radius * 2) + 1;
		std::vector<uint8_t> stack(div * 4 * cores);

		std::vector<std::thread> workers(cores);

		for (uint32_t core = 0; core < cores; ++core)
		{
			workers[core] = std::thread([&, core]()
				{
					StackBlurJob(src, rect.Width, rect.Height, radius, cores, core, 1, &stack[div * 4 * core]);
				});
		}

		for (auto& worker : workers)
		{
			worker.join();
		}

		for (uint32_t core = 0; core < cores; ++core)
		{
			workers[core] = std::thread([&, core]()
				{
					StackBlurJob(src, rect.Width, rect.Height, radius, cores, core, 2, &stack[div * 4 * core]);
				});
		}

		for (auto& worker : workers)
		{
			worker.join();
		}

		m_bitmap->UnlockBits(&bmpdata);
	}

	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Height(UINT* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = m_bitmap->GetHeight();
	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Width(UINT* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = m_bitmap->GetWidth();
	return S_OK;
}

std::unique_ptr<Gdiplus::Bitmap> GdiBitmap::resize(uint32_t w, uint32_t h, Gdiplus::InterpolationMode interpolation_mode)
{
	auto bitmap = std::make_unique<Gdiplus::Bitmap>(w, h, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap.get());
	g.SetInterpolationMode(interpolation_mode);
	g.DrawImage(m_bitmap.get(), 0, 0, w, h);
	return std::move(bitmap);
}

void GdiBitmap::FinalRelease()
{
	if (m_bitmap)
	{
		m_bitmap.reset();
	}
}
