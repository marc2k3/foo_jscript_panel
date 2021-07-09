#pragma once
#include <MLang.h>

class FileHelper
{
public:
	FileHelper(const std::wstring& path) : m_path(path) {}
	FileHelper(jstring path) : m_path(to_wide(path)) {}

	WStrings list_files(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>(Entry::is_file);
		return list_t(Entry::is_file);
	}

	WStrings list_folders(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>(Entry::is_folder);
		return list_t(Entry::is_folder);
	}

	bool is_file()
	{
		std::error_code ec;
		return std::filesystem::is_regular_file(m_path, ec);
	}

	bool is_folder()
	{
		std::error_code ec;
		return std::filesystem::is_directory(m_path, ec);
	}

	bool write(jstring content)
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

	string8 read()
	{
		std::string content;
		std::ifstream f(m_path);
		if (f.is_open())
		{
			std::string line;
			while (std::getline(f, line))
			{
				content += line + CRLF;
			}
			f.close();
		}
		if (content.length() >= 3 && strncmp(content.c_str(), UTF_8_BOM, 3) == 0)
		{
			return content.substr(3).c_str();
		}
		return content.c_str();
	}

	uint32_t guess_codepage()
	{
		return guess_codepage(read());
	}

	void read_wide(uint32_t codepage, std::wstring& content)
	{
		CloseHandleScope hFile = CreateFile(m_path.wstring().data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile.Get() == INVALID_HANDLE_VALUE) return;

		const DWORD dwFileSize = GetFileSize(hFile.Get(), nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) return;

		CloseHandleScope hFileMapping = CreateFileMapping(hFile.Get(), nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (hFileMapping.Get() == nullptr) return;

		LPCBYTE pAddr = static_cast<LPCBYTE>(MapViewOfFile(hFileMapping.Get(), FILE_MAP_READ, 0, 0, 0));
		if (pAddr == nullptr) return;

		if (dwFileSize >= 2 && memcmp(pAddr, UTF_16_LE_BOM, 2) == 0)
		{
			const wchar_t* pSource = reinterpret_cast<const wchar_t*>(pAddr + 2);
			const uint32_t len = (dwFileSize - 2) >> 1;

			content.resize(len);
			pfc::__unsafe__memcpy_t(content.data(), pSource, len);
		}
		else if (dwFileSize >= 3 && memcmp(pAddr, UTF_8_BOM, 3) == 0)
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
	struct Entry
	{
		static bool is_file(const std::filesystem::directory_entry& entry) { return entry.is_regular_file(); }
		static bool is_folder(const std::filesystem::directory_entry& entry) { return entry.is_directory(); }
	};

	using CheckEntry = std::function<bool(const std::filesystem::directory_entry&)>;

	template <typename T = std::filesystem::directory_iterator>
	WStrings list_t(CheckEntry check_entry)
	{
		WStrings wstrings;
		if (is_folder())
		{
			for (const auto& p : T(m_path, m_options))
			{
				if (check_entry(p)) wstrings.emplace_back(p.path().wstring());
			}
		}
		return wstrings;
	}

	uint32_t guess_codepage(jstring content)
	{
		int size = static_cast<int>(content.length());
		if (size == 0) return 0;

		constexpr int maxEncodings = 1;
		int encodingCount = maxEncodings;
		std::array<DetectEncodingInfo, maxEncodings> encodings;

		pfc::com_ptr_t<IMultiLanguage2> lang;
		if (FAILED(CoCreateInstance(CLSID_CMultiLanguage, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(lang.receive_ptr())))) return 0;
		if (FAILED(lang->DetectInputCodepage(MLDETECTCP_NONE, 0, const_cast<char*>(content.get_ptr()), &size, encodings.data(), &encodingCount))) return 0;

		const uint32_t codepage = encodings[0].nCodePage;
		if (codepage == 20127) return CP_UTF8;
		return codepage;
	}

	static constexpr const char* UTF_16_LE_BOM = "\xFF\xFE";
	static constexpr const char* UTF_8_BOM = "\xEF\xBB\xBF";

	std::filesystem::directory_options m_options = std::filesystem::directory_options::skip_permission_denied;
	std::filesystem::path m_path;
};
