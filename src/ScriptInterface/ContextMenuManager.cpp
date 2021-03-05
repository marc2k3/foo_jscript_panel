#include "stdafx.h"
#include "ContextMenuManager.h"

STDMETHODIMP ContextMenuManager::BuildMenu(IMenuObj* obj, int base_id)
{
	if (m_cm.is_empty()) return E_POINTER;

	HMENU menu;
	obj->get__HMENU(&menu);
	contextmenu_node* parent = m_cm->get_root();
	m_cm->win32_build_menu(menu, parent, base_id, -1);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::ExecuteByID(UINT id, VARIANT_BOOL* out)
{
	if (m_cm.is_empty() || !out) return E_POINTER;

	*out = to_variant_bool(m_cm->execute_by_id(id));
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContext(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	contextmenu_manager::g_create(m_cm);
	m_cm->init_context(*handles_ptr, contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContextPlaylist()
{
	contextmenu_manager::g_create(m_cm);
	m_cm->init_context_playlist(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitNowPlaying()
{
	contextmenu_manager::g_create(m_cm);
	m_cm->init_context_now_playing(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

void ContextMenuManager::FinalRelease()
{
	m_cm.release();
}
