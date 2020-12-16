#pragma once
#include "ScriptInfo.h"

_COM_SMARTPTR_TYPEDEF(IActiveScriptParse, IID_IActiveScriptParse);

class ScriptHost : public ImplementCOMRefCounter<IActiveScriptSite>
{
public:
	ScriptHost(PanelWindow* panel);
	~ScriptHost();

	QI_HELPER(IActiveScriptSite)

	HRESULT Initialise();
	STDMETHODIMP GetDocVersionString(BSTR*) override;
	STDMETHODIMP GetItemInfo(LPCOLESTR name, DWORD mask, IUnknown** ppunk, ITypeInfo** ppti) override;
	STDMETHODIMP GetLCID(LCID*) override;
	STDMETHODIMP OnEnterScript() override;
	STDMETHODIMP OnLeaveScript() override;
	STDMETHODIMP OnScriptError(IActiveScriptError* err) override;
	STDMETHODIMP OnScriptTerminate(const VARIANT*, const EXCEPINFO*) override;
	STDMETHODIMP OnStateChange(SCRIPTSTATE) override;
	bool HasError();
	bool Ready();
	void InvokeCallback(CallbackID id, VARIANTARG* argv = nullptr, size_t argc = 0, VARIANT* ret = nullptr);
	void Stop();

	std::unique_ptr<ScriptInfo> m_info;

private:
	DWORD GenerateSourceContext(const std::string& path);
	HRESULT InitCallbackMap();
	HRESULT InitScriptEngine();
	HRESULT ParseScripts(IActiveScriptParsePtr& parser);

	DWORD m_last_source_context = 0;
	IActiveScriptPtr m_script_engine;
	IDispatchPtr m_script_root;
	bool m_engine_inited = false;
	bool m_has_error = false;
	pfc::com_ptr_t<Console> m_console;
	pfc::com_ptr_t<Fb> m_fb;
	pfc::com_ptr_t<Gdi> m_gdi;
	pfc::com_ptr_t<Plman> m_plman;
	pfc::com_ptr_t<Utils> m_utils;
	pfc::com_ptr_t<Window> m_window;
	PanelWindow* m_panel;
	std::unordered_map<DWORD, std::string> m_context_to_path_map;
	std::unordered_map<CallbackID, DISPID> m_callback_map;
};
