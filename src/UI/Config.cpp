#include "stdafx.h"
#include "Config.h"

static const Config::KeyValues init_table =
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
	{"style.caret.fore", ""},
	{"style.selection.back", "#C0C0C0"},
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
		writer->write_object_t(m_data.size(), abort);
		for (const auto& [key, value] : m_data)
		{
			writer->write_string(key.c_str(), abort);
			writer->write_string(value.c_str(), abort);
		}
		writer->write_object(&m_wndpl, sizeof(WINDOWPLACEMENT), abort);
	}
	catch (...) {}
}

void Config::init_data()
{
	m_data = init_table;
}

void Config::load(jstring content)
{
	StringMap map;
	for (const std::string& line : split_string(content, CRLF))
	{
		Strings tmp = split_string(line, "=");
		if (tmp.size() == 2)
		{
			map.emplace(tmp[0], tmp[1]);
		}
	}
	merge_map(map);
}

void Config::merge_map(const StringMap& map)
{
	for (auto& [key, value] : m_data)
	{
		const auto& it = map.find(key);
		if (it != map.end())
		{
			value = it->second;
		}
	}
}

void Config::save(jstring filename)
{
	std::string content;
	for (const auto& [key, value] : m_data)
	{
		content += key + "=" + value + CRLF;
	}
	FileHelper(filename).write(content);
}

void Config::set_data_raw(stream_reader* reader, size_t sizehint, abort_callback& abort)
{
	try
	{
		StringMap map;
		size_t count;
		string8 key, value;

		reader->read_object_t(count, abort);

		for (size_t i = 0; i < count; ++i)
		{
			reader->read_string(key, abort);
			reader->read_string(value, abort);
			map.emplace(key.get_ptr(), value.get_ptr());
		}

		reader->read_object(&m_wndpl, sizeof(WINDOWPLACEMENT), abort);
		merge_map(map);
	}
	catch (...)
	{
		init_data();
	}
}
