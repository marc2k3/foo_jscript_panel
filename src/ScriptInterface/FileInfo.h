#pragma once

class FileInfo : public JSDisposableImpl<IFileInfo>
{
protected:
	FileInfo(const metadb_info_container::ptr& info);
	~FileInfo();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP InfoFind(BSTR name, int* out) override;
	STDMETHODIMP InfoName(UINT idx, BSTR* out) override;
	STDMETHODIMP InfoValue(UINT idx, BSTR* out) override;
	STDMETHODIMP MetaFind(BSTR name, int* out) override;
	STDMETHODIMP MetaName(UINT idx, BSTR* out) override;
	STDMETHODIMP MetaValue(UINT idx, UINT vidx, BSTR* out) override;
	STDMETHODIMP MetaValueCount(UINT idx, UINT* out) override;
	STDMETHODIMP get_InfoCount(UINT* out) override;
	STDMETHODIMP get_MetaCount(UINT* out) override;

private:
	metadb_info_container::ptr m_info;
};
