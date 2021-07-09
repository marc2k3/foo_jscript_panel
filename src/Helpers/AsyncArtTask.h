#pragma once

class AsyncArtTask : public SimpleThreadTask
{
public:
	AsyncArtTask(CWindow hwnd, const metadb_handle_ptr& handle, uint32_t id, bool need_stub, bool only_embed)
		: m_hwnd(hwnd)
		, m_handle(handle)
		, m_id(id)
		, m_need_stub(need_stub)
		, m_only_embed(only_embed) {}

	void run() override
	{
		IGdiBitmap* bitmap = nullptr;
		IMetadbHandle* handle = nullptr;
		string8 image_path;

		if (m_handle.is_valid())
		{
			album_art_data_ptr data;
			handle = new ComObjectImpl<MetadbHandle>(m_handle);

			if (m_only_embed)
			{
				const string8 path = m_handle->get_path();
				data = AlbumArt::get_embedded(path, m_id);
				if (data.is_valid())
				{
					filesystem::g_get_display_path(path, image_path);
				}
			}
			else
			{
				data = AlbumArt::get(m_handle, m_id, m_need_stub, image_path);
			}

			bitmap = AlbumArt::data_to_bitmap(data);
		}

		AsyncArtData data(handle, m_id, bitmap, to_bstr(image_path));
		m_hwnd.SendMessage(to_uint(CallbackID::on_get_album_art_done), reinterpret_cast<WPARAM>(&data));
	}

private:
	CWindow m_hwnd;
	bool m_need_stub = true;
	bool m_only_embed = false;
	metadb_handle_ptr m_handle;
	uint32_t m_id = 0;
};
