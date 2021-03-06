#include "stdafx.h"
#include "Utils.h"
#include "DialogInputBox.h"
#include "AsyncArtTask.h"

#include <foobar2000/helpers/filetimetools.h>

STDMETHODIMP Utils::Chardet(BSTR filename, UINT* out)
{
	if (!out) return E_POINTER;

	*out = FileHelper(filename).guess_codepage();
	return S_OK;
}

STDMETHODIMP Utils::CheckComponent(BSTR name, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	const string8 uname = from_wide(name);

	*out = VARIANT_FALSE;

	for (auto e = service_enum_t<componentversion>(); !e.finished(); ++e)
	{
		string8 str;
		e.get()->get_file_name(str);
		if (_stricmp(str, uname) == 0)
		{
			*out = VARIANT_TRUE;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP Utils::CheckFont(BSTR name, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FontHelpers::check_name(name));
	return S_OK;
}

STDMETHODIMP Utils::ColourPicker(UINT window_id, int default_colour, int* out)
{
	if (!out) return E_POINTER;

	COLORREF colour = to_colorref(default_colour);
	uChooseColor(&colour, reinterpret_cast<HWND>(window_id), m_colours.data());
	*out = to_argb(colour);
	return S_OK;
}

STDMETHODIMP Utils::DateStringToTimestamp(BSTR str, UINT64* out)
{
	if (!out) return E_POINTER;

	*out = pfc::fileTimeWtoU(filetimestamp_from_string(from_wide(str)));
	return S_OK;
}

STDMETHODIMP Utils::FormatDuration(double seconds, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(pfc::format_time_ex(seconds, 0).get_ptr());
	return S_OK;
}

STDMETHODIMP Utils::FormatFileSize(UINT64 bytes, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(pfc::format_file_size_short(bytes));
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtAsync(UINT window_id, IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed, VARIANT_BOOL /* FFS */)
{
	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	auto task = std::make_unique<AsyncArtTask>(reinterpret_cast<HWND>(window_id), ptr, art_id, to_bool(need_stub), to_bool(only_embed));
	SimpleThreadPool::instance().add_task(std::move(task));
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtEmbedded(BSTR rawpath, UINT art_id, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	album_art_data_ptr data = AlbumArt::get_embedded(from_wide(rawpath), art_id);
	*out = AlbumArt::data_to_bitmap(data);
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtV2(IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	string8 dummy;
	album_art_data_ptr data = AlbumArt::get(ptr, art_id, to_bool(need_stub), dummy);
	*out = AlbumArt::data_to_bitmap(data);
	return S_OK;
}

STDMETHODIMP Utils::GetFileSize(BSTR path, __int64* out)
{
	if (!out) return E_POINTER;

	LARGE_INTEGER file_size = { 0 };
	CloseHandleScope hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile.Get() != INVALID_HANDLE_VALUE) GetFileSizeEx(hFile.Get(), &file_size);

	*out = file_size.QuadPart;
	return S_OK;
}

STDMETHODIMP Utils::GetSysColour(UINT index, int* out)
{
	if (!out) return E_POINTER;

	*out = 0;
	if (::GetSysColorBrush(index) != nullptr)
	{
		*out = to_argb(::GetSysColor(index));
	}
	return S_OK;
}

STDMETHODIMP Utils::GetSystemMetrics(UINT index, int* out)
{
	if (!out) return E_POINTER;

	*out = ::GetSystemMetrics(index);
	return S_OK;
}

STDMETHODIMP Utils::Glob(BSTR pattern, UINT exc_mask, UINT inc_mask, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings files;
	WIN32_FIND_DATA data;
	HANDLE hFindFile = FindFirstFile(pattern, &data);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		std::wstring folder = std::filesystem::path(pattern).parent_path().wstring() + std::filesystem::path::preferred_separator;

		do
		{
			const DWORD attr = data.dwFileAttributes;
			if ((attr & inc_mask) && !(attr & exc_mask))
			{
				files.emplace_back(folder + data.cFileName);
			}
		} while (FindNextFile(hFindFile, &data));
		FindClose(hFindFile);
	}

	const size_t count = files.size();
	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, files[i])) return E_OUTOFMEMORY;
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* out)
{
	if (!out) return E_POINTER;

	modal_dialog_scope scope;
	if (scope.can_create())
	{
		HWND hwnd = reinterpret_cast<HWND>(window_id);
		scope.initialize(hwnd);

		const string8 uprompt = from_wide(prompt);
		const string8 ucaption = from_wide(caption);
		const string8 udef = from_wide(def);

		CInputBox dlg(uprompt, ucaption, udef);
		const int status = dlg.DoModal(hwnd);
		if (status == IDOK)
		{
			*out = to_bstr(dlg.m_value);
		}
		else if (status == IDCANCEL)
		{
			if (to_bool(error_on_cancel))
			{
				return E_FAIL;
			}
			*out = to_bstr(udef);
		}
	}
	return S_OK;
}

STDMETHODIMP Utils::IsFile(BSTR filename, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FileHelper(filename).is_file());
	return S_OK;
}

STDMETHODIMP Utils::IsFolder(BSTR folder, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FileHelper(folder).is_folder());
	return S_OK;
}

STDMETHODIMP Utils::IsKeyPressed(UINT vkey, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(::IsKeyPressed(vkey));
	return S_OK;
}

STDMETHODIMP Utils::ListFiles(BSTR folder, VARIANT_BOOL recur, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings files = FileHelper(folder).list_files(to_bool(recur));
	const size_t count = files.size();

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, files[i])) return E_OUTOFMEMORY;
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::ListFolders(BSTR folder, VARIANT_BOOL recur, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings folders = FileHelper(folder).list_folders(to_bool(recur));
	const size_t count = folders.size();

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, folders[i] + std::filesystem::path::preferred_separator)) return E_OUTOFMEMORY;
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::ListFonts(VARIANT* out)
{
	if (!out) return E_POINTER;

	FontStrings fonts = FontHelpers::get_fonts();
	const size_t count = fonts.size();

	ComArrayWriter writer;
	if (!writer.create(count)) return E_OUTOFMEMORY;

	for (size_t i = 0; i < count; ++i)
	{
		if (!writer.put_item(i, fonts[i].data())) return E_OUTOFMEMORY;
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.get_ptr();
	return S_OK;
}

STDMETHODIMP Utils::MapString(BSTR str, UINT lcid, UINT flags, BSTR* out)
{
	if (!out) return E_POINTER;

	int r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, nullptr, 0);
	if (r)
	{
		std::wstring dst(r, '\0');
		r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, dst.data(), dst.size());
		if (r) *out = SysAllocString(dst.data());
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Utils::PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(PathMatchSpec(str, pattern));
	return S_OK;
}

STDMETHODIMP Utils::ReadINI(BSTR filename, BSTR section, BSTR key, BSTR defaultval, BSTR* out)
{
	if (!out) return E_POINTER;

	PathString buffer;
	GetPrivateProfileString(section, key, defaultval, buffer.data(), buffer.size(), filename);
	*out = SysAllocString(buffer.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadTextFile(BSTR filename, UINT codepage, BSTR* out)
{
	if (!out) return E_POINTER;

	std::wstring content;
	FileHelper(filename).read_wide(codepage, content);
	*out = SysAllocString(content.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadUTF8(BSTR filename, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(FileHelper(filename).read());
	return S_OK;
}

STDMETHODIMP Utils::ReplaceIllegalChars(BSTR str, VARIANT_BOOL modern, BSTR* out)
{
	if (!out) return E_POINTER;

	using namespace pfc::io::path;
	const pfc::string ustr = from_wide(str);

	if (to_bool(modern))
	{
		*out = to_bstr(replaceIllegalNameChars(ustr, false, charReplaceModern));
	}
	else
	{
		*out = to_bstr(replaceIllegalNameChars(ustr));
	}
	return S_OK;
}

STDMETHODIMP Utils::TimestampToDateString(UINT64 ts, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(format_filetimestamp(pfc::fileTimeUtoW(ts)).get_ptr());
	return S_OK;
}

STDMETHODIMP Utils::WriteINI(BSTR filename, BSTR section, BSTR key, BSTR val, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(WritePrivateProfileString(section, key, val, filename));
	return S_OK;
}

STDMETHODIMP Utils::WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;
	if (content != nullptr)
	{
		*out = to_variant_bool(FileHelper(filename).write(from_wide(content)));
	}
	return S_OK;
}

STDMETHODIMP Utils::get_Version(UINT* out)
{
	if (!out) return E_POINTER;

	*out = jsp::version;
	return S_OK;
}
