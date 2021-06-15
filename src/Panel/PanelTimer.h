#pragma once
#include "PanelManager.h"

class PanelTimer
{
public:
	PanelTimer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once, uint32_t id) : m_hwnd(hwnd)
		, m_pdisp(pdisp)
		, m_delay(delay)
		, m_execute_once(execute_once)
		, m_id(id)
	{
		m_pdisp->AddRef();
	}

	~PanelTimer()
	{
		m_pdisp->Release();
	}

	static VOID CALLBACK timerProc(PVOID lpParameter, BOOLEAN)
	{
		auto timer = static_cast<PanelTimer*>(lpParameter);

		if (timer->m_stopped)
		{
			return;
		}

		if (timer->m_stop_requested)
		{
			timer->m_stopped = true;
			PanelManager::instance().remove_timer(timer->m_handle, timer->m_id);
			return;
		}

		if (timer->m_execute_once)
		{
			timer->m_stopped = true;
			timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
			PanelManager::instance().remove_timer(timer->m_handle, timer->m_id);
			return;
		}

		timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
	}

	BOOL start(HANDLE timer_queue)
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

	void invoke()
	{
		DISPPARAMS params{};
		m_pdisp->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}

	void stop()
	{
		m_stop_requested = true;
	}

	CWindow m_hwnd;

private:
	HANDLE m_handle = nullptr;
	IDispatch* m_pdisp = nullptr;
	bool m_execute_once = false;
	uint32_t m_delay = 0, m_id = 0;
	std::atomic_bool m_stop_requested = false, m_stopped = false;
};
