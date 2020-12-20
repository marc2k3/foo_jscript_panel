#pragma once

class OptimiseLayout : public threaded_process_callback
{
public:
	OptimiseLayout(metadb_handle_list_cref handles, bool minimise) : m_handles(handles), m_minimise(minimise) {}

	void run(threaded_process_status& status, abort_callback& abort) override
	{
		auto api = file_lock_manager::get();
		const size_t count = m_handles.get_count();

		for (size_t i = 0; i < count; ++i)
		{
			abort.check();

			const string8 path = m_handles[i]->get_path();
			status.set_progress(i, count);
			status.set_item_path(path);

			try
			{
				auto lock = api->acquire_write(path, abort);
				for (auto e = service_enum_t<file_format_sanitizer>(); !e.finished(); ++e)
				{
					if (e.get()->sanitize_file(path, m_minimise, abort)) break;
				}
			}
			catch (...) {}
		}
	}

private:
	bool m_minimise;
	metadb_handle_list m_handles;
};
