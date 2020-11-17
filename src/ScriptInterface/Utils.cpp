#include "stdafx.h"
#include "Utils.h"
#include "DialogInputBox.h"
#include "AsyncArtTask.h"

#include <foobar2000/helpers/filetimetools.h>

Utils::Utils() {}
Utils::~Utils() {}

STDMETHODIMP Utils::Chardet(BSTR filename, UINT* p)
{
	if (!p) return E_POINTER;

	*p = helpers::guess_codepage(helpers::read_file(string_utf8_from_wide(filename)));
	return S_OK;
}

STDMETHODIMP Utils::CheckComponent(BSTR name, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto uname = string_utf8_from_wide(name);

	*p = VARIANT_FALSE;

	for (auto e = service_enum_t<componentversion>(); !e.finished(); ++e)
	{
		string8 str;
		e.get()->get_file_name(str);
		if (_stricmp(str, uname) == 0)
		{
			*p = VARIANT_TRUE;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP Utils::CheckFont(BSTR name, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;

	Gdiplus::InstalledFontCollection fonts;
	const int count = fonts.GetFamilyCount();
	pfc::ptrholder_t<Gdiplus::FontFamily, pfc::releaser_delete_array> families = new Gdiplus::FontFamily[count];

	int found;
	if (families.is_valid() && fonts.GetFamilies(count, families.get_ptr(), &found) == Gdiplus::Ok)
	{
		std::array<wchar_t, LF_FACESIZE> family_name;
		for (int i = 0; i < found; ++i)
		{
			families.get_ptr()[i].GetFamilyName(family_name.data());
			if (_wcsicmp(name, family_name.data()) == 0)
			{
				*p = VARIANT_TRUE;
				break;
			}
		}
	}
	return S_OK;
}

STDMETHODIMP Utils::ColourPicker(UINT window_id, int default_colour, int* p)
{
	if (!p) return E_POINTER;

	COLORREF colour = to_colorref(default_colour);
	uChooseColor(&colour, reinterpret_cast<HWND>(window_id), m_colours.data());
	*p = to_argb(colour);
	return S_OK;
}

STDMETHODIMP Utils::DateStringToTimestamp(BSTR str, UINT64* p)
{
	if (!p) return E_POINTER;

	*p = pfc::fileTimeWtoU(filetimestamp_from_string(string_utf8_from_wide(str).get_ptr()));
	return S_OK;
}

STDMETHODIMP Utils::FormatDuration(double seconds, BSTR* p)
{
	if (!p) return E_POINTER;

	string8 str = pfc::format_time_ex(seconds, 0).get_ptr();
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Utils::FormatFileSize(UINT64 bytes, BSTR* p)
{
	if (!p) return E_POINTER;

	string8 str = pfc::format_file_size_short(bytes);
	*p = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtAsync(UINT window_id, IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed, VARIANT_BOOL no_load)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	auto task = std::make_unique<AsyncArtTask>(reinterpret_cast<HWND>(window_id), ptr, art_id, to_bool(need_stub), to_bool(only_embed), to_bool(no_load));
	SimpleThreadPool::instance().add_task(std::move(task));
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtEmbedded(BSTR rawpath, UINT art_id, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	*pp = helpers::get_album_art_embedded(string_utf8_from_wide(rawpath), art_id);
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtV2(IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	string8 dummy;
	*pp = helpers::get_album_art(ptr, art_id, to_bool(need_stub), false, dummy);
	return S_OK;
}

STDMETHODIMP Utils::GetFileSize(BSTR path, __int64* p)
{
	if (!p) return E_POINTER;

	LARGE_INTEGER file_size = { 0 };
	CloseHandleScope hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile.Get() != INVALID_HANDLE_VALUE) GetFileSizeEx(hFile.Get(), &file_size);

	*p = file_size.QuadPart;
	return S_OK;
}

STDMETHODIMP Utils::GetSysColour(UINT index, int* p)
{
	if (!p) return E_POINTER;

	*p = 0;
	if (::GetSysColorBrush(index) != nullptr)
	{
		*p = to_argb(::GetSysColor(index));
	}
	return S_OK;
}

STDMETHODIMP Utils::GetSystemMetrics(UINT index, int* p)
{
	if (!p) return E_POINTER;

	*p = ::GetSystemMetrics(index);
	return S_OK;
}

STDMETHODIMP Utils::Glob(BSTR pattern, UINT exc_mask, UINT inc_mask, VARIANT* p)
{
	if (!p) return E_POINTER;

	std::vector<std::wstring> strings;

	WIN32_FIND_DATA data;
	HANDLE hFindFile = FindFirstFile(pattern, &data);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		std::wstring path = pattern;
		std::wstring dir = path.substr(0, path.find_last_of(L"\\") + 1);

		do
		{
			const DWORD attr = data.dwFileAttributes;
			if ((attr & inc_mask) && !(attr & exc_mask))
			{
				strings.emplace_back(dir + data.cFileName);
			}
		} while (FindNextFile(hFindFile, &data));
		FindClose(hFindFile);
	}

	const size_t count = strings.size();
	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		_variant_t var;
		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(strings[i].data());

		if (!writer.put_item(i, var)) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* p)
{
	if (!p) return E_POINTER;

	modal_dialog_scope scope;
	if (scope.can_create())
	{
		HWND hwnd = reinterpret_cast<HWND>(window_id);
		scope.initialize(hwnd);

		auto uprompt = string_utf8_from_wide(prompt);
		auto ucaption = string_utf8_from_wide(caption);
		auto udef = string_utf8_from_wide(def);

		CInputBox dlg(uprompt, ucaption, udef);
		const int status = dlg.DoModal(hwnd);
		if (status == IDOK)
		{
			*p = to_bstr(dlg.m_value);
		}
		else if (status == IDCANCEL)
		{
			if (to_bool(error_on_cancel))
			{
				return E_FAIL;
			}
			*p = SysAllocString(def);
		}
	}
	return S_OK;
}

STDMETHODIMP Utils::IsFile(BSTR filename, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(!PathIsDirectory(filename) && PathFileExists(filename));
	return S_OK;
}

STDMETHODIMP Utils::IsFolder(BSTR folder, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(PathIsDirectory(folder));
	return S_OK;
}

STDMETHODIMP Utils::IsKeyPressed(UINT vkey, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(::IsKeyPressed(vkey));
	return S_OK;
}

STDMETHODIMP Utils::ListFiles(BSTR folder, VARIANT_BOOL recur, VARIANT* p)
{
	if (!p) return E_POINTER;

	Strings files = helpers::list_files(string_utf8_from_wide(folder).get_ptr(), to_bool(recur));
	const size_t count = files.size();

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, files[i].c_str())) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::ListFolders(BSTR folder, VARIANT_BOOL recur, VARIANT* p)
{
	if (!p) return E_POINTER;

	Strings folders = helpers::list_folders(string_utf8_from_wide(folder).get_ptr(), to_bool(recur));
	const size_t count = folders.size();

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, PFC_string_formatter() << folders[i].c_str() << "\\")) return E_OUTOFMEMORY;
	}

	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::MapString(BSTR str, UINT lcid, UINT flags, BSTR* p)
{
	if (!p) return E_POINTER;

	int r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, nullptr, 0);
	if (r)
	{
		std::wstring dst(r, '\0');
		r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, dst.data(), dst.size());
		if (r) *p = SysAllocString(dst.data());
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Utils::PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(PathMatchSpec(str, pattern));
	return S_OK;
}

STDMETHODIMP Utils::ReadINI(BSTR filename, BSTR section, BSTR key, BSTR defaultval, BSTR* p)
{
	if (!p) return E_POINTER;

	std::array<wchar_t, MAX_PATH> buf;
	GetPrivateProfileString(section, key, defaultval, buf.data(), buf.size(), filename);
	*p = SysAllocString(buf.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadTextFile(BSTR filename, UINT codepage, BSTR* p)
{
	if (!p) return E_POINTER;

	std::wstring content;
	helpers::read_file_wide(filename, codepage, content);
	*p = SysAllocString(content.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadUTF8(BSTR filename, BSTR* p)
{
	if (!p) return E_POINTER;

	*p = to_bstr(helpers::read_file(string_utf8_from_wide(filename)));
	return S_OK;
}

STDMETHODIMP Utils::TimestampToDateString(UINT64 ts, BSTR* p)
{
	if (!p) return E_POINTER;

	*p = to_bstr(format_filetimestamp(pfc::fileTimeUtoW(ts)).get_ptr());
	return S_OK;
}

STDMETHODIMP Utils::WriteINI(BSTR filename, BSTR section, BSTR key, BSTR val, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = to_variant_bool(WritePrivateProfileString(section, key, val, filename));
	return S_OK;
}

STDMETHODIMP Utils::WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	if (content != nullptr)
	{
		*p = to_variant_bool(helpers::write_file(string_utf8_from_wide(filename).get_ptr(), string_utf8_from_wide(content).get_ptr()));
	}
	return S_OK;
}

STDMETHODIMP Utils::get_Version(UINT* p)
{
	if (!p) return E_POINTER;

	*p = jsp::version;
	return S_OK;
}
