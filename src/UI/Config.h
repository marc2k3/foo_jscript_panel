#pragma once

class Config : public cfg_var
{
public:
	Config(const GUID& guid);

	struct KeyValue
	{
		std::string key, value;
	};

	using KeyValues = std::vector<KeyValue>;
	using StringMap = std::map<std::string, std::string>;

	void get_data_raw(stream_writer* writer, abort_callback& abort) override;
	void init_data();
	void load(jstring content);
	void merge_map(const StringMap& map);
	void save(jstring filename);
	void set_data_raw(stream_reader* reader, size_t, abort_callback& abort) override;

	KeyValues m_data;
	WINDOWPLACEMENT m_wndpl{};
};

extern Config g_config;
