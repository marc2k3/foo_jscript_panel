#pragma once

class ScriptInfo
{
public:
	ScriptInfo();

	void update(uint32_t id, jstring str);

	WStrings m_imports;
	string8 m_name, m_build_string;

private:
	struct Replacement
	{
		std::wstring what, with;
	};

	std::string extract_value(const std::string& str);
	void add_import(const std::string& str);
	void clear();

	inline static std::vector<Replacement> s_replacements;
};
