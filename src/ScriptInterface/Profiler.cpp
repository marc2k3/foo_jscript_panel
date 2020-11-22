#include "stdafx.h"
#include "Profiler.h"

Profiler::Profiler(stringp name) : m_name(name)
{
	m_timer.start();
}

Profiler::~Profiler() {}

STDMETHODIMP Profiler::Print()
{
	FB2K_console_formatter() << "FbProfiler (" << m_name << "): " << static_cast<int>(m_timer.query() * 1000) << " ms";
	return S_OK;
}

STDMETHODIMP Profiler::Reset()
{
	m_timer.start();
	return S_OK;
}

STDMETHODIMP Profiler::get_Time(int* out)
{
	if (!out) return E_POINTER;

	*out = static_cast<int>(m_timer.query() * 1000);
	return S_OK;
}
