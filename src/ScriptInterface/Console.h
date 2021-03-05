#pragma once

class Console : public JSDispatchImpl<IConsole>
{
public:
	STDMETHODIMP Log(SAFEARRAY* sa) override;
};
