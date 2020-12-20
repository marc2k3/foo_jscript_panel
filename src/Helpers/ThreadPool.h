#pragma once

class SimpleThreadTask
{
public:
	virtual void run() = 0;
};

class SimpleThreadPool
{
public:
	SimpleThreadPool()
	{
		max_workers = std::max(1U, std::thread::hardware_concurrency());
		empty_worker = CreateEvent(nullptr, TRUE, TRUE, nullptr);
		exiting = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		have_task = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	}

	~SimpleThreadPool()
	{
		CloseHandle(empty_worker);
		CloseHandle(exiting);
		CloseHandle(have_task);
	}

	using Task = std::unique_ptr<SimpleThreadTask>;
	using TaskQueue = std::queue<Task>;

	static SimpleThreadPool& instance()
	{
		static SimpleThreadPool instance;
		return instance;
	}

	Task get_task()
	{
		std::scoped_lock lock(mutex);
		if (task_queue.empty()) return nullptr;

		Task task;
		task.swap(task_queue.front());
		task_queue.pop();

		if (task_queue.empty())
		{
			task_queue_empty = true;
			ResetEvent(have_task);
		}
		return std::move(task);
	}

	void add_task(Task task)
	{
		if (WaitForSingleObject(exiting, 0) == WAIT_OBJECT_0)
			return;

		std::scoped_lock lock(mutex);
		task_queue.emplace(std::move(task));
		task_queue_empty = false;
		SetEvent(have_task);

		if (workers.size() < max_workers)
		{
			add_worker();
		}
	}

	void add_worker()
	{
		ResetEvent(empty_worker);

		auto t = std::thread([]()
			{
				uint64_t last_tick = GetTickCount64();

				while (WaitForSingleObject(SimpleThreadPool::instance().exiting, 0) == WAIT_TIMEOUT)
				{
					if (WaitForSingleObject(SimpleThreadPool::instance().have_task, 1000) == WAIT_OBJECT_0)
					{
						auto task = SimpleThreadPool::instance().get_task();

						if (task)
						{
							task->run();
							last_tick = GetTickCount64();
							continue;
						}
					}

					if (GetTickCount64() - last_tick >= 10000 && SimpleThreadPool::instance().task_queue_empty)
					{
						break;
					}
				}

				SimpleThreadPool::instance().remove_worker();
			});

		workers.insert(t.get_id());
		t.detach();
	}

	void exit()
	{
		SetEvent(exiting);

		while (WaitForSingleObject(empty_worker, 0) == WAIT_TIMEOUT)
		{
			MSG msg;

			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		std::scoped_lock lock(mutex);
		task_queue = TaskQueue();
	}

	void remove_worker()
	{
		std::scoped_lock lock(mutex);
		workers.erase(std::this_thread::get_id());
		if (workers.empty()) SetEvent(empty_worker);
	}

	HANDLE empty_worker;
	HANDLE exiting;
	HANDLE have_task;
	std::atomic_bool task_queue_empty = true;

private:
	TaskQueue task_queue;
	std::mutex mutex;
	std::set<std::thread::id> workers;
	uint32_t max_workers = 0;

	PFC_CLASS_NOT_COPYABLE_EX(SimpleThreadPool)
};

namespace
{
	class InitQuit : public initquit
	{
	public:
		void on_quit() override
		{
			SimpleThreadPool::instance().exit();
		}
	};

	FB2K_SERVICE_FACTORY(InitQuit);
}
