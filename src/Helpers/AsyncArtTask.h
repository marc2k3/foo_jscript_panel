#pragma once

class AsyncArtTask : public SimpleThreadTask
{
public:
	AsyncArtTask(CWindow hwnd, const metadb_handle_ptr& handle, size_t art_id, bool need_stub, bool only_embed, bool no_load)
		: m_hwnd(hwnd)
		, m_handle(handle)
		, m_art_id(art_id)
		, m_need_stub(need_stub)
		, m_only_embed(only_embed)
		, m_no_load(no_load) {}

	void run() override
	{
		IGdiBitmap* bitmap = nullptr;
		IMetadbHandle* handle = nullptr;
		string8 image_path;

		if (m_handle.is_valid())
		{
			handle = new ComObjectImpl<MetadbHandle>(m_handle);

			if (m_only_embed)
			{
				const auto path = m_handle->get_path();
				bitmap = helpers::get_album_art_embedded(path, m_art_id);
				if (bitmap)
				{
					image_path = file_path_display(path);
				}
			}
			else
			{
				bitmap = helpers::get_album_art(m_handle, m_art_id, m_need_stub, m_no_load, image_path);
			}
		}

		AsyncArtData data(handle, m_art_id, bitmap, to_bstr(image_path));
		m_hwnd.SendMessage(to_uint(CallbackID::on_get_album_art_done), reinterpret_cast<WPARAM>(&data));
	}

private:
	CWindow m_hwnd;
	bool m_need_stub = true;
	bool m_no_load = false;
	bool m_only_embed = false;
	metadb_handle_ptr m_handle;
	size_t m_art_id = 0;
};