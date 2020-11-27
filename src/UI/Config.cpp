#include "stdafx.h"
#include "Config.h"

static const std::vector<Config::SimpleKeyVal> init_table =
{
	{"style.default", "font:Consolas,size:11"},
	{"style.comment", "fore:#008000"},
	{"style.keyword", "bold,fore:#0000FF"},
	{"style.identifier", ""},
	{"style.string", "fore:#FF0000"},
	{"style.number", "fore:#FF0000"},
	{"style.operator", ""},
	{"style.linenumber", "font:Consolas,size:9,fore:#2B91AF"},
	{"style.bracelight", "bold,fore:#000000,back:#FFEE62"},
	{"style.bracebad", "bold,fore:#FF0000"},
	{"style.selection.fore", ""},
	{"style.selection.back", ""},
	{"style.selection.alpha", "256"},
	{"style.caret.fore", ""},
	{"style.caret.width", "1"},
	{"style.caret.line.back", ""},
	{"style.caret.line.back.alpha", "256"}
};

Config g_config(guids::config);

Config::Config(const GUID& guid) : cfg_var(guid)
{
	init_data();
}

void Config::get_data_raw(stream_writer* writer, abort_callback& abort)
{
	try
	{
		writer->write_lendian_t(m_data.size(), abort);
		for (const auto& [key, value] : m_data)
		{
			writer->write_string(key.c_str(), abort);
			writer->write_string(value.c_str(), abort);
		}
		writer->write_object(&m_conf_wndpl, sizeof(WINDOWPLACEMENT), abort);
		writer->write_object(&m_property_wndpl, sizeof(WINDOWPLACEMENT), abort);
	}
	catch (...) {}
}

void Config::import(stringp content)
{
	SimpleMap data_map;
	for (const std::string& line : helpers::split_string(content.get_ptr(), CRLF))
	{
		const size_t pos = line.find('=');
		if (pos == 0 || pos == std::string::npos) continue;
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		data_map[key] = value;
	}
	merge_data(data_map);
}

void Config::init_data()
{
	m_data = init_table;
}

void Config::load_preset(int idx)
{
	import(helpers::get_resource_text(idx));
}

void Config::merge_data(const SimpleMap& data_map)
{
	for (auto& [key, value] : m_data)
	{
		if (data_map.count(key))
		{
			value = data_map.at(key);
		}
	}
}

void Config::set_data_raw(stream_reader* reader, size_t sizehint, abort_callback& abort)
{
	SimpleMap data_map;
	size_t count;
	string8 key, value;

	try
	{
		reader->read_lendian_t(count, abort);

		for (size_t i = 0; i < count; ++i)
		{
			reader->read_string(key, abort);
			reader->read_string(value, abort);
			data_map.emplace(key.get_ptr(), value.get_ptr());
		}

		reader->read_object(&m_conf_wndpl, sizeof(WINDOWPLACEMENT), abort);
		reader->read_object(&m_property_wndpl, sizeof(WINDOWPLACEMENT), abort);
	}
	catch (...)
	{
		init_data();
		return;
	}
	merge_data(data_map);
}
