#pragma once

class Utils : public JSDispatchImpl<IUtils>
{
public:
	STDMETHODIMP Chardet(BSTR filename, UINT* out) override;
	STDMETHODIMP CheckComponent(BSTR name, VARIANT_BOOL* out) override;
	STDMETHODIMP CheckFont(BSTR name, VARIANT_BOOL* out) override;
	STDMETHODIMP ColourPicker(UINT window_id, int default_colour, int* out) override;
	STDMETHODIMP DateStringToTimestamp(BSTR str, UINT64* out) override;
	STDMETHODIMP FormatDuration(double seconds, BSTR* out) override;
	STDMETHODIMP FormatFileSize(UINT64 bytes, BSTR* out) override;
	STDMETHODIMP GetAlbumArtAsync(UINT window_id, IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed, VARIANT_BOOL /* FFS */) override;
	STDMETHODIMP GetAlbumArtEmbedded(BSTR rawpath, UINT art_id, IGdiBitmap** out) override;
	STDMETHODIMP GetAlbumArtV2(IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, IGdiBitmap** out) override;
	STDMETHODIMP GetFileSize(BSTR filename, __int64* out) override;
	STDMETHODIMP GetSysColour(UINT index, int* out) override;
	STDMETHODIMP GetSystemMetrics(UINT index, int* out) override;
	STDMETHODIMP Glob(BSTR pattern, UINT exc_mask, UINT inc_mask, VARIANT* out) override;
	STDMETHODIMP InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* out) override;
	STDMETHODIMP IsFile(BSTR filename, VARIANT_BOOL* out) override;
	STDMETHODIMP IsFolder(BSTR folder, VARIANT_BOOL* out) override;
	STDMETHODIMP IsKeyPressed(UINT vkey, VARIANT_BOOL* out) override;
	STDMETHODIMP ListFiles(BSTR folder, VARIANT_BOOL recur, VARIANT* out) override;
	STDMETHODIMP ListFolders(BSTR folder, VARIANT_BOOL recur, VARIANT* out) override;
	STDMETHODIMP ListFonts(VARIANT* out) override;
	STDMETHODIMP MapString(BSTR str, UINT lcid, UINT flags, BSTR* out) override;
	STDMETHODIMP PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* out) override;
	STDMETHODIMP ReadINI(BSTR filename, BSTR section, BSTR key, BSTR defaultval, BSTR* out) override;
	STDMETHODIMP ReadTextFile(BSTR filename, UINT codepage, BSTR* out) override;
	STDMETHODIMP ReadUTF8(BSTR filename, BSTR* out) override;
	STDMETHODIMP ReplaceIllegalChars(BSTR str, VARIANT_BOOL modern, BSTR* out) override;
	STDMETHODIMP TimestampToDateString(UINT64 ts, BSTR* out) override;
	STDMETHODIMP WriteINI(BSTR filename, BSTR section, BSTR key, BSTR val, VARIANT_BOOL* out) override;
	STDMETHODIMP WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL* out) override;
	STDMETHODIMP get_Version(UINT* out) override;

private:
	std::array<COLORREF, 16> m_colours{};
};
