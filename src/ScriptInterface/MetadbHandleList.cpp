#include "stdafx.h"
#include "MetadbHandleList.h"
#include "db.h"

#include "CustomSort.h"
#include "Embed.h"
#include "FileInfoFilter.h"
#include "OptimiseLayout.h"

MetadbHandleList::MetadbHandleList(metadb_handle_list_cref handles) : m_handles(handles) {}
MetadbHandleList::~MetadbHandleList() {}

STDMETHODIMP MetadbHandleList::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = &m_handles;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Add(IMetadbHandle* handle)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	m_handles.add_item(ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::AddRange(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	m_handles.add_items(*handles_ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::AttachImage(BSTR path, UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	pfc::com_ptr_t<IStream> stream;
	if (SUCCEEDED(SHCreateStreamOnFileEx(path, STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, stream.receive_ptr())))
	{
		STATSTG sts;
		if (SUCCEEDED(stream->Stat(&sts, STATFLAG_DEFAULT)))
		{
			const DWORD bytes = sts.cbSize.LowPart;
			std::vector<uint8_t> ptr(bytes);
			ULONG bytes_read = 0;

			if (SUCCEEDED(stream->Read(ptr.data(), bytes, &bytes_read)))
			{
				album_art_data_ptr data = album_art_data_impl::g_create(ptr.data(), bytes);

				if (data.is_valid())
				{
					auto cb = fb2k::service_new<Embed>(m_handles, art_id, data);
					threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Embedding image...");
				}
			}
		}
	}
	return S_OK;
}

STDMETHODIMP MetadbHandleList::BSearch(IMetadbHandle* handle, int* p)
{
	if (!p) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	*p = to_int(m_handles.bsearch_by_pointer(ptr));
	return S_OK;
}

STDMETHODIMP MetadbHandleList::CalcTotalDuration(double* p)
{
	if (!p) return E_POINTER;

	*p = m_handles.calc_total_duration();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::CalcTotalSize(UINT64* p)
{
	if (!p) return E_POINTER;

	*p = metadb_handle_list_helper::calc_total_size(m_handles, true);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Clone(IMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	*pp = new ComObjectImpl<MetadbHandleList>(m_handles);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Convert(VARIANT* p)
{
	if (!p) return E_POINTER;

	const size_t count = m_handles.get_count();
	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		_variant_t var;
		var.vt = VT_DISPATCH;
		var.pdispVal = new ComObjectImpl<MetadbHandle>(m_handles[i]);
		if (!writer.put_item(i, var)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Find(IMetadbHandle* handle, int* p)
{
	if (!p) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	*p = to_int(m_handles.find_item(ptr));
	return S_OK;
}

STDMETHODIMP MetadbHandleList::GetLibraryRelativePaths(VARIANT* p)
{
	if (!p) return E_POINTER;

	auto api = library_manager::get();
	const size_t count = m_handles.get_count();

	pfc::string8_fast_aggressive str;
	str.prealloc(512);

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		metadb_handle_ptr item = m_handles[i];
		if (!api->get_relative_path(item, str)) str = "";
		if (!writer.put_item(i, str)) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Insert(UINT index, IMetadbHandle* handle)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	m_handles.insert_item(ptr, index);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::InsertRange(UINT index, IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	m_handles.insert_items(*handles_ptr, index);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::MakeDifference(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	metadb_handle_list r1, r2;
	metadb_handle_list_helper::sorted_by_pointer_extract_difference(m_handles, *handles_ptr, r1, r2);
	m_handles = r1;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::MakeIntersection(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	const metadb_handle_list_ref handles_ref = *handles_ptr;
	metadb_handle_list result;
	size_t walk1 = 0;
	size_t walk2 = 0;
	const size_t last1 = m_handles.get_count();
	const size_t last2 = handles_ptr->get_count();

	while (walk1 != last1 && walk2 != last2)
	{
		if (m_handles[walk1] < handles_ref[walk2])
			++walk1;
		else if (handles_ref[walk2] < m_handles[walk1])
			++walk2;
		else
		{
			result.add_item(m_handles[walk1]);
			++walk1;
			++walk2;
		}
	}

	m_handles = result;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::MakeUnion(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	m_handles.add_items(*handles_ptr);
	m_handles.sort_by_pointer_remove_duplicates();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OptimiseFileLayout(VARIANT_BOOL minimise)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	auto cb = fb2k::service_new<OptimiseLayout>(m_handles, to_bool(minimise));
	threaded_process::get()->run_modeless(cb, flags | threaded_process::flag_show_abort, core_api::get_main_window(), "Optimising...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByFormat(__interface ITitleFormat* script, int direction)
{
	titleformat_object* obj = nullptr;
	GET_PTR(script, obj);

	m_handles.sort_by_format(obj, nullptr, direction);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByPath()
{
	m_handles.sort_by_path();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByRelativePath()
{
	auto api = library_manager::get();
	const size_t count = m_handles.get_count();
	CustomSort::Items items(count);

	pfc::string8_fast_aggressive str;
	str.prealloc(512);

	for (size_t i = 0; i < count; ++i)
	{
		metadb_handle_ptr item = m_handles[i];
		if (!api->get_relative_path(item, str)) str = "";
		str << item->get_subsong_index();
		items[i].index = i;
		items[i].text = CustomSort::make_sort_string(str);
	}

	CustomSort::Order order = CustomSort::custom_sort(items);
	m_handles.reorder(order.data());
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RefreshStats()
{
	db::refresh(m_handles);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Remove(IMetadbHandle* handle)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	m_handles.remove_item(ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAll()
{
	m_handles.remove_all();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAttachedImage(UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	auto cb = fb2k::service_new<Embed>(m_handles, art_id);
	threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAttachedImages()
{
	if (m_handles.get_count() == 0) return E_POINTER;

	auto cb = fb2k::service_new<Embed>(m_handles);
	threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveById(UINT index)
{
	if (index < m_handles.get_count())
	{
		m_handles.remove_by_idx(index);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP MetadbHandleList::RemoveRange(UINT from, UINT count)
{
	m_handles.remove_from_idx(from, count);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Sort()
{
	m_handles.sort_by_pointer_remove_duplicates();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::UpdateFileInfoFromJSON(BSTR str)
{
	const size_t count = m_handles.get_count();
	if (count == 0) return E_POINTER;

	json j = json::parse(string_utf8_from_wide(str).get_ptr(), nullptr, false);

	if (j.is_array() && j.size() == count)
	{
		std::vector<file_info_impl> info(count);

		for (size_t i = 0; i < count; ++i)
		{
			if (!j[i].is_object() || j[i].size() == 0) return E_INVALIDARG;

			info[i] = m_handles[i]->get_info_ref()->info();

			for (auto& [name, value] : j[i].items())
			{
				if (name.empty()) return E_INVALIDARG;

				info[i].meta_remove_field(name.c_str());

				for (const std::string& v : FileInfoFilter::g_get_values(value))
				{
					info[i].meta_add(name.c_str(), v.c_str());
				}
			}
		}

		metadb_io_v2::get()->update_info_async_simple(
			m_handles,
			pfc::ptr_list_const_array_t<const file_info, file_info_impl*>(info.data(), info.size()),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_delay_ui,
			nullptr
		);
		return S_OK;
	}
	else if (j.is_object() && j.size() > 0)
	{
		FileInfoFilter::Tags tags;

		for (auto& [name, value] : j.items())
		{
			if (name.empty()) return E_INVALIDARG;

			FileInfoFilter::Tag tag;
			tag.name = name;
			tag.values = FileInfoFilter::g_get_values(value);
			tags.emplace_back(tag);
		}

		metadb_io_v2::get()->update_info_async(
			m_handles,
			fb2k::service_new<FileInfoFilter>(tags),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_delay_ui,
			nullptr
		);
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}

STDMETHODIMP MetadbHandleList::get_Count(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_handles.get_count();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::get_Item(UINT index, IMetadbHandle** pp)
{
	if (!pp) return E_POINTER;

	if (index < m_handles.get_count())
	{
		*pp = new ComObjectImpl<MetadbHandle>(m_handles.get_item_ref(index));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP MetadbHandleList::put_Item(UINT index, IMetadbHandle* handle)
{
	if (index < m_handles.get_count())
	{
		metadb_handle* ptr = nullptr;
		GET_PTR(handle, ptr);

		m_handles.replace_item(index, ptr);
		return S_OK;
	}
	return E_INVALIDARG;
}

void MetadbHandleList::FinalRelease()
{
	m_handles.remove_all();
}