#pragma once

class Embed : public threaded_process_callback
{
public:
	enum class actions
	{
		attach,
		remove,
		remove_all
	};

	Embed(metadb_handle_list_cref handles, size_t id, const album_art_data_ptr& data) : m_handles(handles), m_id(id), m_data(data), m_action(actions::attach) {}
	Embed(metadb_handle_list_cref handles, size_t id) : m_handles(handles), m_id(id), m_action(actions::remove) {}
	Embed(metadb_handle_list_cref handles) : m_handles(handles), m_action(actions::remove_all) {}

	void run(threaded_process_status& status, abort_callback& abort) override
	{
		const GUID what = AlbumArt::id_to_guid(m_id);
		auto api = file_lock_manager::get();
		const size_t count = m_handles.get_count();

		for (size_t i = 0; i < count; ++i)
		{
			const string8 path = m_handles[i]->get_path();
			status.set_progress(i, count);
			status.set_item_path(path);
			album_art_editor::ptr ptr;
			if (album_art_editor::g_get_interface(ptr, path))
			{
				try
				{
					auto lock = api->acquire_write(path, abort);
					album_art_editor_instance_ptr aaep = ptr->open(nullptr, path, abort);
					switch (m_action)
					{
					case actions::attach:
						aaep->set(what, m_data, abort);
						break;
					case actions::remove:
						aaep->remove(what);
						break;
					case actions::remove_all:
						aaep->remove_all_();
						break;
					}
					aaep->commit(abort);
				}
				catch (...) {}
			}
		}
	}

private:
	actions m_action;
	album_art_data_ptr m_data;
	metadb_handle_list m_handles;
	size_t m_id = 0;
};
