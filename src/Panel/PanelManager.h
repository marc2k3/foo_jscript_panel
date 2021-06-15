#pragma once

class PanelTimer;

class PanelManager
{
public:
	PanelManager();
	~PanelManager();

	static PanelManager& instance();

	uint32_t create_timer(CWindow hwnd, IDispatch* pdisp, uint32_t delay, bool execute_once);
	void add_window(CWindow hwnd);
	void invoke_message(uint32_t timer_id);
	void post_msg_to_all(CallbackID id, WPARAM wp = 0);
	void post_msg_to_all_pointer(CallbackID id, pfc::refcounted_object_root* param, HWND except = nullptr);
	void request_stop(CWindow hwnd, uint32_t timer_id);
	void request_stop_multi(CWindow hwnd);
	void remove_timer(HANDLE timer_handle, uint32_t timer_id);
	void remove_window(CWindow hwnd);
	void unload_all();

private:
	using TimerMap = std::map<uint32_t, std::unique_ptr<PanelTimer>>;

	HANDLE m_timer_queue;
	TimerMap m_timer_map;
	std::set<CWindow> m_hwnds;
	std::mutex m_mutex;
	uint32_t m_cur_timer_id = 0;

	PFC_CLASS_NOT_COPYABLE_EX(PanelManager)
};
