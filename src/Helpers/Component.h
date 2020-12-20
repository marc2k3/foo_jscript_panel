#pragma once

namespace Component
{
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
		string8 path = file_path_display(core_api::get_profile_path()).get_ptr();
		path.add_char('\\');
		return path;
	}

	static string8 get_resource_text(int id)
	{
		puResource pures = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		string8 content(static_cast<const char*>(pures->GetPointer()), pures->GetSize());
		return content;
	}
}
