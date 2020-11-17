#pragma once

class FileInfoFilter : public file_info_filter
{
public:
	struct Tag
	{
		std::string name;
		Strings values;
	};

	using Tags = std::vector<Tag>;

	FileInfoFilter(const Tags& tags) : m_tags(tags) {}

	bool apply_filter(metadb_handle_ptr location, t_filestats stats, file_info& info) override
	{
		for (const auto& [name, values] : m_tags)
		{
			info.meta_remove_field(name.c_str());
			for (const std::string& value : values)
			{
				info.meta_add(name.c_str(), value.c_str());
			}
		}
		return true;
	}

	static Strings g_get_values(json j)
	{
		Strings values;

		auto add_value = [&](json value)
		{
			std::string str = value.is_string() ? value.get_ref<const std::string&>() : value.dump();
			if (str.length()) values.emplace_back(str);
		};

		if (j.is_array())
		{
			for (auto& v : j)
			{
				add_value(v);
			}
		}
		else
		{
			add_value(j);
		}
		return values;
	}

private:
	Tags m_tags;
};
