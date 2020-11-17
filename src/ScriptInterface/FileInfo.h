#pragma once

class FileInfo : public JSDisposableImpl<IFileInfo>
{
protected:
	FileInfo(metadb_info_container::ptr info);
	~FileInfo();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP InfoFind(BSTR name, int* p) override;
	STDMETHODIMP InfoName(UINT idx, BSTR* p) override;
	STDMETHODIMP InfoValue(UINT idx, BSTR* p) override;
	STDMETHODIMP MetaFind(BSTR name, int* p) override;
	STDMETHODIMP MetaName(UINT idx, BSTR* p) override;
	STDMETHODIMP MetaValue(UINT idx, UINT vidx, BSTR* p) override;
	STDMETHODIMP MetaValueCount(UINT idx, UINT* p) override;
	STDMETHODIMP get_InfoCount(UINT* p) override;
	STDMETHODIMP get_MetaCount(UINT* p) override;

private:
	metadb_info_container::ptr m_info;
};
