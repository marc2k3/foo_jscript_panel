#pragma once

class PanelTimer
{
public:
	PanelTimer(CWindow hwnd, size_t id, size_t delay, bool is_repeated);
	~PanelTimer();

	static VOID CALLBACK timerProc(PVOID lpParameter, BOOLEAN);

	BOOL start(HANDLE timer_queue);
	void stop();

	CWindow m_hwnd;

private:
	HANDLE m_handle_timer = nullptr;
	IDispatch* m_pdisp = nullptr;
	bool m_is_repeated = false;
	bool m_is_stopped = false;
	size_t m_delay = 0;
	size_t m_id = 0;
	std::atomic_bool m_is_stop_requested = false;
};

class PanelTimerTask
{
public:
	PanelTimerTask(IDispatch* pdisp, size_t timer_id);
	~PanelTimerTask();

	void acquire();
	void invoke();
	void release();

private:
	IDispatch* m_pdisp = nullptr;
	pfc::refcounter m_refcounter = 0;
	size_t m_timer_id = 0;
};

class PanelTimerDispatcher
{
public:
	PanelTimerDispatcher();
	~PanelTimerDispatcher();

	static PanelTimerDispatcher& instance();

	size_t set_interval(CWindow hwnd, size_t delay, IDispatch* pdisp);
	size_t set_timeout(CWindow hwnd, size_t delay, IDispatch* pdisp);
	void invoke_message(size_t timer_id);
	void kill_timer(size_t timer_id);
	void kill_timers(CWindow hwnd);
	void on_task_complete(size_t timer_id);
	void on_timer_stop_request(CWindow hwnd, HANDLE handle_timer, size_t timer_id);

private:
	using TaskMap = std::map<size_t, std::unique_ptr<PanelTimerTask>>;
	using TimerMap = std::map<size_t, std::unique_ptr<PanelTimer>>;

	enum class ThreadTaskID
	{
		kill_timer_task,
		shutdown_task
	};

	struct ThreadTask
	{
		HANDLE handle_timer;
		CWindow hwnd;
		ThreadTaskID task_id;
		size_t timer_id;
	};

	size_t create_timer(CWindow hwnd, size_t delay, bool is_repeated, IDispatch* pdisp);
	void create_thread();
	void stop_thread();
	void thread_main();

	HANDLE m_timer_queue;
	TaskMap m_task_map;
	TimerMap m_timer_map;
	size_t m_cur_timer_id = 1;
	std::condition_variable m_cv;
	std::list<ThreadTask> m_thread_task_list;
	std::mutex m_timer_mutex;
	std::mutex m_thread_task_mutex;
	std::thread* m_thread{};

	PFC_CLASS_NOT_COPYABLE_EX(PanelTimerDispatcher)
};
