#include "stdafx.h"
#include "PanelManager.h"
#include "PanelTimer.h"

PanelManager::PanelManager() : m_timer_queue(CreateTimerQueue()) {}

PanelManager::~PanelManager()
{
	DeleteTimerQueueEx(m_timer_queue, INVALID_HANDLE_VALUE);
}

PanelManager& PanelManager::instance()
{
	static PanelManager instance;
	return instance;
}

uint32_t PanelManager::create_timer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once)
{
	auto timer = std::make_unique<PanelTimer>(hwnd, pdisp, delay, execute_once, ++m_cur_timer_id);
	if (timer->start(m_timer_queue) && m_timer_map.try_emplace(m_cur_timer_id, std::move(timer)).second)
	{
		return m_cur_timer_id;
	}
	return 0;
}

void PanelManager::add_window(CWindow hwnd)
{
	m_hwnds.insert(hwnd);
}

void PanelManager::post_msg_to_all(CallbackID id, WPARAM wp)
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.PostMessage(to_uint(id), wp);
	}
}

void PanelManager::post_msg_to_all_pointer(CallbackID id, pfc::refcounted_object_root* param, HWND except)
{
	for (CWindow hwnd : m_hwnds)
	{
		if (hwnd != except)
		{
			param->refcount_add_ref();
			hwnd.PostMessage(to_uint(id), reinterpret_cast<WPARAM>(param));
		}
	}
}

void PanelManager::remove_timer(HANDLE timer_handle, uint32_t timer_id)
{
	DeleteTimerQueueTimer(m_timer_queue, timer_handle, nullptr);

	{
		std::scoped_lock lock(m_mutex);
		m_timer_map.erase(timer_id);
	}
}

void PanelManager::remove_window(CWindow hwnd)
{
	m_hwnds.erase(hwnd);
}

void PanelManager::stop_timer(CWindow hwnd, uint32_t timer_id)
{
	if (m_timer_map.contains(timer_id))
	{
		const auto& timer = m_timer_map.at(timer_id);
		if (timer->m_hwnd == hwnd) timer->stop();
	}
}

void PanelManager::stop_timers(CWindow hwnd)
{
	for (const auto& [id, timer] : m_timer_map)
	{
		if (timer->m_hwnd == hwnd) timer->stop();
	}
}

void PanelManager::timer_invoke(uint32_t timer_id)
{
	if (m_timer_map.contains(timer_id))
	{
		m_timer_map.at(timer_id)->invoke();
	}
}

void PanelManager::unload_all()
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.SendMessage(jsp::uwm_unload);
	}
}
