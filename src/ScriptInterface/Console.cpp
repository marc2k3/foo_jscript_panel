#include "stdafx.h"
#include "Console.h"

Console::Console() {}
Console::~Console() {}

STDMETHODIMP Console::Log(SAFEARRAY* sa)
{
	string8 str;
	LONG count = 0;
	if (FAILED(SafeArrayGetUBound(sa, 1, &count))) return E_INVALIDARG;

	for (LONG i = 0; i <= count; ++i)
	{
		_variant_t var;
		if (FAILED(SafeArrayGetElement(sa, &i, &var))) continue;
		if (FAILED(VariantChangeType(&var, &var, VARIANT_ALPHABOOL, VT_BSTR))) continue;
		const string8 tmp = from_wide(var.bstrVal);
		if (tmp.get_length())
		{
			if (str.get_length()) str.add_byte(' ');
			str.add_string(tmp);
		}
	}
	FB2K_console_formatter() << str;
	return S_OK;
}
