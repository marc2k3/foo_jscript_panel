#pragma once

class ContextMenuManager : public JSDisposableImpl<IContextMenuManager>
{
public:
	STDMETHODIMP BuildMenu(IMenuObj* obj, int base_id) override;
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* out) override;
	STDMETHODIMP InitContext(IMetadbHandleList* handles) override;
	STDMETHODIMP InitContextPlaylist() override;
	STDMETHODIMP InitNowPlaying() override;

protected:
	void FinalRelease() override;

private:
	contextmenu_manager::ptr m_cm;
};
