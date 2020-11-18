#include "stdafx.h"
#include "PanelManager.h"

PanelManager::PanelManager() {}

PanelManager& PanelManager::instance()
{
	static PanelManager instance;
	return instance;
}

void PanelManager::add_window(CWindow hwnd)
{
	m_hwnds.insert(hwnd);
}

void PanelManager::post_msg_to_all(CallbackID id, WPARAM wp, LPARAM lp)
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.PostMessage(to_uint(id), wp, lp);
	}
}

void PanelManager::post_msg_to_all_pointer(CallbackID id, pfc::refcounted_object_root* param, HWND except)
{
	size_t count = m_hwnds.size();
	if (count > 0 && except != nullptr) count--;
	if (count == 0) return;

	for (size_t i = 0; i < count; ++i)
	{
		param->refcount_add_ref();
	}

	for (CWindow hwnd : m_hwnds)
	{
		if (hwnd != except)
		{
			hwnd.PostMessage(to_uint(id), reinterpret_cast<WPARAM>(param));
		}
	}
}

void PanelManager::remove_window(CWindow hwnd)
{
	m_hwnds.erase(hwnd);
}

void PanelManager::unload_all()
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.SendMessage(jsp::uwm_unload);
	}
}