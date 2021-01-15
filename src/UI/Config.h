#pragma once

class Config : public cfg_var
{
public:
	Config(const GUID& guid);

	struct SimpleKeyVal
	{
		std::string key, value;
	};

	using SimpleMap = std::map<std::string, std::string>;

	void get_data_raw(stream_writer* writer, abort_callback& abort) override;
	void init_data();
	void load(stringp content);
	void load_preset(int idx);
	void merge_data(const SimpleMap& data);
	void set_data_raw(stream_reader* reader, size_t sizehint, abort_callback& abort) override;

	WINDOWPLACEMENT m_wndpl{};
	std::vector<SimpleKeyVal> m_data;
};

extern Config g_config;
