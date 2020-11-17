#pragma once

class Fb : public JSDispatchImpl<IFb>
{
protected:
	Fb();
	~Fb();

public:
	STDMETHODIMP AcquireUiSelectionHolder(IUiSelectionHolder** pp) override;
	STDMETHODIMP AddDirectory() override;
	STDMETHODIMP AddFiles() override;
	STDMETHODIMP CheckClipboardContents(UINT /* FFS */, VARIANT_BOOL* p) override;
	STDMETHODIMP ClearPlaylist() override;
	STDMETHODIMP CopyHandleListToClipboard(IMetadbHandleList* handles, VARIANT_BOOL* p) override;
	STDMETHODIMP CreateContextMenuManager(IContextMenuManager** pp) override;
	STDMETHODIMP CreateHandleList(VARIANT handle, IMetadbHandleList** pp) override;
	STDMETHODIMP CreateMainMenuManager(IMainMenuManager** pp) override;
	STDMETHODIMP CreateProfiler(BSTR name, IProfiler** pp) override;
	STDMETHODIMP DoDragDrop(IMetadbHandleList* handles, UINT okEffects, UINT* p) override;
	STDMETHODIMP Exit() override;
	STDMETHODIMP GetClipboardContents(UINT /* FFS */, IMetadbHandleList** pp) override;
	STDMETHODIMP GetDSPPresets(BSTR* p) override;
	STDMETHODIMP GetFocusItem(IMetadbHandle** pp) override;
	STDMETHODIMP GetLibraryItems(IMetadbHandleList** pp) override;
	STDMETHODIMP GetNowPlaying(IMetadbHandle** pp) override;
	STDMETHODIMP GetOutputDevices(BSTR* p) override;
	STDMETHODIMP GetQueryItems(IMetadbHandleList* handles, BSTR query, IMetadbHandleList** pp) override;
	STDMETHODIMP GetSelection(IMetadbHandle** pp) override;
	STDMETHODIMP GetSelections(UINT flags, IMetadbHandleList** pp) override;
	STDMETHODIMP GetSelectionType(UINT* p) override;
	STDMETHODIMP IsLibraryEnabled(VARIANT_BOOL* p) override;
	STDMETHODIMP IsMetadbInMediaLibrary(IMetadbHandle* handle, VARIANT_BOOL* p) override;
	STDMETHODIMP LoadPlaylist() override;
	STDMETHODIMP Next() override;
	STDMETHODIMP Pause() override;
	STDMETHODIMP Play() override;
	STDMETHODIMP PlayOrPause() override;
	STDMETHODIMP Prev() override;
	STDMETHODIMP Random() override;
	STDMETHODIMP RunContextCommand(BSTR command, VARIANT_BOOL* p) override;
	STDMETHODIMP RunContextCommandWithMetadb(BSTR command, VARIANT handle, VARIANT_BOOL* p) override;
	STDMETHODIMP RunMainMenuCommand(BSTR command, VARIANT_BOOL* p) override;
	STDMETHODIMP SavePlaylist() override;
	STDMETHODIMP SetDSPPreset(UINT idx) override;
	STDMETHODIMP SetOutputDevice(BSTR output, BSTR device) override;
	STDMETHODIMP ShowConsole() override;
	STDMETHODIMP ShowLibrarySearchUI(BSTR query) override;
	STDMETHODIMP ShowPopupMessage(BSTR msg, BSTR title) override;
	STDMETHODIMP ShowPreferences() override;
	STDMETHODIMP Stop() override;
	STDMETHODIMP TitleFormat(BSTR pattern, ITitleFormat** pp) override;
	STDMETHODIMP VolumeDown() override;
	STDMETHODIMP VolumeMute() override;
	STDMETHODIMP VolumeUp() override;
	STDMETHODIMP get_AlwaysOnTop(VARIANT_BOOL* p) override;
	STDMETHODIMP get_ComponentPath(BSTR* p) override;
	STDMETHODIMP get_CursorFollowPlayback(VARIANT_BOOL* p) override;
	STDMETHODIMP get_FoobarPath(BSTR* p) override;
	STDMETHODIMP get_IsPaused(VARIANT_BOOL* p) override;
	STDMETHODIMP get_IsPlaying(VARIANT_BOOL* p) override;
	STDMETHODIMP get_PlaybackFollowCursor(VARIANT_BOOL* p) override;
	STDMETHODIMP get_PlaybackLength(double* p) override;
	STDMETHODIMP get_PlaybackTime(double* p) override;
	STDMETHODIMP get_ProfilePath(BSTR* p) override;
	STDMETHODIMP get_ReplaygainMode(UINT* p) override;
	STDMETHODIMP get_StopAfterCurrent(VARIANT_BOOL* p) override;
	STDMETHODIMP get_Volume(float* p) override;
	STDMETHODIMP put_AlwaysOnTop(VARIANT_BOOL p) override;
	STDMETHODIMP put_CursorFollowPlayback(VARIANT_BOOL p) override;
	STDMETHODIMP put_PlaybackFollowCursor(VARIANT_BOOL p) override;
	STDMETHODIMP put_PlaybackTime(double time) override;
	STDMETHODIMP put_ReplaygainMode(UINT p) override;
	STDMETHODIMP put_StopAfterCurrent(VARIANT_BOOL p) override;
	STDMETHODIMP put_Volume(float value) override;
};
