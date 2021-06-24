#pragma once
#define SDK_STRING_(x) #x
#define SDK_STRING(x) SDK_STRING_(x)

namespace jsp
{
	static constexpr const char* component_name = "JScript Panel";
	static constexpr const char* component_version = "2.6.0.2";
	static constexpr const char* component_dll_name = "foo_jscript_panel.dll";
	static constexpr const char* component_info = "Copyright (C) 2015-2021 marc2003\n"
		"Based on WSH Panel Mod by T.P. Wang\n"
		"Thanks for the contributions by TheQwertiest and kbuffington\n\n"
		"Build: " __TIME__ ", " __DATE__ "\n\n"
		"foobar2000 SDK: " SDK_STRING(FOOBAR2000_SDK_VERSION) "\n"
		"Columns UI SDK: " UI_EXTENSION_VERSION;
	static constexpr size_t uwm_refreshbk = WM_USER + 1;
	static constexpr size_t uwm_timer = WM_USER + 2;
	static constexpr size_t uwm_unload = WM_USER + 3;
	static constexpr size_t version = 2602;
}
