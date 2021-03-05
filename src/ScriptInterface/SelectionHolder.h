#pragma once

class SelectionHolder : public JSDisposableImpl<ISelectionHolder>
{
public:
	SelectionHolder();

	STDMETHODIMP SetPlaylistSelectionTracking() override;
	STDMETHODIMP SetPlaylistTracking() override;
	STDMETHODIMP SetSelection(IMetadbHandleList* handles, UINT type) override;

protected:
	void FinalRelease() override;

private:
	ui_selection_holder::ptr m_holder;
};
