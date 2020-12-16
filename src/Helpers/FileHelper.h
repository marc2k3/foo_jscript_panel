#pragma once
#include <MLang.h>
_COM_SMARTPTR_TYPEDEF(IMultiLanguage2, IID_IMultiLanguage2);

class FileHelper
{
public:
	FileHelper(const string8& path) : m_path(std::filesystem::u8path(path.get_ptr())) {}
	FileHelper(const std::string& path) : m_path(std::filesystem::u8path(path)) {}
	FileHelper(const std::wstring& wpath) : m_path(wpath), m_wpath(wpath) {}

	WStrings list_files(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>();
		return list_t();
	}

	WStrings list_folders(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>(false);
		return list_t(false);
	}

	bool write(stringp content)
	{
		std::ofstream f(m_path, std::ios::binary);
		if (f.is_open())
		{
			f << content;
			f.close();
			return true;
		}
		return false;
	}

	size_t guess_codepage()
	{
		return guess_codepage(read());
	}

	string8 read()
	{
		string8 content;
		std::ifstream f(m_path);
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

	void read_wide(size_t codepage, std::wstring& content)
	{
		CloseHandleScope hFile = CreateFile(m_wpath.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
				content.resize(pfc::stringcvt::estimate_codepage_to_wide(codepage, str, dwFileSize));
				pfc::stringcvt::convert_codepage_to_wide(codepage, content.data(), content.size(), str, dwFileSize);
			}
		}

		UnmapViewOfFile(pAddr);
	}

private:
	template <typename T = std::filesystem::directory_iterator>
	WStrings list_t(bool want_files = true)
	{
		WStrings wstrings;
		if (std::filesystem::is_directory(m_path))
		{
			for (const auto& p : T(m_path, std::filesystem::directory_options::skip_permission_denied))
			{
				if ((want_files && p.is_regular_file()) || (!want_files && p.is_directory())) wstrings.emplace_back(p.path().wstring());
			}
		}
		return wstrings;
	}

	size_t guess_codepage(stringp content)
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

	std::filesystem::path m_path;
	std::wstring m_wpath;
};
