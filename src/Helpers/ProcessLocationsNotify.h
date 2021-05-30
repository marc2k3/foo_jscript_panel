#pragma once

class ProcessLocationsNotify : public process_locations_notify
{
public:
	enum class mode
	{
		callback,
		playlist,
	};

	ProcessLocationsNotify(CWindow hwnd, int cookie) : m_hwnd(hwnd), m_cookie(cookie), m_mode(mode::callback) {}
	ProcessLocationsNotify(size_t playlist, size_t base, bool to_select) : m_playlist(playlist), m_base(base), m_to_select(to_select), m_mode(mode::playlist) {}

	void on_aborted() override {}

	void on_completion(metadb_handle_list_cref handles) override
	{
		if (m_mode == mode::callback)
		{
			if (m_hwnd.IsWindow())
			{
				auto data = new MetadbCallbackData(handles);
				data->refcount_add_ref();
				m_hwnd.PostMessage(to_uint(CallbackID::on_locations_added), reinterpret_cast<WPARAM>(data), m_cookie);
			}
		}
		else if (m_mode == mode::playlist)
		{
			auto api = playlist_manager::get();
			const uint32_t mask = api->playlist_lock_get_filter_mask(m_playlist);

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
	}

private:
	CWindow m_hwnd;
	bool m_to_select = true;
	int m_cookie = 0;
	mode m_mode = mode::callback;
	size_t m_base = 0, m_playlist = 0;
};
