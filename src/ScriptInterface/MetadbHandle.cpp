#include "stdafx.h"
#include "MetadbHandle.h"
#include "db.h"

MetadbHandle::MetadbHandle(const metadb_handle_ptr& handle) : m_handle(handle) {}

STDMETHODIMP MetadbHandle::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = m_handle.get_ptr();
	return S_OK;
}

STDMETHODIMP MetadbHandle::ClearStats()
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::set(hash, db::Fields());
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::Compare(IMetadbHandle* handle, VARIANT_BOOL* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	*out = to_variant_bool(ptr == m_handle.get_ptr());
	return S_OK;
}

STDMETHODIMP MetadbHandle::GetAlbumArt(UINT art_id, VARIANT_BOOL need_stub, VARIANT* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	string8 image_path;
	album_art_data_ptr data = AlbumArt::get(m_handle, art_id, to_bool(need_stub), image_path);
	_variant_t var = AlbumArt::data_to_bitmap(data);

	ComArrayWriter writer;
	if (!writer.create(2)) return E_OUTOFMEMORY;
	if (!writer.put_item(0, var)) return E_OUTOFMEMORY;
	if (!writer.put_item(1, image_path)) return E_OUTOFMEMORY;

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP MetadbHandle::GetFileInfo(IFileInfo** out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	*out = new ComObjectImpl<FileInfo>(m_handle->get_info_ref());
	return S_OK;
}

STDMETHODIMP MetadbHandle::RefreshStats()
{
	if (m_handle.is_empty()) return E_POINTER;

	db::refresh(pfc::list_single_ref_t<metadb_handle_ptr>(m_handle));
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetFirstPlayed(BSTR first_played)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields f = db::get(hash);
		const string8 fp = from_wide(first_played);
		if (!f.first_played.equals(fp))
		{
			f.first_played = fp;
			db::set(hash, f);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetLastPlayed(BSTR last_played)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields f = db::get(hash);
		const string8 lp = from_wide(last_played);
		if (!f.last_played.equals(lp))
		{
			f.last_played = lp;
			db::set(hash, f);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetLoved(UINT loved)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields f = db::get(hash);
		if (f.loved != loved)
		{
			f.loved = loved;
			db::set(hash, f);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetPlaycount(UINT playcount)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields f = db::get(hash);
		if (f.playcount != playcount)
		{
			f.playcount = playcount;
			db::set(hash, f);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetRating(UINT rating)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash = 0;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields f = db::get(hash);
		if (f.rating != rating)
		{
			f.rating = rating;
			db::set(hash, f);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::ShowAlbumArtViewer(UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed)
{
	if (m_handle.is_empty()) return E_POINTER;

	string8 message;
	auto api = fb2k::imageViewer::tryGet();
	if (api.is_valid())
	{
		album_art_data_ptr data;
		if (to_bool(only_embed))
		{
			data = AlbumArt::get_embedded(m_handle->get_path(), art_id);
		}
		else
		{
			string8 dummy;
			data = AlbumArt::get(m_handle, art_id, to_bool(need_stub), dummy);
		}

		if (data.is_valid())
		{
			api->show(core_api::get_main_window(), data);
		}
		else
		{
			message = "Album art not found.";
		}
	}
	else
	{
		message = "ShowAlbumArtViewer requires foobar2000 v1.6.2 or later.";
	}

	if (message.get_length() > 0)
	{
		popup_message::g_show(message, jsp::component_name);
	}

	return S_OK;
}

STDMETHODIMP MetadbHandle::get_FileSize(UINT64* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	*out = m_handle->get_filesize();
	return S_OK;
}

STDMETHODIMP MetadbHandle::get_Length(double* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	*out = m_handle->get_length();
	return S_OK;
}

STDMETHODIMP MetadbHandle::get_Path(BSTR* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	string8 str;
	filesystem::g_get_display_path(m_handle->get_path(), str);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP MetadbHandle::get_RawPath(BSTR* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	*out = to_bstr(m_handle->get_path());
	return S_OK;
}

STDMETHODIMP MetadbHandle::get_SubSong(UINT* out)
{
	if (m_handle.is_empty() || !out) return E_POINTER;

	*out = m_handle->get_subsong_index();
	return S_OK;
}

void MetadbHandle::FinalRelease()
{
	m_handle.release();
}
