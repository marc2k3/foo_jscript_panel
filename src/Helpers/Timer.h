#pragma once

class Timer
{
public:
	Timer()
	{
		reset();
	}

	int64_t query()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_ts).count();
	}

	void reset()
	{
		m_ts = std::chrono::steady_clock::now();
	}

private:
	std::chrono::steady_clock::time_point m_ts;
};
