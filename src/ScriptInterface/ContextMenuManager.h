#pragma once

class ContextMenuManager : public JSDisposableImpl<IContextMenuManager>
{
protected:
	ContextMenuManager();
	~ContextMenuManager();

	void FinalRelease() override;

public:
	STDMETHODIMP BuildMenu(IMenuObj* p, int base_id) override;
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* p) override;
	STDMETHODIMP InitContext(IMetadbHandleList* handles) override;
	STDMETHODIMP InitContextPlaylist() override;
	STDMETHODIMP InitNowPlaying() override;

private:
	contextmenu_manager::ptr m_cm;
};
