#pragma once

namespace helpers
{
	namespace fs = std::filesystem;

	template <typename T = fs::directory_iterator>
	static Strings list_t(const std::string& path, bool want_files = true)
	{
		Strings strings;
		const auto u8path = fs::u8path(path);
		if (fs::is_directory(u8path))
		{
			for (const auto& p : T(u8path, fs::directory_options::skip_permission_denied))
			{
				if ((want_files && p.is_regular_file()) || (!want_files && p.is_directory())) strings.emplace_back(p.path().u8string());
			}
		}
		return strings;
	}

	static Strings list_files(const std::string& path, bool recur = false)
	{
		if (recur) return list_t<fs::recursive_directory_iterator>(path);
		return list_t(path);
	}

	static Strings list_folders(const std::string& path, bool recur = false)
	{
		if (recur) return list_t<fs::recursive_directory_iterator>(path, false);
		return list_t(path, false);
	}

	static bool write_file(stringp path, stringp content)
	{
		std::ofstream f(fs::u8path(path.get_ptr()), std::ios::binary);
		if (f.is_open())
		{
			f << content;
			f.close();
			return true;
		}
		return false;
	}

	static string8 read_file(stringp path)
	{
		string8 content;
		std::ifstream f(fs::u8path(path.get_ptr()));
		if (f.is_open())
		{
			std::string line;
			while (std::getline(f, line))
			{
				content << line.c_str() << CRLF;
			}
			f.close();
		}
		return content;
	}

	static void read_file_wide(const std::wstring& path, size_t codepage, std::wstring& content)
	{
		CloseHandleScope hFile = CreateFile(path.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile.Get() == INVALID_HANDLE_VALUE) return;

		const DWORD dwFileSize = GetFileSize(hFile.Get(), nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) return;

		CloseHandleScope hFileMapping = CreateFileMapping(hFile.Get(), nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (hFileMapping.Get() == nullptr) return;

		LPCBYTE pAddr = static_cast<LPCBYTE>(MapViewOfFile(hFileMapping.Get(), FILE_MAP_READ, 0, 0, 0));
		if (pAddr == nullptr) return;

		if (dwFileSize >= 2 && pAddr[0] == 0xFF && pAddr[1] == 0xFE) // UTF16 LE?
		{
			const wchar_t* pSource = reinterpret_cast<const wchar_t*>(pAddr + 2);
			const size_t len = (dwFileSize - 2) >> 1;

			content.resize(len);
			pfc::__unsafe__memcpy_t(content.data(), pSource, len);
		}
		else if (dwFileSize >= 3 && pAddr[0] == 0xEF && pAddr[1] == 0xBB && pAddr[2] == 0xBF) // UTF8-BOM?
		{
			string8 str(reinterpret_cast<const char*>(pAddr + 3), dwFileSize - 3);
			content = to_wide(str);
		}
		else
		{
			string8 str(reinterpret_cast<const char*>(pAddr), dwFileSize);

			if (codepage == CP_UTF8 || guess_codepage(str) == CP_UTF8)
			{
				content = to_wide(str);
			}
			else
			{
				content.resize(estimate_codepage_to_wide(codepage, str, dwFileSize));
				convert_codepage_to_wide(codepage, content.data(), content.size(), str, dwFileSize);
			}
		}

		UnmapViewOfFile(pAddr);
	}
}
