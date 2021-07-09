#pragma once

class MetadbHandleList : public JSDisposableImpl<IMetadbHandleList>
{
public:
	MetadbHandleList(metadb_handle_list_cref handles);

	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP Add(IMetadbHandle* handle) override;
	STDMETHODIMP AddRange(IMetadbHandleList* handles) override;
	STDMETHODIMP AttachImage(BSTR path, UINT art_id) override;
	STDMETHODIMP BSearch(IMetadbHandle* handle, int* out) override;
	STDMETHODIMP CalcTotalDuration(double* out) override;
	STDMETHODIMP CalcTotalSize(UINT64* out) override;
	STDMETHODIMP Clone(IMetadbHandleList** out) override;
	STDMETHODIMP Convert(VARIANT* out) override;
	STDMETHODIMP Find(IMetadbHandle* handle, int* out) override;
	STDMETHODIMP GetLibraryRelativePaths(VARIANT* out) override;
	STDMETHODIMP Insert(UINT index, IMetadbHandle* handle) override;
	STDMETHODIMP InsertRange(UINT index, IMetadbHandleList* handles) override;
	STDMETHODIMP MakeDifference(IMetadbHandleList* handles) override;
	STDMETHODIMP MakeIntersection(IMetadbHandleList* handles) override;
	STDMETHODIMP OptimiseFileLayout(VARIANT_BOOL minimise) override;
	STDMETHODIMP OrderByFormat(ITitleFormat* script, int direction) override;
	STDMETHODIMP OrderByPath() override;
	STDMETHODIMP OrderByRelativePath() override;
	STDMETHODIMP Randomise() override;
	STDMETHODIMP RefreshStats() override;
	STDMETHODIMP Remove(IMetadbHandle* handle) override;
	STDMETHODIMP RemoveAll() override;
	STDMETHODIMP RemoveAttachedImage(UINT art_id) override;
	STDMETHODIMP RemoveAttachedImages() override;
	STDMETHODIMP RemoveById(UINT index) override;
	STDMETHODIMP RemoveDuplicates() override;
	STDMETHODIMP RemoveDuplicatesByFormat(ITitleFormat* script) override;
	STDMETHODIMP RemoveRange(UINT from, UINT count) override;
	STDMETHODIMP SaveAs(BSTR filename) override;
	STDMETHODIMP UpdateFileInfoFromJSON(BSTR str) override;
	STDMETHODIMP get_Count(UINT* out) override;
	STDMETHODIMP get_Item(UINT index, IMetadbHandle** out) override;
	STDMETHODIMP put_Item(UINT index, IMetadbHandle* handle) override;

protected:
	void FinalRelease() override;

private:
	static constexpr uint32_t flags = threaded_process::flag_show_progress | threaded_process::flag_show_delayed | threaded_process::flag_show_item;

	metadb_handle_list m_handles;
};
