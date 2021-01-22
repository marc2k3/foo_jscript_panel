#include "stdafx.h"
#include "ScriptInfo.h"

ScriptInfo::ScriptInfo()
{
	if (s_replacements.empty())
	{
		s_replacements =
		{
			{ "%fb2k_profile_path%", Component::get_profile_path().get_ptr() },
			{ "%fb2k_component_path%", Component::get_path().get_ptr() },
		};
	}
}

std::string ScriptInfo::extract_value(const std::string& str)
{
	constexpr char q = '"';
	const size_t first = str.find_first_of(q);
	const size_t last = str.find_last_of(q);
	if (first < last && last < str.length())
	{
		return str.substr(first + 1, last - first - 1);
	}
	return "";
}

void ScriptInfo::add_import(const std::string& str)
{
	std::string path = extract_value(str);
	if (path.length())
	{
		for (const auto& [what, with] : s_replacements)
		{
			if (path.starts_with(what))
			{
				path = with + path.substr(what.length());
				break;
			}
		}
		m_imports.emplace_back(path);
	}
}

void ScriptInfo::clear()
{
	m_imports.clear();
	m_name.reset();
	m_build_string.reset();
}

void ScriptInfo::update(size_t id, jstring str)
{
	clear();
	m_name << "id:" << id;

	std::string source = str;
	string8 author, version;
	const size_t start = source.find("// ==PREPROCESSOR==");
	const size_t end = source.find("// ==/PREPROCESSOR==");

	if (start < end && end < source.length())
	{
		std::string pre = source.substr(start + 21, end - start - 21);

		for (const std::string& line : split_string(pre, CRLF))
		{
			const size_t len = line.length();
			if (line.find("@name") < len)
			{
				m_name = extract_value(line).c_str();
			}
			else if (line.find("@version") < len)
			{
				version = extract_value(line).c_str();
			}
			else if (line.find("@author") < len)
			{
				author = extract_value(line).c_str();
			}
			else if (line.find("@import") < len)
			{
				add_import(line);
			}
		}
	}

	m_build_string << jsp::component_name << " v" << jsp::component_version << " (" << m_name;
	if (version.get_length()) m_build_string << " v" << version;
	if (author.get_length()) m_build_string << " by " << author;
	m_build_string << ")";
}
