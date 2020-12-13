#pragma once

class Profiler : public JSDispatchImpl<IProfiler>
{
protected:
	Profiler(stringp name);
	~Profiler();

public:
	STDMETHODIMP Print() override;
	STDMETHODIMP Reset() override;
	STDMETHODIMP get_Time(__int64* out) override;

private:
	Timer m_timer;
	string8 m_name;
};
