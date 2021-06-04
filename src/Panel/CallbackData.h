#pragma once

struct CallbackData : public pfc::refcounted_object_root
{
	CallbackData(const VariantArgs& args) : m_args(args) {}

	VariantArgs m_args;
};

struct MetadbCallbackData : public pfc::refcounted_object_root
{
	MetadbCallbackData(const metadb_handle_ptr& handle) : m_handles(pfc::list_single_ref_t<metadb_handle_ptr>(handle)) {}
	MetadbCallbackData(metadb_handle_list_cref handles) : m_handles(handles) {}

	metadb_handle_list m_handles;
};

template <class T>
class CallbackDataReleaser
{
public:
	template <class TParam>
	CallbackDataReleaser(TParam data)
	{
		m_data = reinterpret_cast<T*>(data);
	}

	~CallbackDataReleaser()
	{
		m_data->refcount_release();
	}

	T* operator->()
	{
		return m_data;
	}

private:
	T* m_data;
};

struct AsyncArtData
{
	AsyncArtData(IMetadbHandle* handle, size_t art_id, IGdiBitmap* bitmap, BSTR path) : m_handle(handle), m_art_id(art_id), m_bitmap(bitmap), m_path(path) {}

	~AsyncArtData()
	{
		if (m_handle)
		{
			m_handle->Release();
		}

		if (m_bitmap)
		{
			m_bitmap->Release();
		}
	}

	IGdiBitmap* m_bitmap;
	IMetadbHandle* m_handle;
	_bstr_t m_path;
	size_t m_art_id;
};

struct AsyncImageData
{
	AsyncImageData(size_t cookie, IGdiBitmap* bitmap, BSTR path) : m_cookie(cookie), m_bitmap(bitmap), m_path(path) {}

	~AsyncImageData()
	{
		if (m_bitmap)
		{
			m_bitmap->Release();
		}
	}

	IGdiBitmap* m_bitmap;
	_bstr_t m_path;
	size_t m_cookie;
};
