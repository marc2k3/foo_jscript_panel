#include "stdafx.h"
#include "CallbackData.h"
#include "PanelManager.h"
#include "PlaylistLock.h"

#include <Scintilla.h>

namespace
{
	struct WatchedObject
	{
		const GUID* guid;
		CallbackID id;
	};

	static const std::vector<WatchedObject> watched_objects =
	{
		{ &standard_config_objects::bool_playlist_stop_after_current, CallbackID::on_playlist_stop_after_current_changed },
		{ &standard_config_objects::bool_cursor_follows_playback, CallbackID::on_cursor_follow_playback_changed },
		{ &standard_config_objects::bool_playback_follows_cursor, CallbackID::on_playback_follow_cursor_changed },
		{ &standard_config_objects::bool_ui_always_on_top, CallbackID::on_always_on_top_changed }
	};

	class ConfigObjectNotify : public config_object_notify
	{
	public:
		GUID get_watched_object(size_t index) override
		{
			return *watched_objects[index].guid;
		}

		size_t get_watched_object_count() override
		{
			return watched_objects.size();
		}

		void on_watched_object_changed(const config_object::ptr& object) override
		{
			const GUID g = object->get_guid();
			const auto& it = std::ranges::find_if(watched_objects, [=](const WatchedObject& item) { return g == *item.guid; });
			if (it != watched_objects.end())
			{
				bool b;
				object->get_data_bool(b);
				PanelManager::instance().post_msg_to_all(it->id, b);
			}
		}
	};

	class DSPConfigCallback : public dsp_config_callback
	{
	public:
		void on_core_settings_change(const dsp_chain_config&) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_dsp_preset_changed);
		}
	};

	class InitQuit : public initquit, public ui_selection_callback, public replaygain_core_settings_notify, public output_config_change_callback, public now_playing_album_art_notify
	{
	public:
		void on_album_art(album_art_data_ptr) override
		{
			metadb_handle_ptr handle;
			if (playback_control::get()->get_now_playing(handle) && !std::string(handle->get_path()).starts_with("file://"))
			{
				PanelManager::instance().post_msg_to_all(CallbackID::on_playback_dynamic_info_track, 1);
			}
		}

		void on_changed(const t_replaygain_config& cfg) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_replaygain_mode_changed, cfg.m_source_mode);
		}

		void on_init() override
		{
			apply_playlist_locks();

			HINSTANCE ins = core_api::get_my_instance();
			Scintilla_RegisterClasses(ins);

			now_playing_album_art_notify_manager::get()->add(this);
			output_manager_v2::get()->addCallback(this);
			replaygain_manager_v2::get()->add_notify(this);
			ui_selection_manager_v2::get()->register_callback(this, 0);
		}

		void on_quit() override
		{
			Scintilla_ReleaseResources();

			now_playing_album_art_notify_manager::get()->remove(this);
			output_manager_v2::get()->removeCallback(this);
			replaygain_manager_v2::get()->remove_notify(this);
			ui_selection_manager_v2::get()->unregister_callback(this);

			PanelManager::instance().unload_all();
		}

		void on_selection_changed(metadb_handle_list_cref) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_selection_changed);
		}

		void outputConfigChanged() override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_output_device_changed);
		}

	private:
		void apply_playlist_locks()
		{
			auto api = playlist_manager_v2::get();
			const size_t count = api->get_playlist_count();

			for (size_t i = 0; i < count; ++i)
			{
				uint32_t flags = 0;
				if (api->playlist_get_property_int(i, guids::playlist_lock_flags, flags))
				{
					PlaylistLock::add(i, flags);
				}
			}
		}
	};

	class LibraryCallback : public library_callback
	{
	public:
		void on_items_added(metadb_handle_list_cref handles) override
		{
			auto data = new MetadbCallbackData(handles);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_library_items_added, data);
		}

		void on_items_modified(metadb_handle_list_cref handles) override
		{
			auto data = new MetadbCallbackData(handles);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_library_items_changed, data);
		}

		void on_items_removed(metadb_handle_list_cref handles) override
		{
			auto data = new MetadbCallbackData(handles);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_library_items_removed, data);
		}
	};

	class MainMenu : public mainmenu_commands
	{
	public:
		GUID get_command(uint32_t index) override
		{
			return guids::mainmenu_items[index];
		}

		GUID get_parent() override
		{
			return guids::mainmenu_group;
		}

		bool get_description(uint32_t index, pfc::string_base& out) override
		{
			out = PFC_string_formatter() << "Invoke on_main_menu(" << (index + 1) << ")";
			return true;
		}

		bool get_display(uint32_t index, pfc::string_base& out, uint32_t& flags) override
		{
			get_name(index, out);
			flags = mainmenu_commands::flag_defaulthidden;
			return true;
		}

		uint32_t get_command_count() override
		{
			return guids::mainmenu_items.size();
		}

		void execute(uint32_t index, service_ptr_t<service_base> callback) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_main_menu, index + 1);
		}

		void get_name(uint32_t index, pfc::string_base& out) override
		{
			out = std::to_string(index + 1).c_str();
		}
	};

	class MetadbIOCallback : public metadb_io_callback
	{
	public:
		void on_changed_sorted(metadb_handle_list_cref handles, bool) override
		{
			auto data = new MetadbCallbackData(handles);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_metadb_changed, data);
		}
	};

	class PlaybackQueueCallback : public playback_queue_callback
	{
	public:
		void on_changed(t_change_origin p_origin) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_queue_changed, p_origin);
		}
	};

	class PlayCallbackStatic : public play_callback_static
	{
	public:
		uint32_t get_flags() override
		{
			return flag_on_playback_all | flag_on_volume_change;
		}

		void on_playback_dynamic_info(const file_info&) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_dynamic_info);
		}

		void on_playback_dynamic_info_track(const file_info&) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_dynamic_info_track, 0);
		}

		void on_playback_edited(metadb_handle_ptr handle) override
		{
			auto data = new MetadbCallbackData(handle);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playback_edited, data);
		}

		void on_playback_new_track(metadb_handle_ptr handle) override
		{
			auto data = new MetadbCallbackData(handle);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playback_new_track, data);
		}

		void on_playback_pause(bool state) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_pause, state);
		}

		void on_playback_seek(double time) override
		{
			auto data = new CallbackData({ time });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playback_seek, data);
		}

		void on_playback_starting(playback_control::t_track_command cmd, bool paused) override
		{
			auto data = new CallbackData({ cmd, paused });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playback_starting, data);
		}

		void on_playback_stop(playback_control::t_stop_reason reason) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_stop, reason);
		}

		void on_playback_time(double time) override
		{
			auto data = new CallbackData({ time });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playback_time, data);
		}

		void on_volume_change(float new_val) override
		{
			auto data = new CallbackData({ new_val });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_volume_change, data);
		}
	};

	class PlaybackStatisticsCollector : public playback_statistics_collector
	{
	public:
		void on_item_played(metadb_handle_ptr handle) override
		{
			auto data = new MetadbCallbackData(handle);
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_item_played, data);
		}
	};

	class PlaylistCallbackStatic : public playlist_callback_static
	{
	public:
		void on_default_format_changed() override {}
		void on_items_modified(size_t, const pfc::bit_array&) override {}
		void on_items_modified_fromplayback(size_t, const pfc::bit_array&, playback_control::t_display_level) override {}
		void on_items_removing(size_t, const pfc::bit_array&, size_t, size_t) override {}
		void on_items_replaced(size_t, const pfc::bit_array&, const pfc::list_base_const_t<t_on_items_replaced_entry>&) override {}
		void on_playlists_removing(const pfc::bit_array&, size_t, size_t) override {}

		uint32_t get_flags() override
		{
			return flag_on_item_ensure_visible | flag_on_item_focus_change | flag_on_items_added | flag_on_items_removed | flag_on_items_reordered |
				flag_on_items_selection_change | flag_on_playback_order_changed | flag_on_playlist_activate | flag_on_playlist_created |
				flag_on_playlist_locked | flag_on_playlists_removed | flag_on_playlist_renamed | flag_on_playlists_reorder;
		}

		void on_item_ensure_visible(size_t playlist, size_t idx) override
		{
			auto data = new CallbackData({ playlist, idx });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playlist_item_ensure_visible, data);
		}

		void on_item_focus_change(size_t playlist, size_t from, size_t to) override
		{
			auto data = new CallbackData({ playlist, from, to });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_item_focus_change, data);
		}

		void on_items_added(size_t playlist, size_t, metadb_handle_list_cref, const pfc::bit_array&) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlist_items_added, playlist);
		}

		void on_items_removed(size_t playlist, const pfc::bit_array&, size_t, size_t new_count) override
		{
			auto data = new CallbackData({ playlist, new_count });
			PanelManager::instance().post_msg_to_all_pointer(CallbackID::on_playlist_items_removed, data);
		}

		void on_items_reordered(size_t playlist, const size_t*, size_t) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlist_items_reordered, playlist);
		}

		void on_items_selection_change(size_t, const pfc::bit_array&, const pfc::bit_array&) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlist_items_selection_change);
		}

		void on_playback_order_changed(size_t new_index) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playback_order_changed, new_index);
		}

		void on_playlist_activate(size_t old_index, size_t new_index) override
		{
			if (old_index != new_index)
			{
				PanelManager::instance().post_msg_to_all(CallbackID::on_playlist_switch);
			}
		}

		void on_playlist_created(size_t, const char*, size_t) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlists_changed);
		}

		void on_playlist_locked(size_t, bool) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlists_changed);
		}

		void on_playlist_renamed(size_t, const char*, size_t) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlists_changed);
		}

		void on_playlists_removed(const pfc::bit_array&, size_t, size_t) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlists_changed);
		}

		void on_playlists_reorder(const size_t*, size_t) override
		{
			PanelManager::instance().post_msg_to_all(CallbackID::on_playlists_changed);
		}
	};

	static mainmenu_group_popup_factory g_mainmenu_group(guids::mainmenu_group, mainmenu_groups::file, mainmenu_commands::sort_priority_base, jsp::component_name);

	FB2K_SERVICE_FACTORY(ConfigObjectNotify);
	FB2K_SERVICE_FACTORY(DSPConfigCallback);
	FB2K_SERVICE_FACTORY(InitQuit);
	FB2K_SERVICE_FACTORY(LibraryCallback);
	FB2K_SERVICE_FACTORY(MainMenu);
	FB2K_SERVICE_FACTORY(MetadbIOCallback);
	FB2K_SERVICE_FACTORY(PlaybackQueueCallback);
	FB2K_SERVICE_FACTORY(PlayCallbackStatic);
	FB2K_SERVICE_FACTORY(PlaybackStatisticsCollector);
	FB2K_SERVICE_FACTORY(PlaylistCallbackStatic);
}
