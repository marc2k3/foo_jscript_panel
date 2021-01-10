#pragma once

class ProcessLocationsNotify : public process_locations_notify
{
public:
	ProcessLocationsNotify(size_t playlist, size_t base, bool to_select) : m_playlist(playlist), m_base(base), m_to_select(to_select) {}

	void on_aborted() override {}

	void on_completion(metadb_handle_list_cref handles) override
	{
		auto api = playlist_manager::get();
		auto mask = api->playlist_lock_get_filter_mask(m_playlist);

		if (m_playlist < api->get_playlist_count() && !(mask & playlist_lock::filter_add))
		{
			api->playlist_insert_items(m_playlist, m_base, handles, pfc::bit_array_val(m_to_select));
			if (m_to_select)
			{
				api->set_active_playlist(m_playlist);
				api->playlist_set_focus_item(m_playlist, m_base);
			}
		}
	}

private:
	bool m_to_select = true;
	size_t m_base = 0;
	size_t m_playlist = 0;
};
