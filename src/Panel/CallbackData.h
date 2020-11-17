#pragma once

template <typename T1, typename T2 = char, typename T3 = char>
struct CallbackData : public pfc::refcounted_object_root
{
	CallbackData(const T1& item1) : m_item1(item1) {}
	CallbackData(const T1& item1, const T2& item2) : m_item1(item1), m_item2(item2) {}
	CallbackData(const T1& item1, const T2& item2, const T3& item3) : m_item1(item1), m_item2(item2), m_item3(item3) {}

	T1 m_item1;
	T2 m_item2;
	T3 m_item3;
};

struct MetadbCallbackData : public pfc::refcounted_object_root
{
	MetadbCallbackData(metadb_handle_list_cref handles) : m_handles(handles) {}

	metadb_handle_list m_handles;
};

template <class T>
class CallbackDataScopeReleaser
{
public:
	template <class TParam>
	CallbackDataScopeReleaser(TParam data)
	{
		m_data = reinterpret_cast<T*>(data);
	}

	~CallbackDataScopeReleaser()
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
