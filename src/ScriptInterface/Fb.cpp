#include "stdafx.h"
#include "Fb.h"
#include "DropSourceImpl.h"
#include "Menu.h"

Fb::Fb() {}
Fb::~Fb() {}

STDMETHODIMP Fb::AcquireUiSelectionHolder(IUiSelectionHolder** out)
{
	if (!out) return E_POINTER;

	ui_selection_holder::ptr holder = ui_selection_manager::get()->acquire();
	*out = new ComObjectImpl<UiSelectionHolder>(holder);
	return S_OK;
}

STDMETHODIMP Fb::AddDirectory()
{
	standard_commands::main_add_directory();
	return S_OK;
}

STDMETHODIMP Fb::AddFiles()
{
	standard_commands::main_add_files();
	return S_OK;
}

STDMETHODIMP Fb::CheckClipboardContents(UINT /* FFS */, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;
	pfc::com_ptr_t<IDataObject> obj;
	if (SUCCEEDED(OleGetClipboard(obj.receive_ptr())))
	{
		bool native;
		DWORD drop_effect = DROPEFFECT_COPY;
		*out = to_variant_bool(SUCCEEDED(ole_interaction::get()->check_dataobject(obj, drop_effect, native)));
	}
	return S_OK;
}

STDMETHODIMP Fb::ClearPlaylist()
{
	standard_commands::main_clear_playlist();
	return S_OK;
}

STDMETHODIMP Fb::CopyHandleListToClipboard(IMetadbHandleList* handles, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	auto obj = ole_interaction::get()->create_dataobject(*handles_ptr);
	*out = to_variant_bool(SUCCEEDED(OleSetClipboard(obj.get_ptr())));
	return S_OK;
}

STDMETHODIMP Fb::CreateContextMenuManager(IContextMenuManager** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<ContextMenuManager>();
	return S_OK;
}

STDMETHODIMP Fb::CreateHandleList(VARIANT handle, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	IDispatch* temp = nullptr;

	if (handle.vt == VT_DISPATCH && handle.pdispVal && SUCCEEDED(handle.pdispVal->QueryInterface(__uuidof(IMetadbHandle), reinterpret_cast<void**>(&temp))))
	{
		IDispatchPtr handle_s = temp;
		void* ptr = nullptr;
		reinterpret_cast<IMetadbHandle*>(handle_s.GetInterfacePtr())->get__ptr(&ptr);
		if (!ptr) return E_INVALIDARG;

		items.add_item(reinterpret_cast<metadb_handle*>(ptr));
	}
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::CreateMainMenuManager(IMainMenuManager** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<MainMenuManager>();
	return S_OK;
}

STDMETHODIMP Fb::CreateProfiler(BSTR name, IProfiler** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<Profiler>(string_utf8_from_wide(name));
	return S_OK;
}

STDMETHODIMP Fb::DoDragDrop(IMetadbHandleList* handles, UINT okEffects, UINT* out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	*out = DROPEFFECT_NONE;

	if (handles_ptr->get_count() && okEffects != DROPEFFECT_NONE)
	{
		auto obj = ole_interaction::get()->create_dataobject(*handles_ptr);
		pfc::com_ptr_t<DropSourceImpl> source = new DropSourceImpl();

		DWORD returnEffect;
		if (::DoDragDrop(obj.get_ptr(), source.get_ptr(), okEffects, &returnEffect) != DRAGDROP_S_CANCEL)
		{
			*out = returnEffect;
		}
	}
	return S_OK;
}

STDMETHODIMP Fb::Exit()
{
	standard_commands::main_exit();
	return S_OK;
}

STDMETHODIMP Fb::GetClipboardContents(UINT /* FFS */, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	auto api = ole_interaction::get();
	pfc::com_ptr_t<IDataObject> obj;
	metadb_handle_list items;

	if (SUCCEEDED(OleGetClipboard(obj.receive_ptr())))
	{
		DWORD drop_effect = DROPEFFECT_COPY;
		bool native;
		if (SUCCEEDED(api->check_dataobject(obj, drop_effect, native)))
		{
			dropped_files_data_impl data;
			if (SUCCEEDED(api->parse_dataobject(obj, data)))
			{
				data.to_handles(items, native, core_api::get_main_window());
			}
		}
	}

	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetDSPPresets(BSTR* out)
{
	if (!out) return E_POINTER;

	json j = json::array();
	auto api = dsp_config_manager_v2::get();
	const size_t count = api->get_preset_count();
	string8 name;

	for (size_t i = 0; i < count; ++i)
	{
		api->get_preset_name(i, name);

		j.push_back(
			{
				{ "active", api->get_selected_preset() == i },
				{ "name", name.get_ptr() }
			});
	}
	*out = to_bstr(j.dump().c_str());
	return S_OK;
}

STDMETHODIMP Fb::GetFocusItem(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_ptr metadb;
	*out = playlist_manager::get()->activeplaylist_get_focus_item_handle(metadb) ? new ComObjectImpl<MetadbHandle>(metadb) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetLibraryItems(IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	library_manager::get()->get_all_items(items);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetNowPlaying(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_ptr metadb;
	*out = playback_control::get()->get_now_playing(metadb) ? new ComObjectImpl<MetadbHandle>(metadb) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetOutputDevices(BSTR* out)
{
	if (!out) return E_POINTER;

	json j = json::array();
	auto api = output_manager_v2::get();
	outputCoreConfig_t config;
	api->getCoreConfig(config);

	api->listDevices([&](auto&& name, auto&& output_id, auto&& device_id)
		{
			j.push_back(
				{
					{ "name", name },
					{ "output_id", PFC_string_formatter() << "{" << pfc::print_guid(output_id) << "}" },
					{ "device_id", PFC_string_formatter() << "{" << pfc::print_guid(device_id) << "}" },
					{ "active", config.m_output == output_id && config.m_device == device_id }
				});
		});
	*out = to_bstr(j.dump().c_str());
	return S_OK;
}

STDMETHODIMP Fb::GetQueryItems(IMetadbHandleList* handles, BSTR query, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	search_filter_v2::ptr filter;

	try
	{
		filter = search_filter_manager_v2::get()->create_ex(string_utf8_from_wide(query), fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
	}
	catch (...) {}

	if (filter.is_valid())
	{
		metadb_handle_list copy(*handles_ptr);
		pfc::array_t<bool> mask;
		mask.set_size(copy.get_count());
		filter->test_multi(copy, mask.get_ptr());
		copy.filter_mask(mask.get_ptr());
		*out = new ComObjectImpl<MetadbHandleList>(copy);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Fb::GetSelection(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	ui_selection_manager::get()->get_selection(items);
	*out = items.get_count() ? new ComObjectImpl<MetadbHandle>(items[0]) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetSelections(UINT flags, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	ui_selection_manager_v2::get()->get_selection(items, flags);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetSelectionType(UINT* out)
{
	if (!out) return E_POINTER;

	const GUID type = ui_selection_manager_v2::get()->get_selection_type(0);
	const auto it = FIND_IF(guids::selections, [type](const GUID* g) { return *g == type; });
	*out = it != guids::selections.end() ? std::distance(guids::selections.begin(), it) : 0;
	return S_OK;
}

STDMETHODIMP Fb::IsLibraryEnabled(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(library_manager::get()->is_library_enabled());
	return S_OK;
}

STDMETHODIMP Fb::IsMetadbInMediaLibrary(IMetadbHandle* handle, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	*out = to_variant_bool(library_manager::get()->is_item_in_library(ptr));
	return S_OK;
}

STDMETHODIMP Fb::LoadPlaylist()
{
	standard_commands::main_load_playlist();
	return S_OK;
}

STDMETHODIMP Fb::Next()
{
	standard_commands::main_next();
	return S_OK;
}

STDMETHODIMP Fb::Pause()
{
	standard_commands::main_pause();
	return S_OK;
}

STDMETHODIMP Fb::Play()
{
	standard_commands::main_play();
	return S_OK;
}

STDMETHODIMP Fb::PlayOrPause()
{
	standard_commands::main_play_or_pause();
	return S_OK;
}

STDMETHODIMP Fb::Prev()
{
	standard_commands::main_previous();
	return S_OK;
}

STDMETHODIMP Fb::Random()
{
	standard_commands::main_random();
	return S_OK;
}

STDMETHODIMP Fb::RunContextCommand(BSTR command, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;
	if (playback_control::get()->is_playing())
	{
		contextmenu_manager::ptr cm;
		contextmenu_manager::g_create(cm);
		cm->init_context_now_playing(contextmenu_manager::flag_view_full);
		*out = to_variant_bool(helpers::execute_context_command_recur(cm->get_root(), string_utf8_from_wide(command)));
	}
	return S_OK;
}

STDMETHODIMP Fb::RunContextCommandWithMetadb(BSTR command, VARIANT handle, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	if (handle.vt != VT_DISPATCH || !handle.pdispVal) return E_INVALIDARG;

	*out = VARIANT_FALSE;

	metadb_handle_list handle_list;
	IDispatch* temp = nullptr;
	IDispatchPtr handle_s = nullptr;
	void* ptr = nullptr;

	if (SUCCEEDED(handle.pdispVal->QueryInterface(__uuidof(IMetadbHandle), reinterpret_cast<void**>(&temp))))
	{
		handle_s = temp;
		reinterpret_cast<IMetadbHandle*>(handle_s.GetInterfacePtr())->get__ptr(&ptr);
		if (!ptr) return E_INVALIDARG;
		handle_list = pfc::list_single_ref_t<metadb_handle_ptr>(reinterpret_cast<metadb_handle*>(ptr));
	}
	else if (SUCCEEDED(handle.pdispVal->QueryInterface(__uuidof(IMetadbHandleList), reinterpret_cast<void**>(&temp))))
	{
		handle_s = temp;
		reinterpret_cast<IMetadbHandleList*>(handle_s.GetInterfacePtr())->get__ptr(&ptr);
		if (!ptr) return E_INVALIDARG;
		handle_list = *reinterpret_cast<metadb_handle_list*>(ptr);
	}
	else
	{
		return E_INVALIDARG;
	}

	if (handle_list.get_count() > 0)
	{
		contextmenu_manager::ptr cm;
		contextmenu_manager::g_create(cm);
		cm->init_context(handle_list, contextmenu_manager::flag_view_full);
		*out = to_variant_bool(helpers::execute_context_command_recur(cm->get_root(), string_utf8_from_wide(command)));
	}
	return S_OK;
}

STDMETHODIMP Fb::RunMainMenuCommand(BSTR command, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(helpers::execute_mainmenu_command(string_utf8_from_wide(command)));
	return S_OK;
}

STDMETHODIMP Fb::SavePlaylist()
{
	standard_commands::main_save_playlist();
	return S_OK;
}

STDMETHODIMP Fb::SetDSPPreset(UINT idx)
{
	auto api = dsp_config_manager_v2::get();
	const size_t count = api->get_preset_count();
	if (idx < count)
	{
		api->select_preset(idx);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Fb::SetOutputDevice(BSTR output, BSTR device)
{
	GUID output_id, device_id;
	if (CLSIDFromString(output, &output_id) == NOERROR && CLSIDFromString(device, &device_id) == NOERROR)
	{
		output_manager_v2::get()->setCoreConfigDevice(output_id, device_id);
	}
	return S_OK;
}

STDMETHODIMP Fb::ShowConsole()
{
	standard_commands::main_show_console();
	return S_OK;
}

STDMETHODIMP Fb::ShowLibrarySearchUI(BSTR query)
{
	if (!query) return E_INVALIDARG;

	library_search_ui::get()->show(string_utf8_from_wide(query));
	return S_OK;
}

STDMETHODIMP Fb::ShowPopupMessage(BSTR msg, BSTR title)
{
	auto umsg = string_utf8_from_wide(msg);
	auto utitle = string_utf8_from_wide(title);
	fb2k::inMainThread([=]()
		{
			popup_message::g_show(umsg, utitle);
		});
	return S_OK;
}

STDMETHODIMP Fb::ShowPreferences()
{
	standard_commands::main_preferences();
	return S_OK;
}

STDMETHODIMP Fb::Stop()
{
	standard_commands::main_stop();
	return S_OK;
}

STDMETHODIMP Fb::TitleFormat(BSTR pattern, ITitleFormat** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<::TitleFormat>(string_utf8_from_wide(pattern));
	return S_OK;
}

STDMETHODIMP Fb::VolumeDown()
{
	standard_commands::main_volume_down();
	return S_OK;
}

STDMETHODIMP Fb::VolumeMute()
{
	standard_commands::main_volume_mute();
	return S_OK;
}

STDMETHODIMP Fb::VolumeUp()
{
	standard_commands::main_volume_up();
	return S_OK;
}

STDMETHODIMP Fb::get_AlwaysOnTop(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_ui_always_on_top, false));
	return S_OK;
}

STDMETHODIMP Fb::get_ComponentPath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(helpers::get_fb2k_component_path());
	return S_OK;
}

STDMETHODIMP Fb::get_CursorFollowPlayback(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_cursor_follows_playback, false));
	return S_OK;
}

STDMETHODIMP Fb::get_FoobarPath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(helpers::get_fb2k_path());
	return S_OK;
}

STDMETHODIMP Fb::get_IsPaused(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->is_paused());
	return S_OK;
}

STDMETHODIMP Fb::get_IsPlaying(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->is_playing());
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackFollowCursor(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_playback_follows_cursor, false));
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackLength(double* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->playback_get_length();
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackTime(double* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->playback_get_position();
	return S_OK;
}

STDMETHODIMP Fb::get_ProfilePath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(helpers::get_profile_path());
	return S_OK;
}

STDMETHODIMP Fb::get_ReplaygainMode(UINT* out)
{
	if (!out) return E_POINTER;

	t_replaygain_config rg;
	replaygain_manager::get()->get_core_settings(rg);
	*out = rg.m_source_mode;
	return S_OK;
}

STDMETHODIMP Fb::get_StopAfterCurrent(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->get_stop_after_current());
	return S_OK;
}

STDMETHODIMP Fb::get_Volume(float* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->get_volume();
	return S_OK;
}

STDMETHODIMP Fb::put_AlwaysOnTop(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_ui_always_on_top, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_CursorFollowPlayback(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_cursor_follows_playback, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_PlaybackFollowCursor(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_playback_follows_cursor, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_PlaybackTime(double time)
{
	playback_control::get()->playback_seek(time);
	return S_OK;
}

STDMETHODIMP Fb::put_ReplaygainMode(UINT idx)
{
	switch (idx)
	{
	case 0: standard_commands::main_rg_disable(); break;
	case 1: standard_commands::main_rg_set_track(); break;
	case 2: standard_commands::main_rg_set_album(); break;
	case 3: standard_commands::run_main(standard_commands::guid_main_rg_byorder); break;
	default: return E_INVALIDARG;
	}

	playback_control_v3::get()->restart();
	return S_OK;
}

STDMETHODIMP Fb::put_StopAfterCurrent(VARIANT_BOOL b)
{
	playback_control::get()->set_stop_after_current(to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_Volume(float value)
{
	playback_control::get()->set_volume(value);
	return S_OK;
}
