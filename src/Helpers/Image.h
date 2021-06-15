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

	static IGdiBitmap* webp_to_bitmap(const uint8_t* data, size_t bytes)
	{
		if (bytes < 12 || memcmp(data, "RIFF", 4) != 0 || memcmp((const char*)data + 8, "WEBP", 4) != 0) return nullptr;

		WebPBitstreamFeatures bs;
		int width = 0, height = 0;

		if (WebPGetFeatures(data, bytes, &bs) == VP8_STATUS_OK && WebPGetInfo(data, bytes, &width, &height))
		{
			const Gdiplus::PixelFormat pf = bs.has_alpha ? PixelFormat32bppARGB : PixelFormat32bppRGB;
			auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, pf);
			if (ensure_gdiplus_object(bitmap))
			{
				const Gdiplus::Rect rect(0, 0, width, height);
				Gdiplus::BitmapData bmpdata;
				if (bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, pf, &bmpdata) == Gdiplus::Ok)
				{
					if (WebPDecodeBGRAInto(data, bytes, static_cast<uint8_t*>(bmpdata.Scan0), bmpdata.Stride * height, bmpdata.Stride))
					{
						bitmap->UnlockBits(&bmpdata);
						return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
					}
				}
			}
		}
		return nullptr;
	}

	static bool istream_to_buffer(IStream* stream, ImageBuffer& buffer)
	{
		STATSTG sts;
		if (SUCCEEDED(stream->Stat(&sts, STATFLAG_DEFAULT)))
		{
			const DWORD bytes = sts.cbSize.LowPart;
			buffer.resize(bytes);
			ULONG bytes_read = 0;
			return SUCCEEDED(stream->Read(buffer.data(), bytes, &bytes_read)) && bytes == bytes_read;
		}
		return false;
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
				ImageBuffer buffer;
				if (istream_to_buffer(stream.get_ptr(), buffer))
				{
					return webp_to_bitmap(buffer.data(), buffer.size());
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
		if (data.is_valid())
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
				else
				{
					return ImageHelper::webp_to_bitmap(ptr, bytes);
				}
			}
		}
		return nullptr;
	}

	static album_art_data_ptr get(const metadb_handle_ptr& handle, size_t id, bool need_stub, pfc::string_base& image_path)
	{
		const GUID what = id_to_guid(id);
		album_art_data_ptr data;
		album_art_extractor_instance_v2::ptr ptr;
		album_art_path_list::ptr pathlist;
		auto api = album_art_manager_v2::get();

		try
		{
			ptr = api->open(pfc::list_single_ref_t<metadb_handle_ptr>(handle), pfc::list_single_ref_t<GUID>(what), fb2k::noAbort);
			data = ptr->query(what, fb2k::noAbort);
			pathlist = ptr->query_paths(what, fb2k::noAbort);
		}
		catch (...)
		{
			if (core_api::is_main_thread() && what == album_art_ids::cover_front)
			{
				const std::string path = handle->get_path();
				if (!path.starts_with("file://"))
				{
					metadb_handle_ptr np;
					if (playback_control::get()->get_now_playing(np) && path == np->get_path())
					{
						data = now_playing_album_art_notify_manager::get()->current();
						if (data.is_valid()) return data;
					}
				}
			}

			if (need_stub)
			{
				try
				{
					ptr = api->open_stub(fb2k::noAbort);
					data = ptr->query(what, fb2k::noAbort);
					pathlist = ptr->query_paths(what, fb2k::noAbort);
				}
				catch (...) {}
			}
		}

		if (pathlist.is_valid() && pathlist->get_count() > 0)
		{
			filesystem::g_get_display_path(pathlist->get_path(0), image_path);
		}
		return data;
	}

	static album_art_data_ptr get_embedded(jstring path, size_t id)
	{
		album_art_data_ptr data;
		album_art_extractor::ptr ptr;
		if (album_art_extractor::g_get_interface(ptr, path))
		{
			try
			{
				data = ptr->open(nullptr, path, fb2k::noAbort)->query(id_to_guid(id), fb2k::noAbort);
			}
			catch (...) {}
		}
		return data;
	}
}
