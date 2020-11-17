#pragma once

class Config : public cfg_var
{
public:
	Config(const GUID& guid);

	struct SimpleKeyVal
	{
		pfc::string_simple key, value;
	};

	using SimpleMap = std::map<pfc::string_simple, pfc::string_simple, StricmpAscii>;

	void get_data_raw(stream_writer* writer, abort_callback& abort) override;
	void import(stringp content);
	void init_data();
	void load_preset(int idx);
	void merge_data(const SimpleMap& data_map);
	void set_data_raw(stream_reader* reader, size_t sizehint, abort_callback& abort) override;

	WINDOWPLACEMENT m_conf_wndpl{}, m_property_wndpl{};
	std::vector<SimpleKeyVal> m_data;
};

extern Config g_config;
