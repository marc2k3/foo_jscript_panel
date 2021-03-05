#pragma once

class MainMenuManager : public JSDisposableImpl<IMainMenuManager>
{
public:
	STDMETHODIMP BuildMenu(IMenuObj* obj, UINT base_id) override;
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* out) override;
	STDMETHODIMP Init(BSTR root_name) override;

protected:
	void FinalRelease() override;

private:
	struct ValidRootName
	{
		std::wstring name;
		const GUID* guid;
	};

	mainmenu_manager::ptr m_mm;
};
