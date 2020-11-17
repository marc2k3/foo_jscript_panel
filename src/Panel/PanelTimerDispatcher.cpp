#include "stdafx.h"
#include "PanelTimerDispatcher.h"

PanelTimer::PanelTimer(CWindow hwnd, size_t id, size_t delay, bool is_repeated) : m_hwnd(hwnd), m_id(id), m_delay(delay), m_is_repeated(is_repeated) {}

PanelTimer::~PanelTimer() {}

VOID CALLBACK PanelTimer::timerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	auto timer = static_cast<PanelTimer*>(lpParameter);

	if (timer->m_is_stopped)
	{
		return;
	}

	if (timer->m_is_stop_requested)
	{
		timer->m_is_stopped = true;
		PanelTimerDispatcher::instance().on_timer_stop_request(timer->m_hwnd, timer->m_handle_timer, timer->m_id);

		return;
	}

	if (!timer->m_is_repeated)
	{
		timer->m_is_stopped = true;
		timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
		PanelTimerDispatcher::instance().on_timer_stop_request(timer->m_hwnd, timer->m_handle_timer, timer->m_id);

		return;
	}

	timer->m_hwnd.SendMessage(jsp::uwm_timer, timer->m_id);
}

BOOL PanelTimer::start(HANDLE timer_queue)
{
	return CreateTimerQueueTimer(
		&m_handle_timer,
		timer_queue,
		PanelTimer::timerProc,
		this,
		m_delay,
		m_is_repeated ? m_delay : 0,
		WT_EXECUTEINTIMERTHREAD | (m_is_repeated ? 0 : WT_EXECUTEONLYONCE));
}

void PanelTimer::stop()
{
	m_is_stop_requested = true;
}

PanelTimerTask::PanelTimerTask(IDispatch* p_disp, size_t timer_id) : m_pdisp(p_disp), m_timer_id(timer_id)
{
	m_pdisp->AddRef();
}

PanelTimerTask::~PanelTimerTask()
{
	m_pdisp->Release();
}

void PanelTimerTask::acquire()
{
	++m_refcounter;
}

void PanelTimerTask::invoke()
{
	acquire();

	VARIANTARG args[1];
	args[0].vt = VT_I4;
	args[0].lVal = m_timer_id;
	DISPPARAMS dispParams = { args, nullptr, _countof(args), 0 };
	m_pdisp->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispParams, nullptr, nullptr, nullptr);

	release();
}

void PanelTimerTask::release()
{
	if (m_refcounter && --m_refcounter == 0)
	{
		PanelTimerDispatcher::instance().on_task_complete(m_timer_id);
	}
}

PanelTimerDispatcher::PanelTimerDispatcher()
{
	m_timer_queue = CreateTimerQueue();
}

PanelTimerDispatcher::~PanelTimerDispatcher()
{
	stop_thread();
	m_task_map.clear();
}

PanelTimerDispatcher& PanelTimerDispatcher::instance()
{
	static PanelTimerDispatcher instance;
	return instance;
}

size_t PanelTimerDispatcher::create_timer(CWindow hwnd, size_t delay, bool is_repeated, IDispatch* pdisp)
{
	if (!pdisp)
	{
		return 0;
	}

	std::scoped_lock lock(m_timer_mutex);

	size_t id = m_cur_timer_id++;
	while (m_task_map.count(id) && m_timer_map.count(id))
	{
		id = m_cur_timer_id++;
	}

	m_timer_map.emplace(id, new PanelTimer(hwnd, id, delay, is_repeated));

	const auto curTask = m_task_map.emplace(id, new PanelTimerTask(pdisp, id));
	curTask.first->second->acquire();

	if (m_timer_map[id]->start(m_timer_queue))
	{
		return id;
	}

	m_timer_map.erase(id);
	m_task_map.erase(id);
	return 0;
}

size_t PanelTimerDispatcher::set_interval(CWindow hwnd, size_t delay, IDispatch* pdisp)
{
	return create_timer(hwnd, delay, true, pdisp);
}

size_t PanelTimerDispatcher::set_timeout(CWindow hwnd, size_t delay, IDispatch* pdisp)
{
	return create_timer(hwnd, delay, false, pdisp);
}

void PanelTimerDispatcher::create_thread()
{
	m_thread = new std::thread(&PanelTimerDispatcher::thread_main, this);
}

void PanelTimerDispatcher::invoke_message(size_t timer_id)
{
	if (m_task_map.count(timer_id))
	{
		m_task_map.at(timer_id)->invoke();
	}
}

void PanelTimerDispatcher::kill_timer(size_t timer_id)
{
	{
		std::scoped_lock lock(m_timer_mutex);

		if (m_timer_map.count(timer_id))
		{
			m_timer_map.at(timer_id)->stop();
		}
	}

	if (m_task_map.count(timer_id))
	{
		m_task_map.at(timer_id)->release();
	}
}

void PanelTimerDispatcher::kill_timers(CWindow hwnd)
{
	std::list<size_t> timersToDelete;

	{
		std::scoped_lock lock(m_timer_mutex);
		for (const auto& [id, timer] : m_timer_map)
		{
			if (timer->m_hwnd == hwnd)
			{
				timersToDelete.emplace_back(id);
			}
		}
	}

	for (auto timerId : timersToDelete)
	{
		kill_timer(timerId);
	}
}

void PanelTimerDispatcher::on_task_complete(size_t timerId)
{
	m_task_map.erase(timerId);
}

void PanelTimerDispatcher::on_timer_stop_request(CWindow hwnd, HANDLE handle_timer, size_t timer_id)
{
	std::unique_lock<std::mutex> lock(m_thread_task_mutex);

	ThreadTask thread_task{};
	thread_task.task_id = ThreadTaskID::kill_timer_task;
	thread_task.hwnd = hwnd;
	thread_task.handle_timer = handle_timer;
	thread_task.timer_id = timer_id;

	m_thread_task_list.push_front(thread_task);
	m_cv.notify_one();
}

void PanelTimerDispatcher::stop_thread()
{
	if (!m_thread)
	{
		return;
	}

	{
		std::scoped_lock lock(m_thread_task_mutex);
		ThreadTask thread_task{};
		thread_task.task_id = ThreadTaskID::shutdown_task;

		m_thread_task_list.push_front(thread_task);
		m_cv.notify_one();
	}

	if (m_thread->joinable())
	{
		m_thread->join();
	}

	delete m_thread;
	m_thread = nullptr;
}

void PanelTimerDispatcher::thread_main()
{
	while (true)
	{
		ThreadTask thread_task{};

		{
			std::unique_lock<std::mutex> lock(m_thread_task_mutex);

			while (m_thread_task_list.empty())
			{
				m_cv.wait(lock);
			}

			if (m_thread_task_list.empty())
			{
				continue;
			}

			thread_task = m_thread_task_list.front();
			m_thread_task_list.pop_front();
		}

		switch (thread_task.task_id)
		{
		case ThreadTaskID::kill_timer_task:
			DeleteTimerQueueTimer(m_timer_queue, thread_task.handle_timer, INVALID_HANDLE_VALUE);

			{
				std::unique_lock<std::mutex> lock(m_timer_mutex);
				m_timer_map.erase(thread_task.timer_id);
			}

			break;
		case ThreadTaskID::shutdown_task:
			DeleteTimerQueueEx(m_timer_queue, INVALID_HANDLE_VALUE);
			m_timer_queue = nullptr;
			return;
		default:
			assert(0);
			break;
		}
	}
}
