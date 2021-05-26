#include "stdafx.h"
#include "db.h"

namespace db
{
	static constexpr std::array field_names =
	{
		"jsp_playcount",
		"jsp_loved",
		"jsp_first_played",
		"jsp_last_played",
		"jsp_rating"
	};

	static constexpr const char* g_pin_to = "$lower(%artist% - %title%)";

	class MetadbIndexClient : public metadb_index_client
	{
	public:
		metadb_index_hash transform(const file_info& info, const playable_location& location) override
		{
			if (m_obj.is_empty())
			{
				titleformat_compiler::get()->compile_force(m_obj, g_pin_to);
			}

			string8 str;
			m_obj->run_simple(location, &info, str);
			return hasher_md5::get()->process_single_string(str).xorHalve();
		}

	private:
		titleformat_object::ptr m_obj;
	};

	static auto g_client = new service_impl_single_t<MetadbIndexClient>();
	static metadb_index_manager::ptr g_cachedAPI;

	class InitStageCallback : public init_stage_callback
	{
	public:
		void on_init_stage(uint32_t stage) override
		{
			if (stage == init_stages::before_config_read)
			{
				g_cachedAPI = metadb_index_manager::get();
				try
				{
					g_cachedAPI->add(g_client, guids::metadb_index, system_time_periods::week * 4);
					g_cachedAPI->dispatch_global_refresh();
				}
				catch (const std::exception& e)
				{
					g_cachedAPI->remove(guids::metadb_index);
					FB2K_console_formatter() << jsp::component_name << " stats: Critical initialisation failure: " << e;
				}
			}
		}
	};

	class InitQuitDB : public initquit
	{
	public:
		void on_quit() override
		{
			g_cachedAPI.release();
		}
	};

	class MetadbDisplayFieldProvider : public metadb_display_field_provider
	{
	public:
		bool process_field(uint32_t index, metadb_handle* handle, titleformat_text_out* out) override
		{
			metadb_index_hash hash;
			if (!hashHandle(handle, hash)) return false;
			const Fields f = get(hash);

			switch (index)
			{
			case 0:
				if (f.playcount == 0) return false;
				out->write_int(titleformat_inputtypes::meta, f.playcount);
				return true;
			case 1:
				if (f.loved == 0) return false;
				out->write_int(titleformat_inputtypes::meta, f.loved);
				return true;
			case 2:
				if (f.first_played.is_empty()) return false;
				out->write(titleformat_inputtypes::meta, f.first_played);
				return true;
			case 3:
				if (f.last_played.is_empty()) return false;
				out->write(titleformat_inputtypes::meta, f.last_played);
				return true;
			case 4:
				if (f.rating == 0) return false;
				out->write_int(titleformat_inputtypes::meta, f.rating);
				return true;
			}
			return false;
		}

		uint32_t get_field_count() override
		{
			return field_names.size();
		}

		void get_field_name(uint32_t index, pfc::string_base& out) override
		{
			out.set_string(field_names[index]);
		}
	};

	class TrackPropertyProviderV4 : public track_property_provider_v4
	{
	public:
		bool is_our_tech_info(const char* p_name) override
		{
			return false;
		}

		void enumerate_properties_helper(metadb_handle_list_cref handles, track_property_provider_v3_info_source& info, track_property_callback_v2& callback, abort_callback& abort)
		{
			if (callback.is_group_wanted(jsp::component_name))
			{
				const size_t count = handles.get_count();
				if (count == 1)
				{
					metadb_index_hash hash;
					if (hashHandle(handles[0], hash))
					{
						const Fields f = get(hash);
						if (f.playcount > 0) callback.set_property(jsp::component_name, 0, "Playcount", std::to_string(f.playcount).c_str());
						if (f.loved > 0) callback.set_property(jsp::component_name, 1, "Loved", std::to_string(f.loved).c_str());
						if (!f.first_played.is_empty()) callback.set_property(jsp::component_name, 2, "First Played", f.first_played);
						if (!f.last_played.is_empty()) callback.set_property(jsp::component_name, 3, "Last Played", f.last_played);
						if (f.rating > 0) callback.set_property(jsp::component_name, 4, "Rating", std::to_string(f.rating).c_str());
					}
				}
				else
				{
					hash_set hashes;
					get_hashes(handles, hashes);

					size_t total = std::accumulate(hashes.begin(), hashes.end(), 0U, [](size_t t, const metadb_index_hash hash)
						{
							return t + get(hash).playcount;
						});

					if (total > 0)
					{
						callback.set_property(jsp::component_name, 0, "Playcount", std::to_string(total).c_str());
					}
				}
			}
		}

		void enumerate_properties_v4(metadb_handle_list_cref handles, track_property_provider_v3_info_source& info, track_property_callback_v2& callback, abort_callback& abort) override
		{
			enumerate_properties_helper(handles, info, callback, abort);
		}
	};

	FB2K_SERVICE_FACTORY(InitStageCallback);
	FB2K_SERVICE_FACTORY(InitQuitDB);
	FB2K_SERVICE_FACTORY(MetadbDisplayFieldProvider);
	FB2K_SERVICE_FACTORY(TrackPropertyProviderV4);

	Fields get(metadb_index_hash hash)
	{
		mem_block_container_impl temp;
		theAPI()->get_user_data(guids::metadb_index, hash, temp);
		if (temp.get_size() > 0)
		{
			try
			{
				stream_reader_formatter_simple_ref<false> reader(temp.get_ptr(), temp.get_size());
				Fields f;
				reader >> f.playcount;
				reader >> f.loved;
				reader >> f.first_played;
				reader >> f.last_played;
				reader >> f.rating;
				return f;
			}
			catch (exception_io_data) {}
		}
		return Fields();
	}

	bool hashHandle(const metadb_handle_ptr& handle, metadb_index_hash& hash)
	{
		return g_client->hashHandle(handle, hash);
	}

	metadb_index_manager::ptr theAPI()
	{
		if (g_cachedAPI.is_valid()) return g_cachedAPI;
		return metadb_index_manager::get();
	}

	void get_hashes(metadb_handle_list_cref handles, hash_set& hashes)
	{
		const size_t count = handles.get_count();
		for (size_t i = 0; i < count; ++i)
		{
			metadb_index_hash hash;
			if (hashHandle(handles[i], hash))
			{
				hashes.emplace(hash);
			}
		}
	}

	void refresh(metadb_handle_list_cref handles)
	{
		hash_set hashes;
		get_hashes(handles, hashes);

		hash_list to_refresh;
		for (const auto& hash : hashes)
		{
			to_refresh += hash;
		}
		theAPI()->dispatch_refresh(guids::metadb_index, to_refresh);
	}

	void set(metadb_index_hash hash, Fields f)
	{
		stream_writer_formatter_simple<false> writer;
		writer << f.playcount;
		writer << f.loved;
		writer << f.first_played;
		writer << f.last_played;
		writer << f.rating;
		theAPI()->set_user_data(guids::metadb_index, hash, writer.m_buffer.get_ptr(), writer.m_buffer.get_size());
	}
}
