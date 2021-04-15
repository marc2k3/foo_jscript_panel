#include "stdafx.h"
#include "PanelTimerDispatcher.h"

PanelTimer::PanelTimer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once, uint32_t id)
	: m_hwnd(hwnd)
	, m_pdisp(pdisp)
	, m_delay(delay)
	, m_execute_once(execute_once)
	, m_id(id)
{
	m_pdisp->AddRef();
}

PanelTimer::~PanelTimer()
{
	m_pdisp->Release();
}

VOID CALLBACK PanelTimer::timerProc(PVOID lpParameter, BOOLEAN)
{
	auto timer = static_cast<PanelTimer*>(lpParameter);

	if (timer->m_stopped)
	{
		return;
	}

	if (timer->m_stop_requested)
	{
		timer->m_stopped = true;
		PanelTimerDispatcher::instance().remove_timer(timer->m_handle, timer->m_id);
		return;
	}

	if (timer->m_execute_once)
	{
		timer->m_stopped = true;
		timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
		PanelTimerDispatcher::instance().remove_timer(timer->m_handle, timer->m_id);
		return;
	}

	timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
}

BOOL PanelTimer::start(HANDLE timer_queue)
{
	return CreateTimerQueueTimer(
		&m_handle,
		timer_queue,
		PanelTimer::timerProc,
		this,
		m_delay,
		m_execute_once ? 0 : m_delay,
		WT_EXECUTEINTIMERTHREAD | (m_execute_once ? WT_EXECUTEONLYONCE : WT_EXECUTEDEFAULT));
}

void PanelTimer::invoke()
{
	DISPPARAMS params{};
	m_pdisp->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
}

void PanelTimer::stop()
{
	m_stop_requested = true;
}

PanelTimerDispatcher::PanelTimerDispatcher() : m_timer_queue(CreateTimerQueue()) {}
PanelTimerDispatcher::~PanelTimerDispatcher()
{
	DeleteTimerQueueEx(m_timer_queue, INVALID_HANDLE_VALUE);
}

PanelTimerDispatcher& PanelTimerDispatcher::instance()
{
	static PanelTimerDispatcher instance;
	return instance;
}

uint32_t PanelTimerDispatcher::create_timer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once)
{
	auto timer = std::make_unique<PanelTimer>(hwnd, pdisp, delay, execute_once, ++m_cur_timer_id);
	if (timer->start(m_timer_queue) && m_timer_map.try_emplace(m_cur_timer_id, std::move(timer)).second)
	{
		return m_cur_timer_id;
	}
	return 0;
}

void PanelTimerDispatcher::invoke_message(uint32_t timer_id)
{
	if (m_timer_map.contains(timer_id))
	{
		m_timer_map.at(timer_id)->invoke();
	}
}

void PanelTimerDispatcher::request_stop(CWindow hwnd, uint32_t timer_id)
{
	if (m_timer_map.contains(timer_id))
	{
		const auto& timer = m_timer_map.at(timer_id);
		if (timer->m_hwnd == hwnd) timer->stop();
	}
}

void PanelTimerDispatcher::request_stop_multi(CWindow hwnd)
{
	for (const auto& [id, timer] : m_timer_map)
	{
		if (timer->m_hwnd == hwnd) timer->stop();
	}
}

void PanelTimerDispatcher::remove_timer(HANDLE timer_handle, uint32_t timer_id)
{
	DeleteTimerQueueTimer(m_timer_queue, timer_handle, nullptr);

	{
		std::scoped_lock lock(m_mutex);
		m_timer_map.erase(timer_id);
	}
}
