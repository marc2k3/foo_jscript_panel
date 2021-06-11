#include "stdafx.h"

ITypeLibPtr g_typelib;

namespace jsp
{
	DECLARE_COMPONENT_VERSION(component_name, component_version, component_info);
	VALIDATE_COMPONENT_FILENAME(component_dll_name);

	CAppModule app;
	GdiplusScope scope;

	extern "C" BOOL WINAPI DllMain(HINSTANCE ins, DWORD reason, LPVOID)
	{
		if (reason == DLL_PROCESS_ATTACH)
		{
			app.Init(nullptr, ins);

			std::array<wchar_t, MAX_PATH> path;
			GetModuleFileName(ins, path.data(), path.size());
			return SUCCEEDED(LoadTypeLibEx(path.data(), REGKIND_NONE, &g_typelib));
		}
		else if (reason == DLL_PROCESS_DETACH)
		{
			app.Term();
		}
		return TRUE;
	}
}
