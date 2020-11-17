#pragma once

class TitleFormat : public JSDisposableImpl<ITitleFormat>
{
protected:
	TitleFormat(stringp pattern);
	~TitleFormat();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP Eval(BSTR* p) override;
	STDMETHODIMP EvalWithMetadb(IMetadbHandle* handle, BSTR* p) override;
	STDMETHODIMP EvalWithMetadbs(IMetadbHandleList* handles, VARIANT* p) override;

private:
	titleformat_object::ptr m_obj;
};
