#pragma once

class PlaylistLock : public playlist_lock
{
public:
	PlaylistLock(uint32_t flags) : m_flags(flags) {}

	bool execute_default_action(size_t) override
	{
		return false;
	}

	bool query_items_add(size_t, const pfc::list_base_const_t<metadb_handle_ptr>&, const bit_array&) override
	{
		return !(m_flags & filter_add);
	}

	bool query_items_remove(const bit_array&, bool) override
	{
		return !(m_flags & filter_remove);
	}

	bool query_items_reorder(const size_t*, size_t) override
	{
		return !(m_flags & filter_reorder);
	}

	bool query_item_replace(size_t, const metadb_handle_ptr&, const metadb_handle_ptr&) override
	{
		return !(m_flags & filter_replace);
	}

	bool query_playlist_remove() override
	{
		return !(m_flags & filter_remove_playlist);
	}

	bool query_playlist_rename(const char*, size_t) override
	{
		return !(m_flags & filter_rename);
	}

	uint32_t get_filter_mask() override
	{
		return m_flags;
	}

	void get_lock_name(pfc::string_base& out) override
	{
		out = jsp::component_name;
	}

	void on_playlist_index_change(size_t) override {}
	void on_playlist_remove() override {}
	void show_ui() override {}

	static bool add(size_t playlistIndex, uint32_t flags)
	{
		if (flags > 0)
		{
			auto api = playlist_manager_v2::get();

			if (!api->playlist_lock_is_present(playlistIndex))
			{
				auto lock = fb2k::service_new<PlaylistLock>(flags);

				if (api->playlist_lock_install(playlistIndex, lock))
				{
					GUID g;
					CoCreateGuid(&g);
					uint64_t hash = hasher_md5::get()->process_single_string(pfc::print_guid(g).get_ptr()).xorHalve();

					PlaylistLock::s_map.emplace(hash, lock);
					api->playlist_set_property_int(playlistIndex, guids::playlist_lock_hash, hash);
					api->playlist_set_property_int(playlistIndex, guids::playlist_lock_flags, flags);
					return true;
				}
			}
		}
		return false;
	}

	static bool remove(size_t playlistIndex)
	{
		auto api = playlist_manager_v2::get();

		string8 name;
		api->playlist_lock_query_name(playlistIndex, name);

		if (name.equals(jsp::component_name))
		{
			uint64_t hash;
			if (api->playlist_get_property_int(playlistIndex, guids::playlist_lock_hash, hash) && s_map.contains(hash))
			{
				auto lock = s_map.at(hash);
				if (api->playlist_lock_uninstall(playlistIndex, lock))
				{
					s_map.erase(hash);
					api->playlist_remove_property(playlistIndex, guids::playlist_lock_flags);
					api->playlist_remove_property(playlistIndex, guids::playlist_lock_hash);
					return true;
				}
			}
		}
		return false;
	}

	inline static std::map<uint64_t, playlist_lock::ptr> s_map;

private:
	uint32_t m_flags = 0;
};
