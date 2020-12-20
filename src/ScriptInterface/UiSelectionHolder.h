#pragma once

class UiSelectionHolder : public JSDisposableImpl<IUiSelectionHolder>
{
protected:
	UiSelectionHolder();
	~UiSelectionHolder();

	void FinalRelease() override;

public:
	STDMETHODIMP SetPlaylistSelectionTracking() override;
	STDMETHODIMP SetPlaylistTracking() override;
	STDMETHODIMP SetSelection(IMetadbHandleList* handles) override;

private:
	ui_selection_holder::ptr m_holder;
};
