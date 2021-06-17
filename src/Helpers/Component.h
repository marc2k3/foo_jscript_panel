#pragma once

namespace Component
{
	static std::string get_resource_text(int id)
	{
		puResource pures = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return std::string(static_cast<const char*>(pures->GetPointer()), pures->GetSize());
	}

	static std::wstring get_path()
	{
		PathArray path;
		GetModuleFileName(core_api::get_my_instance(), path.data(), path.size());
		return std::filesystem::path(path.data()).parent_path().wstring() + std::filesystem::path::preferred_separator;
	}

	static std::wstring get_fb2k_path()
	{
		PathArray path;
		GetModuleFileName(nullptr, path.data(), path.size());
		return std::filesystem::path(path.data()).parent_path().wstring() + std::filesystem::path::preferred_separator;
	}

	static std::wstring get_profile_path()
	{
		string8 path;
		filesystem::g_get_display_path(core_api::get_profile_path(), path);
		return to_wide(path) + std::filesystem::path::preferred_separator;
	}
}
