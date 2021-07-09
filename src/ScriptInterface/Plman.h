#pragma once

class Plman : public JSDispatchImpl<IPlman>
{
public:
	STDMETHODIMP AddItemToPlaybackQueue(IMetadbHandle* handle) override;
	STDMETHODIMP AddLocations(UINT playlistIndex, VARIANT locations, VARIANT_BOOL select) override;
	STDMETHODIMP AddPlaylistItemToPlaybackQueue(UINT playlistIndex, UINT playlistItemIndex) override;
	STDMETHODIMP AddPlaylistLock(UINT playlistIndex, UINT flags, VARIANT_BOOL* out) override;
	STDMETHODIMP ClearPlaylist(UINT playlistIndex) override;
	STDMETHODIMP ClearPlaylistSelection(UINT playlistIndex) override;
	STDMETHODIMP CreateAutoPlaylist(UINT playlistIndex, BSTR name, BSTR query, BSTR sort, UINT flags, int* out) override;
	STDMETHODIMP CreatePlaylist(UINT playlistIndex, BSTR name, UINT* out) override;
	STDMETHODIMP DuplicatePlaylist(UINT from, BSTR name, UINT* out) override;
	STDMETHODIMP EnsurePlaylistItemVisible(UINT playlistIndex, UINT playlistItemIndex) override;
	STDMETHODIMP ExecutePlaylistDefaultAction(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP FindOrCreatePlaylist(BSTR name, VARIANT_BOOL unlocked, UINT* out) override;
	STDMETHODIMP FindPlaybackQueueItemIndex(IMetadbHandle* handle, UINT playlistIndex, UINT playlistItemIndex, int* out) override;
	STDMETHODIMP FindPlaylist(BSTR name, int* out) override;
	STDMETHODIMP FlushPlaybackQueue() override;
	STDMETHODIMP GetPlaybackQueueContents(VARIANT* out) override;
	STDMETHODIMP GetPlaybackQueueHandles(IMetadbHandleList** out) override;
	STDMETHODIMP GetPlayingItemLocation(IPlayingItemLocation** out) override;
	STDMETHODIMP GetPlaylistFocusItemIndex(UINT playlistIndex, int* out) override;
	STDMETHODIMP GetPlaylistItems(UINT playlistIndex, IMetadbHandleList** out) override;
	STDMETHODIMP GetPlaylistLockFilterMask(UINT playlistIndex, int* out) override;
	STDMETHODIMP GetPlaylistLockName(UINT playlistIndex, BSTR* out) override;
	STDMETHODIMP GetPlaylistName(UINT playlistIndex, BSTR* out) override;
	STDMETHODIMP GetPlaylistSelectedItems(UINT playlistIndex, IMetadbHandleList** out) override;
	STDMETHODIMP GetRecyclerItems(UINT index, IMetadbHandleList** out) override;
	STDMETHODIMP GetRecyclerName(UINT index, BSTR* out) override;
	STDMETHODIMP InsertPlaylistItems(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select) override;
	STDMETHODIMP InsertPlaylistItemsFilter(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select) override;
	STDMETHODIMP IsAutoPlaylist(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP IsPlaylistItemSelected(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP IsPlaylistLocked(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP MovePlaylist(UINT from, UINT to, VARIANT_BOOL* out) override;
	STDMETHODIMP MovePlaylistSelection(UINT playlistIndex, int delta, VARIANT_BOOL* out) override;
	STDMETHODIMP PlaylistItemCount(UINT playlistIndex, UINT* out) override;
	STDMETHODIMP RecyclerPurge(VARIANT affectedItems) override;
	STDMETHODIMP RecyclerRestore(UINT index) override;
	STDMETHODIMP RemoveItemFromPlaybackQueue(UINT index) override;
	STDMETHODIMP RemoveItemsFromPlaybackQueue(VARIANT affectedItems) override;
	STDMETHODIMP RemovePlaylist(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP RemovePlaylistLock(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP RemovePlaylists(VARIANT playlistIndexes, VARIANT_BOOL* out) override;
	STDMETHODIMP RemovePlaylistSelection(UINT playlistIndex, VARIANT_BOOL crop) override;
	STDMETHODIMP RemovePlaylistSwitch(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP RenamePlaylist(UINT playlistIndex, BSTR name, VARIANT_BOOL* out) override;
	STDMETHODIMP SetActivePlaylistContext() override;
	STDMETHODIMP SetPlaylistFocusItem(UINT playlistIndex, UINT playlistItemIndex) override;
	STDMETHODIMP SetPlaylistFocusItemByHandle(UINT playlistIndex, IMetadbHandle* handle) override;
	STDMETHODIMP SetPlaylistSelection(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state) override;
	STDMETHODIMP SetPlaylistSelectionSingle(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state) override;
	STDMETHODIMP ShowAutoPlaylistUI(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP ShowPlaylistLockUI(UINT playlistIndex, VARIANT_BOOL* out) override;
	STDMETHODIMP SortByFormat(UINT playlistIndex, BSTR pattern, VARIANT_BOOL selOnly, VARIANT_BOOL* out) override;
	STDMETHODIMP SortByFormatV2(UINT playlistIndex, BSTR pattern, int direction, VARIANT_BOOL* out) override;
	STDMETHODIMP SortPlaylistsByName(int direction) override;
	STDMETHODIMP UndoBackup(UINT playlistIndex) override;
	STDMETHODIMP get_ActivePlaylist(int* out) override;
	STDMETHODIMP get_PlaybackOrder(UINT* out) override;
	STDMETHODIMP get_PlayingPlaylist(int* out) override;
	STDMETHODIMP get_PlaylistCount(UINT* out) override;
	STDMETHODIMP get_RecyclerCount(UINT* out) override;
	STDMETHODIMP put_ActivePlaylist(UINT playlistIndex) override;
	STDMETHODIMP put_PlaybackOrder(UINT order) override;

private:
	uint32_t CreatePlaylist(uint32_t playlistIndex, const std::wstring& name);
};
