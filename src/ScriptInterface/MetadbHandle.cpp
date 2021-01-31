#include "stdafx.h"
#include "MetadbHandle.h"
#include "db.h"

MetadbHandle::MetadbHandle(const metadb_handle_ptr& handle) : m_handle(handle) {}
MetadbHandle::~MetadbHandle() {}

STDMETHODIMP MetadbHandle::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = m_handle.get_ptr();
	return S_OK;
}

STDMETHODIMP MetadbHandle::ClearStats()
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
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
	IGdiBitmap* bitmap = AlbumArt::get(m_handle, art_id, to_bool(need_stub), image_path);

	_variant_t var;
	var.vt = VT_DISPATCH;
	var.pdispVal = bitmap;

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

	metadb_index_hash hash;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields tmp = db::get(hash);
		const string8 fp = from_wide(first_played);
		if (!tmp.first_played.equals(fp))
		{
			tmp.first_played = fp;
			db::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetLastPlayed(BSTR last_played)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields tmp = db::get(hash);
		const string8 lp = from_wide(last_played);
		if (!tmp.last_played.equals(lp))
		{
			tmp.last_played = lp;
			db::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetLoved(UINT loved)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields tmp = db::get(hash);
		if (tmp.loved != loved)
		{
			tmp.loved = loved;
			db::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetPlaycount(UINT playcount)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields tmp = db::get(hash);
		if (tmp.playcount != playcount)
		{
			tmp.playcount = playcount;
			db::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandle::SetRating(UINT rating)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (db::hashHandle(m_handle, hash))
	{
		db::Fields tmp = db::get(hash);
		if (tmp.rating != rating)
		{
			tmp.rating = rating;
			db::set(hash, tmp);
		}
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
