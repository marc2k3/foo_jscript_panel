#pragma once

class TitleFormat : public JSDisposableImpl<ITitleFormat>
{
public:
	TitleFormat(const std::wstring& pattern);

	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP Eval(BSTR* out) override;
	STDMETHODIMP EvalWithMetadb(IMetadbHandle* handle, BSTR* out) override;
	STDMETHODIMP EvalWithMetadbs(IMetadbHandleList* handles, VARIANT* out) override;

protected:
	void FinalRelease() override;

private:
	titleformat_object::ptr m_obj;
};
