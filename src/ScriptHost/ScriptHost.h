#pragma once
#include "ScriptInfo.h"

class ScriptHost : public ImplementCOMRefCounter<IActiveScriptSite>
{
public:
	ScriptHost(PanelWindow* panel);

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
	bool InvokeMouseRBtnUp(VariantArgs& args);
	bool Ready();
	void InvokeCallback(CallbackID id);
	void InvokeCallback(CallbackID id, VariantArgs& args);
	void Stop();

	ScriptInfo m_info;

private:
	using CallbackMap = std::unordered_map<CallbackID, DISPID>;
	using ContextPathMap = std::unordered_map<DWORD, std::wstring>;

	DWORD GenerateSourceContext(const std::wstring& path);
	HRESULT InitCallbackMap();
	HRESULT InitScriptEngine();
	HRESULT ParseScripts();

	CallbackMap m_callback_map;
	ContextPathMap m_context_to_path_map;
	DWORD m_last_source_context = 0;
	PanelWindow* m_panel;
	bool m_engine_inited = false;
	bool m_has_error = false;
	pfc::com_ptr_t<Console> m_console;
	pfc::com_ptr_t<Fb> m_fb;
	pfc::com_ptr_t<Gdi> m_gdi;
	pfc::com_ptr_t<IActiveScript> m_script_engine;
	pfc::com_ptr_t<IActiveScriptParse32> m_parser;
	pfc::com_ptr_t<IDispatch> m_script_root;
	pfc::com_ptr_t<Plman> m_plman;
	pfc::com_ptr_t<Utils> m_utils;
	pfc::com_ptr_t<Window> m_window;
};
