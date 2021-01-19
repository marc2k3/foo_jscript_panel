#pragma once

namespace Component
{
	static std::string get_resource_text(int id)
	{
		puResource pures = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return std::string(static_cast<const char*>(pures->GetPointer()), pures->GetSize());
	}

	static string8 get_path()
	{
		string8 path;
		uGetModuleFileName(core_api::get_my_instance(), path);
		path = pfc::string_directory(path);
		path.add_char('\\');
		return path;
	}

	static string8 get_fb2k_path()
	{
		string8 path;
		uGetModuleFileName(nullptr, path);
		path = pfc::string_directory(path);
		path.add_char('\\');
		return path;
	}

	static string8 get_profile_path()
	{
		string8 path;
		filesystem::g_get_display_path(core_api::get_profile_path(), path);
		path.add_char('\\');
		return path;
	}
}
