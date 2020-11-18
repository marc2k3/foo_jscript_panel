#include "stdafx.h"
#include "Plman.h"
#include "CustomSort.h"
#include "ProcessLocationsNotify.h"

Plman::Plman() {}
Plman::~Plman() {}

STDMETHODIMP Plman::AddItemToPlaybackQueue(IMetadbHandle* handle)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	playlist_manager::get()->queue_add_item(ptr);
	return S_OK;
}

STDMETHODIMP Plman::AddLocations(UINT playlistIndex, VARIANT locations, VARIANT_BOOL select)
{
	constexpr auto flags = playlist_incoming_item_filter_v2::op_flag_no_filter | playlist_incoming_item_filter_v2::op_flag_delay_ui;
	auto api = playlist_manager::get();
	auto mask = api->playlist_lock_get_filter_mask(playlistIndex);

	if (playlistIndex < api->get_playlist_count() && !(mask & playlist_lock::filter_add))
	{
		pfc::string_list_impl list;
		ComArrayReader reader;
		if (!reader.convert(locations, list)) return E_INVALIDARG;

		auto obj = fb2k::service_new<ProcessLocationsNotify>(playlistIndex, api->playlist_get_item_count(playlistIndex), to_bool(select));
		playlist_incoming_item_filter_v2::get()->process_locations_async(list, flags, nullptr, nullptr, nullptr, obj);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::AddPlaylistItemToPlaybackQueue(UINT playlistIndex, UINT playlistItemIndex)
{
	playlist_manager::get()->queue_add_item_playlist(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::ClearPlaylist(UINT playlistIndex)
{
	playlist_manager::get()->playlist_clear(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::ClearPlaylistSelection(UINT playlistIndex)
{
	playlist_manager::get()->playlist_clear_selection(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::CreateAutoPlaylist(UINT playlistIndex, BSTR name, BSTR query, BSTR sort, UINT flags, int* p)
{
	if (!p) return E_POINTER;

	*p = -1;

	auto uquery = string_utf8_from_wide(query);
	search_filter_v2::ptr filter;

	try
	{
		filter = search_filter_manager_v2::get()->create_ex(uquery, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
	}
	catch (...) {}

	if (filter.is_valid())
	{
		size_t pos;
		CreatePlaylist(playlistIndex, name, &pos);
		autoplaylist_manager::get()->add_client_simple(uquery, string_utf8_from_wide(sort), pos, flags);
		*p = to_int(pos);
	}
	return S_OK;
}

STDMETHODIMP Plman::CreatePlaylist(UINT playlistIndex, BSTR name, UINT* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	auto uname = string_utf8_from_wide(name);

	if (uname.length())
	{
		*p = api->create_playlist(uname, uname.length(), playlistIndex);
	}
	else
	{
		*p = api->create_playlist_autoname(playlistIndex);
	}
	return S_OK;
}

STDMETHODIMP Plman::DuplicatePlaylist(UINT playlistIndex, BSTR name, UINT* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager_v4::get();

	if (playlistIndex < api->get_playlist_count())
	{
		metadb_handle_list items;
		api->playlist_get_all_items(playlistIndex, items);

		string8 uname = string_utf8_from_wide(name).get_ptr();
		if (uname.is_empty())
		{
			api->playlist_get_name(playlistIndex, uname);
		}

		stream_reader_dummy dummy_reader;
		*p = api->create_playlist_ex(uname.get_ptr(), uname.get_length(), playlistIndex + 1, items, &dummy_reader, fb2k::noAbort);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::EnsurePlaylistItemVisible(UINT playlistIndex, UINT playlistItemIndex)
{
	playlist_manager::get()->playlist_ensure_visible(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::ExecutePlaylistDefaultAction(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count() && playlistItemIndex < api->playlist_get_item_count(playlistIndex))
	{
		*p = to_variant_bool(api->playlist_execute_default_action(playlistIndex, playlistItemIndex));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::FindOrCreatePlaylist(BSTR name, VARIANT_BOOL unlocked, UINT* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	auto uname = string_utf8_from_wide(name);

	if (to_bool(unlocked))
	{
		*p = api->find_or_create_playlist_unlocked(uname);
	}
	else
	{
		*p = api->find_or_create_playlist(uname);
	}
	return S_OK;
}

STDMETHODIMP Plman::FindPlaybackQueueItemIndex(IMetadbHandle* handle, UINT playlistIndex, UINT playlistItemIndex, int* p)
{
	if (!p) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	t_playback_queue_item item;
	item.m_handle = ptr;
	item.m_playlist = playlistIndex;
	item.m_item = playlistItemIndex;
	*p = to_int(playlist_manager::get()->queue_find_index(item));
	return S_OK;
}

STDMETHODIMP Plman::FindPlaylist(BSTR name, int* p)
{
	if (!p) return E_POINTER;

	*p = to_int(playlist_manager::get()->find_playlist(string_utf8_from_wide(name)));
	return S_OK;
}

STDMETHODIMP Plman::FlushPlaybackQueue()
{
	playlist_manager::get()->queue_flush();
	return S_OK;
}

STDMETHODIMP Plman::GetPlaybackQueueContents(VARIANT* p)
{
	if (!p) return E_POINTER;

	pfc::list_t<t_playback_queue_item> contents;
	playlist_manager::get()->queue_get_contents(contents);
	const size_t count = contents.get_count();
	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		_variant_t var;
		var.vt = VT_DISPATCH;
		var.pdispVal = new ComObjectImpl<PlaybackQueueItem>(contents[i]);

		if (!writer.put_item(i, var)) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Plman::GetPlaybackQueueHandles(IMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	pfc::list_t<t_playback_queue_item> contents;
	playlist_manager::get()->queue_get_contents(contents);
	const size_t count = contents.get_count();
	metadb_handle_list items;
	for (size_t i = 0; i < count; ++i)
	{
		items.add_item(contents[i].m_handle);
	}
	*pp = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetPlayingItemLocation(IPlayingItemLocation** pp)
{
	if (!pp) return E_POINTER;

	size_t playlistIndex;
	size_t playlistItemIndex;
	bool isValid = playlist_manager::get()->get_playing_item_location(&playlistIndex, &playlistItemIndex);
	*pp = new ComObjectImpl<PlayingItemLocation>(isValid, playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistFocusItemIndex(UINT playlistIndex, int* p)
{
	if (!p) return E_POINTER;

	*p = to_int(playlist_manager::get()->playlist_get_focus_item(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistItems(UINT playlistIndex, IMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	playlist_manager::get()->playlist_get_all_items(playlistIndex, items);
	*pp = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistLockFilterMask(UINT playlistIndex, int* p)
{
	if (!p) return E_POINTER;

	*p = to_int(playlist_manager::get()->playlist_lock_get_filter_mask(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistLockName(UINT playlistIndex, BSTR* p)
{
	if (!p) return E_POINTER;

	string8 str;
	playlist_manager::get()->playlist_lock_query_name(playlistIndex, str);
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistName(UINT playlistIndex, BSTR* p)
{
	if (!p) return E_POINTER;

	string8 str;
	playlist_manager::get()->playlist_get_name(playlistIndex, str);
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistSelectedItems(UINT playlistIndex, IMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	playlist_manager::get()->playlist_get_selected_items(playlistIndex, items);
	*pp = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetRecyclerItems(UINT index, IMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		metadb_handle_list items;
		api->recycler_get_content(index, items);
		*pp = new ComObjectImpl<MetadbHandleList>(items);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::GetRecyclerName(UINT index, BSTR* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		string8 str;
		api->recycler_get_name(index, str);
		*p = to_bstr(str);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::InsertPlaylistItems(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	pfc::bit_array_val selection(to_bool(select));
	playlist_manager::get()->playlist_insert_items(playlistIndex, base, *handles_ptr, selection);
	return S_OK;
}

STDMETHODIMP Plman::InsertPlaylistItemsFilter(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	playlist_manager::get()->playlist_insert_items_filter(playlistIndex, base, *handles_ptr, to_bool(select));
	return S_OK;
}

STDMETHODIMP Plman::IsAutoPlaylist(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	if (playlistIndex < playlist_manager::get()->get_playlist_count())
	{
		*p = to_variant_bool(autoplaylist_manager::get()->is_client_present(playlistIndex));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::IsPlaylistItemSelected(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(playlist_manager::get()->playlist_is_item_selected(playlistIndex, playlistItemIndex));
	return S_OK;
}

STDMETHODIMP Plman::IsPlaylistLocked(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count())
	{
		*p = to_variant_bool(api->playlist_lock_is_present(playlistIndex));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::MovePlaylist(UINT from, UINT to, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	auto api = playlist_manager::get();
	const size_t count = api->get_playlist_count();

	if (from < count && to < count && from != to)
	{
		CustomSort::Order order(count);
		std::iota(order.begin(), order.end(), 0U);
		pfc::create_move_items_permutation(order.data(), count, pfc::bit_array_one(from), to - from);

		*p = to_variant_bool(api->reorder(order.data(), order.size()));
	}
	return S_OK;
}

STDMETHODIMP Plman::MovePlaylistSelection(UINT playlistIndex, int delta, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(playlist_manager::get()->playlist_move_selection(playlistIndex, delta));
	return S_OK;
}

STDMETHODIMP Plman::PlaylistItemCount(UINT playlistIndex, UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->playlist_get_item_count(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::RecyclerPurge(VARIANT affectedItems)
{
	auto api = playlist_manager_v3::get();
	pfc::bit_array_bittable affected(api->recycler_get_count());
	ComArrayReader reader;
	if (!reader.convert(affectedItems, affected)) return E_INVALIDARG;

	if (affected.size())
	{
		api->recycler_purge(affected);
	}
	return S_OK;
}

STDMETHODIMP Plman::RecyclerRestore(UINT index)
{
	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		api->recycler_restore(index);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::RemoveItemFromPlaybackQueue(UINT index)
{
	playlist_manager::get()->queue_remove_mask(pfc::bit_array_one(index));
	return S_OK;
}

STDMETHODIMP Plman::RemoveItemsFromPlaybackQueue(VARIANT affectedItems)
{
	auto api = playlist_manager::get();
	pfc::bit_array_bittable affected(api->queue_get_count());
	ComArrayReader reader;
	if (!reader.convert(affectedItems, affected)) return E_INVALIDARG;

	if (affected.size())
	{
		api->queue_remove_mask(affected);
	}
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylist(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(playlist_manager::get()->remove_playlist(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylists(VARIANT playlistIndexes, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;

	auto api = playlist_manager::get();
	pfc::bit_array_bittable affected(api->get_playlist_count());
	ComArrayReader reader;
	if (!reader.convert(playlistIndexes, affected)) return E_INVALIDARG;

	if (affected.size())
	{
		*p = to_variant_bool(api->remove_playlists(affected));
	}
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylistSelection(UINT playlistIndex, VARIANT_BOOL crop)
{
	playlist_manager::get()->playlist_remove_selection(playlistIndex, to_bool(crop));
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylistSwitch(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(playlist_manager::get()->remove_playlist_switch(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::RenamePlaylist(UINT playlistIndex, BSTR name, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto uname = string_utf8_from_wide(name);
	*p = to_variant_bool(playlist_manager::get()->playlist_rename(playlistIndex, uname, uname.length()));
	return S_OK;
}

STDMETHODIMP Plman::SetActivePlaylistContext()
{
	ui_edit_context_manager::get()->set_context_active_playlist();
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistFocusItem(UINT playlistIndex, UINT playlistItemIndex)
{
	playlist_manager::get()->playlist_set_focus_item(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistFocusItemByHandle(UINT playlistIndex, IMetadbHandle* handle)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	playlist_manager::get()->playlist_set_focus_by_handle(playlistIndex, ptr);
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistSelection(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state)
{
	auto api = playlist_manager::get();
	pfc::bit_array_bittable affected(api->playlist_get_item_count(playlistIndex));
	ComArrayReader reader;
	if (!reader.convert(affectedItems, affected)) return E_INVALIDARG;

	if (affected.size())
	{
		pfc::bit_array_val status(to_bool(state));
		api->playlist_set_selection(playlistIndex, affected, status);
	}
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistSelectionSingle(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state)
{
	playlist_manager::get()->playlist_set_selection_single(playlistIndex, playlistItemIndex, to_bool(state));
	return S_OK;
}

STDMETHODIMP Plman::ShowAutoPlaylistUI(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	if (playlistIndex < playlist_manager::get()->get_playlist_count())
	{
		*p = VARIANT_FALSE;
		auto api = autoplaylist_manager::get();
		if (api->is_client_present(playlistIndex))
		{
			api->query_client(playlistIndex)->show_ui(playlistIndex);
			*p = VARIANT_TRUE;
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::SortByFormat(UINT playlistIndex, BSTR pattern, VARIANT_BOOL selOnly, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto upattern = string_utf8_from_wide(pattern);
	*p = to_variant_bool(playlist_manager::get()->playlist_sort_by_format(playlistIndex, upattern.length() ? upattern.get_ptr() : nullptr, to_bool(selOnly)));
	return S_OK;
}

STDMETHODIMP Plman::SortByFormatV2(UINT playlistIndex, BSTR pattern, int direction, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();

	metadb_handle_list items;
	api->playlist_get_all_items(playlistIndex, items);
	const size_t count = items.get_count();

	CustomSort::Order order(count);

	titleformat_object::ptr obj;
	titleformat_compiler::get()->compile_safe(obj, string_utf8_from_wide(pattern));

	metadb_handle_list_helper::sort_by_format_get_order_v2(items, order.data(), obj, nullptr, direction, fb2k::noAbort);

	*p = to_variant_bool(api->playlist_reorder_items(playlistIndex, order.data(), count));
	return S_OK;
}

STDMETHODIMP Plman::SortPlaylistsByName(int direction)
{
	auto api = playlist_manager::get();
	const size_t count = api->get_playlist_count();
	CustomSort::Items items(count);

	pfc::string8_fast_aggressive str;
	str.prealloc(512);

	for (size_t i = 0; i < count; ++i)
	{
		api->playlist_get_name(i, str);
		items[i].index = i;
		items[i].text = CustomSort::make_sort_string(str);
	}

	CustomSort::Order order = CustomSort::custom_sort(items, direction);
	api->reorder(order.data(), order.size());
	return S_OK;
}

STDMETHODIMP Plman::UndoBackup(UINT playlistIndex)
{
	playlist_manager::get()->playlist_undo_backup(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::get_ActivePlaylist(int* p)
{
	if (!p) return E_POINTER;

	*p = to_int(playlist_manager::get()->get_active_playlist());
	return S_OK;
}

STDMETHODIMP Plman::get_PlaybackOrder(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->playback_order_get_active();
	return S_OK;
}

STDMETHODIMP Plman::get_PlayingPlaylist(int* p)
{
	if (!p) return E_POINTER;

	*p = to_int(playlist_manager::get()->get_playing_playlist());
	return S_OK;
}

STDMETHODIMP Plman::get_PlaylistCount(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->get_playlist_count();
	return S_OK;
}

STDMETHODIMP Plman::get_RecyclerCount(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager_v3::get()->recycler_get_count();
	return S_OK;
}

STDMETHODIMP Plman::put_ActivePlaylist(UINT playlistIndex)
{
	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count())
	{
		api->set_active_playlist(playlistIndex);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Plman::put_PlaybackOrder(UINT p)
{
	auto api = playlist_manager::get();
	if (p < api->playback_order_get_count())
	{
		api->playback_order_set_active(p);
		return S_OK;
	}
	return E_INVALIDARG;
}