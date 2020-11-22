#pragma once

class Profiler : public JSDispatchImpl<IProfiler>
{
protected:
	Profiler(stringp name);
	~Profiler();

public:
	STDMETHODIMP Print() override;
	STDMETHODIMP Reset() override;
	STDMETHODIMP get_Time(int* out) override;

private:
	pfc::hires_timer m_timer;
	string8 m_name;
};
