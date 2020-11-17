#pragma once
#include <MLang.h>
_COM_SMARTPTR_TYPEDEF(IMultiLanguage2, IID_IMultiLanguage2);

namespace helpers
{
	template <class T>
	static std::vector<T> split_string_t(const T& text, const T& delims)
	{
		std::vector<T> out;
		size_t pos = 0;
		while (true)
		{
			size_t old_pos = pos;
			pos = text.find(delims, old_pos);
			out.emplace_back(text.substr(old_pos, pos - old_pos));
			if (pos == T::npos) break;
			pos += delims.length();
		}
		return out;
	}

	static std::vector<std::string> split_string(const std::string& text, const std::string& delims) { return split_string_t<std::string>(text, delims); }
	static std::vector<std::wstring> split_string(const std::wstring& text, const std::wstring& delims) { return split_string_t<std::wstring>(text, delims); }

	static string8 get_fb2k_component_path()
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

	static size_t guess_codepage(stringp content)
	{
		int size = static_cast<int>(content.length());
		if (size == 0) return 0;

		constexpr int maxEncodings = 1;
		int encodingCount = maxEncodings;
		std::array<DetectEncodingInfo, maxEncodings> encodings;

		IMultiLanguage2Ptr lang;
		if (FAILED(lang.CreateInstance(CLSID_CMultiLanguage, nullptr, CLSCTX_INPROC_SERVER))) return 0;
		if (FAILED(lang->DetectInputCodepage(MLDETECTCP_NONE, 0, const_cast<char*>(content.get_ptr()), &size, encodings.data(), &encodingCount))) return 0;

		const size_t codepage = encodings[0].nCodePage;
		if (codepage == 20127) return CP_UTF8;
		return codepage;
	}
}
