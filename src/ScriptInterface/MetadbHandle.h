#pragma once

class MetadbHandle : public JSDisposableImpl<IMetadbHandle>
{
protected:
	MetadbHandle(const metadb_handle_ptr& handle);
	~MetadbHandle();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP ClearStats() override;
	STDMETHODIMP Compare(IMetadbHandle* handle, VARIANT_BOOL* p) override;
	STDMETHODIMP GetAlbumArt(UINT art_id, VARIANT_BOOL need_stub, VARIANT* p) override;
	STDMETHODIMP GetFileInfo(IFileInfo** pp) override;
	STDMETHODIMP RefreshStats() override;
	STDMETHODIMP SetFirstPlayed(BSTR first_played) override;
	STDMETHODIMP SetLastPlayed(BSTR last_played) override;
	STDMETHODIMP SetLoved(UINT loved) override;
	STDMETHODIMP SetPlaycount(UINT playcount) override;
	STDMETHODIMP SetRating(UINT rating) override;
	STDMETHODIMP get_FileSize(UINT64* p) override;
	STDMETHODIMP get_Length(double* p) override;
	STDMETHODIMP get_Path(BSTR* p) override;
	STDMETHODIMP get_RawPath(BSTR* p) override;
	STDMETHODIMP get_SubSong(UINT* p) override;

private:
	metadb_handle_ptr m_handle;
};
