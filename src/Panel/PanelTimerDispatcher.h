#pragma once

class PanelTimer
{
public:
	PanelTimer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once, uint32_t id);
	~PanelTimer();

	static VOID CALLBACK timerProc(PVOID lpParameter, BOOLEAN);

	BOOL start(HANDLE timer_queue);
	void invoke();
	void stop();

	CWindow m_hwnd;

private:
	HANDLE m_handle = nullptr;
	IDispatch* m_pdisp = nullptr;
	bool m_execute_once = false;
	uint32_t m_delay = 0, m_id = 0;
	std::atomic_bool m_stop_requested = false, m_stopped = false;
};

class PanelTimerDispatcher
{
public:
	PanelTimerDispatcher();
	~PanelTimerDispatcher();

	static PanelTimerDispatcher& instance();

	uint32_t create_timer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once);
	void invoke_message(uint32_t timer_id);
	void request_stop(CWindow hwnd, uint32_t timer_id);
	void request_stop_multi(CWindow hwnd);
	void remove_timer(HANDLE timer_handle, uint32_t timer_id);

private:
	using TimerMap = std::map<uint32_t, std::unique_ptr<PanelTimer>>;

	HANDLE m_timer_queue;
	TimerMap m_timer_map;
	uint32_t m_cur_timer_id = 0;
	std::mutex m_mutex;

	PFC_CLASS_NOT_COPYABLE_EX(PanelTimerDispatcher)
};
