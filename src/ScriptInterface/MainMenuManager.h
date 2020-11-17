#pragma once

class MainMenuManager : public JSDisposableImpl<IMainMenuManager>
{
protected:
	MainMenuManager();
	~MainMenuManager();

	void FinalRelease() override;

public:
	STDMETHODIMP BuildMenu(IMenuObj* p, UINT base_id) override;
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* p) override;
	STDMETHODIMP Init(BSTR root_name) override;

private:
	struct ValidRootName
	{
		std::wstring name;
		const GUID* guid;
	};

	mainmenu_manager::ptr m_mm;
};
