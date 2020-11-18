#include "stdafx.h"
#include "Console.h"

Console::Console() {}
Console::~Console() {}

STDMETHODIMP Console::Log(SAFEARRAY* p)
{
	string8 str;
	LONG count = 0;
	if (FAILED(SafeArrayGetUBound(p, 1, &count))) return E_INVALIDARG;

	for (LONG i = 0; i <= count; ++i)
	{
		_variant_t var;
		if (FAILED(SafeArrayGetElement(p, &i, &var))) continue;
		if (FAILED(VariantChangeType(&var, &var, VARIANT_ALPHABOOL, VT_BSTR))) continue;
		auto tmp = string_utf8_from_wide(var.bstrVal);
		if (tmp.length())
		{
			if (str.get_length()) str.add_byte(' ');
			str.add_string(tmp);
		}
	}
	FB2K_console_formatter() << str;
	return S_OK;
}