#pragma once

class UiSelectionHolder : public JSDisposableImpl<IUiSelectionHolder>
{
protected:
	UiSelectionHolder(const ui_selection_holder::ptr& holder);
	~UiSelectionHolder();

	void FinalRelease() override;

public:
	STDMETHODIMP SetPlaylistSelectionTracking() override;
	STDMETHODIMP SetPlaylistTracking() override;
	STDMETHODIMP SetSelection(IMetadbHandleList* handles) override;

private:
	ui_selection_holder::ptr m_holder;
};
