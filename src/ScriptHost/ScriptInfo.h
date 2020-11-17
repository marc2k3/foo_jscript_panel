#pragma once

class ScriptInfo
{
public:
	ScriptInfo();

	void update(size_t id, stringp code);

	Strings m_imports;
	string8 m_name, m_build_string;

private:
	struct Replacement
	{
		std::string what, with;
	};

	std::string expand_import(const std::string& path);
	std::string extract_value(const std::string& source);
	void clear();

	const std::vector<Replacement> m_replacements =
	{
		{ "%fb2k_profile_path%", helpers::get_profile_path().get_ptr() },
		{ "%fb2k_component_path%", helpers::get_fb2k_component_path().get_ptr() },
	};
};
