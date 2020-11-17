#include "stdafx.h"
#include "UiSelectionHolder.h"

UiSelectionHolder::UiSelectionHolder(const ui_selection_holder::ptr& holder) : m_holder(holder) {}
UiSelectionHolder::~UiSelectionHolder() {}

STDMETHODIMP UiSelectionHolder::SetPlaylistSelectionTracking()
{
	m_holder->set_playlist_selection_tracking();
	return S_OK;
}

STDMETHODIMP UiSelectionHolder::SetPlaylistTracking()
{
	m_holder->set_playlist_tracking();
	return S_OK;
}

STDMETHODIMP UiSelectionHolder::SetSelection(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	m_holder->set_selection(*handles_ptr);
	return S_OK;
}

void UiSelectionHolder::FinalRelease()
{
	m_holder.release();
}
