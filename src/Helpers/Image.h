#pragma once
#include <libwebp/decode.h>
#ifdef _DEBUG
#pragma comment(lib, "../lib/libwebp/libwebpdecoder_debug.lib")
#else
#pragma comment(lib, "../lib/libwebp/libwebpdecoder.lib")
#endif

class ImageHelper
{
public:
	ImageHelper(const std::wstring& path) : m_path(path) {}

	static bool is_webp(const void* data, size_t bytes)
	{
		if (bytes < 12) return false;
		return memcmp(data, "RIFF", 4) == 0 && memcmp((const char*)data + 8, "WEBP", 4) == 0;
	}

	static IGdiBitmap* webp_to_bitmap(const uint8_t* data, size_t bytes)
	{
		WebPBitstreamFeatures bs;
		if (WebPGetFeatures(data, bytes, &bs) == VP8_STATUS_OK)
		{
			int w = 0, h = 0;
			pfc::ptrholder_t<uint8_t, pfc::releaser_free> decodedData = WebPDecodeBGRA(data, bytes, &w, &h);

			if (decodedData.is_valid() && w > 0 && h > 0)
			{
				const Gdiplus::PixelFormat pf = bs.has_alpha ? PixelFormat32bppARGB : PixelFormat32bppRGB;
				auto bitmap = std::make_unique<Gdiplus::Bitmap>(w, h, pf);
				if (ensure_gdiplus_object(bitmap))
				{
					const Gdiplus::Rect rect(0, 0, w, h);
					Gdiplus::BitmapData bmpdata;
					if (bitmap->LockBits(&rect, 0, pf, &bmpdata) == Gdiplus::Ok)
					{
						uint8_t* target = static_cast<uint8_t*>(bmpdata.Scan0);
						const uint8_t* source = decodedData.get_ptr();
						const int inc = w * 4;

						for (int y = 0; y < h; ++y)
						{
							memcpy(target, source, inc);
							target += bmpdata.Stride;
							source += inc;
						}

						bitmap->UnlockBits(&bmpdata);
						return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
					}
				}
			}
		}
		return nullptr;
	}

	IGdiBitmap* load()
	{
		pfc::com_ptr_t<IStream> stream;
		if (SUCCEEDED(SHCreateStreamOnFileEx(m_path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, stream.receive_ptr())))
		{
			auto bitmap = std::make_unique<Gdiplus::Bitmap>(stream.get_ptr(), TRUE);
			if (ensure_gdiplus_object(bitmap))
			{
				return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
			}
			else
			{
				STATSTG sts;
				if (SUCCEEDED(stream->Stat(&sts, STATFLAG_DEFAULT)))
				{
					const DWORD bytes = sts.cbSize.LowPart;
					std::vector<uint8_t> ptr(bytes);
					ULONG bytes_read = 0;

					if (SUCCEEDED(stream->Read(ptr.data(), bytes, &bytes_read)) && is_webp(ptr.data(), bytes))
					{
						return webp_to_bitmap(ptr.data(), bytes);
					}
				}
			}
		}
		return nullptr;
	}

private:
	std::wstring m_path;
};

namespace AlbumArt
{
	static GUID id_to_guid(size_t id)
	{
		if (id < guids::art.size())
		{
			return *guids::art[id];
		}
		return album_art_ids::cover_front;
	}

	static IGdiBitmap* data_to_bitmap(const album_art_data_ptr& data)
	{
		const auto ptr = (const uint8_t*)data->get_ptr();
		const size_t bytes = data->get_size();

		pfc::com_ptr_t<IStream> stream;
		stream.attach(SHCreateMemStream(ptr, bytes));
		if (stream.is_valid())
		{
			auto bitmap = std::make_unique<Gdiplus::Bitmap>(stream.get_ptr(), TRUE);
			if (ensure_gdiplus_object(bitmap))
			{
				return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
			}
			else if (ImageHelper::is_webp(ptr, bytes))
			{
				return ImageHelper::webp_to_bitmap(ptr, bytes);
			}
		}
		return nullptr;
	}

	static IGdiBitmap* get(const metadb_handle_ptr& handle, size_t id, bool need_stub, bool no_load, pfc::string_base& image_path)
	{
		const GUID what = id_to_guid(id);

		auto api = album_art_manager_v2::get();
		album_art_data_ptr data;
		album_art_extractor_instance_v2::ptr ptr;

		try
		{
			ptr = api->open(pfc::list_single_ref_t<metadb_handle_ptr>(handle), pfc::list_single_ref_t<GUID>(what), fb2k::noAbort);
			data = ptr->query(what, fb2k::noAbort);
		}
		catch (...)
		{
			if (need_stub)
			{
				try
				{
					ptr = api->open_stub(fb2k::noAbort);
					data = ptr->query(what, fb2k::noAbort);
				}
				catch (...) {}
			}
		}

		if (data.is_valid())
		{
			album_art_path_list::ptr pathlist = ptr->query_paths(what, fb2k::noAbort);
			if (pathlist->get_count() > 0)
			{
				filesystem::g_get_display_path(pathlist->get_path(0), image_path);
			}
			if (!no_load)
			{
				return data_to_bitmap(data);
			}
		}
		return nullptr;
	}

	static IGdiBitmap* get_embedded(jstring path, size_t id)
	{
		const GUID what = id_to_guid(id);

		album_art_extractor::ptr ptr;
		if (album_art_extractor::g_get_interface(ptr, path))
		{
			try
			{
				album_art_data_ptr data = ptr->open(nullptr, path, fb2k::noAbort)->query(what, fb2k::noAbort);
				if (data.is_valid()) return data_to_bitmap(data);
			}
			catch (...) {}
		}
		return nullptr;
	}
}
