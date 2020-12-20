#include "stdafx.h"
#include <Scintilla.h>

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
			std::array<wchar_t, MAX_PATH> path;
			GetModuleFileName(ins, path.data(), path.size());
			LoadTypeLibEx(path.data(), REGKIND_NONE, &g_typelib);

			Scintilla_RegisterClasses(ins);
			app.Init(nullptr, ins);
		}
		else if (reason == DLL_PROCESS_DETACH)
		{
			app.Term();
			Scintilla_ReleaseResources();
		}
		return TRUE;
	}
}
