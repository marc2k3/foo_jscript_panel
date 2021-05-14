#pragma once

class PlaylistLock : public playlist_lock
{
public:
	PlaylistLock(uint32_t flags) : m_flags(flags) {}

	bool execute_default_action(size_t) override
	{
		return false;
	}

	bool query_items_add(t_size, const pfc::list_base_const_t<metadb_handle_ptr>&, const bit_array&) override
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

	inline static std::map<uint64_t, playlist_lock::ptr> s_map;

private:
	uint32_t m_flags = 0;
};
