#pragma once

class TitleFormat : public JSDisposableImpl<ITitleFormat>
{
protected:
	TitleFormat(const std::wstring& pattern);
	~TitleFormat();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP Eval(BSTR* out) override;
	STDMETHODIMP EvalWithMetadb(IMetadbHandle* handle, BSTR* out) override;
	STDMETHODIMP EvalWithMetadbs(IMetadbHandleList* handles, VARIANT* out) override;

private:
	titleformat_object::ptr m_obj;
};
