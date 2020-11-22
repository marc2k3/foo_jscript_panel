#pragma once

class Console : public JSDispatchImpl<IConsole>
{
protected:
	Console();
	~Console();

public:
	STDMETHODIMP Log(SAFEARRAY* sa) override;
};
