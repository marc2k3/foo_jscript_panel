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
			return SUCCEEDED(LoadTypeLibEx(path.data(), REGKIND_NONE, &g_typelib));
		}
		return TRUE;
	}

	class InitQuitJSP : public initquit
	{
	public:
		void on_init() override
		{
			HINSTANCE ins = core_api::get_my_instance();
			app.Init(nullptr, ins);
			Scintilla_RegisterClasses(ins);
		}

		void on_quit() override
		{
			Scintilla_ReleaseResources();
			app.Term();
		}
	};

	FB2K_SERVICE_FACTORY(InitQuitJSP)
}
