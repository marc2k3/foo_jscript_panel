#pragma once

class Utils : public JSDispatchImpl<IUtils>
{
protected:
	Utils();
	~Utils();

public:
	STDMETHODIMP Chardet(BSTR filename, UINT* p) override;
	STDMETHODIMP CheckComponent(BSTR name, VARIANT_BOOL* p) override;
	STDMETHODIMP CheckFont(BSTR name, VARIANT_BOOL* p) override;
	STDMETHODIMP ColourPicker(UINT window_id, int default_colour, int* p) override;
	STDMETHODIMP DateStringToTimestamp(BSTR str, UINT64* p) override;
	STDMETHODIMP FormatDuration(double seconds, BSTR* p) override;
	STDMETHODIMP FormatFileSize(UINT64 bytes, BSTR* p) override;
	STDMETHODIMP GetAlbumArtAsync(UINT window_id, IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed, VARIANT_BOOL no_load) override;
	STDMETHODIMP GetAlbumArtEmbedded(BSTR rawpath, UINT art_id, IGdiBitmap** pp) override;
	STDMETHODIMP GetAlbumArtV2(IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, IGdiBitmap** pp) override;
	STDMETHODIMP GetFileSize(BSTR filename, __int64* p) override;
	STDMETHODIMP GetSysColour(UINT index, int* p) override;
	STDMETHODIMP GetSystemMetrics(UINT index, int* p) override;
	STDMETHODIMP Glob(BSTR pattern, UINT exc_mask, UINT inc_mask, VARIANT* p) override;
	STDMETHODIMP InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* p) override;
	STDMETHODIMP IsFile(BSTR filename, VARIANT_BOOL* p) override;
	STDMETHODIMP IsFolder(BSTR folder, VARIANT_BOOL* p) override;
	STDMETHODIMP IsKeyPressed(UINT vkey, VARIANT_BOOL* p) override;
	STDMETHODIMP ListFiles(BSTR folder, VARIANT_BOOL recur, VARIANT* p) override;
	STDMETHODIMP ListFolders(BSTR folder, VARIANT_BOOL recur, VARIANT* p) override;
	STDMETHODIMP MapString(BSTR str, UINT lcid, UINT flags, BSTR* p) override;
	STDMETHODIMP PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* p) override;
	STDMETHODIMP ReadINI(BSTR filename, BSTR section, BSTR key, BSTR defaultval, BSTR* p) override;
	STDMETHODIMP ReadTextFile(BSTR filename, UINT codepage, BSTR* p) override;
	STDMETHODIMP ReadUTF8(BSTR filename, BSTR* p) override;
	STDMETHODIMP TimestampToDateString(UINT64 ts, BSTR* p) override;
	STDMETHODIMP WriteINI(BSTR filename, BSTR section, BSTR key, BSTR val, VARIANT_BOOL* p) override;
	STDMETHODIMP WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL* p) override;
	STDMETHODIMP get_Version(UINT* p) override;

private:
	std::array<COLORREF, 16> m_colours{};
};
