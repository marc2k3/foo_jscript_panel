#pragma once

class MetadbHandle : public JSDisposableImpl<IMetadbHandle>
{
public:
	MetadbHandle(const metadb_handle_ptr& handle);

	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP ClearStats() override;
	STDMETHODIMP Compare(IMetadbHandle* handle, VARIANT_BOOL* out) override;
	STDMETHODIMP GetAlbumArt(UINT art_id, VARIANT_BOOL need_stub, VARIANT* out) override;
	STDMETHODIMP GetFileInfo(IFileInfo** out) override;
	STDMETHODIMP RefreshStats() override;
	STDMETHODIMP SetFirstPlayed(BSTR first_played) override;
	STDMETHODIMP SetLastPlayed(BSTR last_played) override;
	STDMETHODIMP SetLoved(UINT loved) override;
	STDMETHODIMP SetPlaycount(UINT playcount) override;
	STDMETHODIMP SetRating(UINT rating) override;
	STDMETHODIMP ShowAlbumArtViewer(UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed) override;
	STDMETHODIMP get_FileSize(UINT64* out) override;
	STDMETHODIMP get_Length(double* out) override;
	STDMETHODIMP get_Path(BSTR* out) override;
	STDMETHODIMP get_RawPath(BSTR* out) override;
	STDMETHODIMP get_SubSong(UINT* out) override;

protected:
	void FinalRelease() override;

private:
	metadb_handle_ptr m_handle;
};
