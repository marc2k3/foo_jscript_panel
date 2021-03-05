#pragma once

class Profiler : public JSDispatchImpl<IProfiler>
{
public:
	Profiler(const std::wstring& name);

	STDMETHODIMP Print() override;
	STDMETHODIMP Reset() override;
	STDMETHODIMP get_Time(__int64* out) override;

private:
	Timer m_timer;
	string8 m_name;
};
