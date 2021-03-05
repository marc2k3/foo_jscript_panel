#include "stdafx.h"
#include "PanelWindow.h"
#include "ScriptHost.h"

ScriptHost::ScriptHost(PanelWindow* panel)
	: m_panel(panel)
	, m_console(ComObjectSingleton<Console>::instance())
	, m_fb(ComObjectSingleton<Fb>::instance())
	, m_gdi(ComObjectSingleton<Gdi>::instance())
	, m_plman(ComObjectSingleton<Plman>::instance())
	, m_utils(ComObjectSingleton<Utils>::instance())
	, m_window(new ImplementCOMRefCounter<Window>(panel)) {}

ScriptHost::~ScriptHost() {}

DWORD ScriptHost::GenerateSourceContext(const std::wstring& path)
{
	m_context_to_path_map.emplace(++m_last_source_context, path);
	return m_last_source_context;
}

HRESULT ScriptHost::Initialise()
{
	m_info.update(m_panel->m_id, m_panel->m_config.m_code);

	IActiveScriptParsePtr parser;
	HRESULT hr = InitScriptEngine();
	if (SUCCEEDED(hr)) hr = m_script_engine->SetScriptSite(this);
	if (SUCCEEDED(hr)) hr = m_script_engine->QueryInterface(&parser);
	if (SUCCEEDED(hr)) hr = parser->InitNew();
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"console", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"fb", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"gdi", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"plman", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"utils", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->AddNamedItem(L"window", SCRIPTITEM_ISVISIBLE);
	if (SUCCEEDED(hr)) hr = m_script_engine->SetScriptState(SCRIPTSTATE_CONNECTED);
	if (SUCCEEDED(hr)) hr = m_script_engine->GetScriptDispatch(nullptr, &m_script_root);
	if (SUCCEEDED(hr)) hr = ParseScripts(parser);
	if (SUCCEEDED(hr)) hr = InitCallbackMap();

	m_engine_inited = SUCCEEDED(hr);
	m_has_error = FAILED(hr);
	return hr;
}

HRESULT ScriptHost::InitCallbackMap()
{
	m_callback_map.clear();
	if (!m_script_root) return E_POINTER;
	for (const auto& [id, name] : CallbackIDNames)
	{
		auto cname = const_cast<LPOLESTR>(name.data());
		DISPID dispId;
		if (SUCCEEDED(m_script_root->GetIDsOfNames(IID_NULL, &cname, 1, LOCALE_USER_DEFAULT, &dispId)))
		{
			m_callback_map.emplace(id, dispId);

			switch (id)
			{
			case CallbackID::on_char:
			case CallbackID::on_focus:
			case CallbackID::on_key_down:
			case CallbackID::on_key_up:
				m_panel->m_grabfocus = true;
				break;
			case CallbackID::on_drag_drop:
			case CallbackID::on_drag_enter:
			case CallbackID::on_drag_leave:
			case CallbackID::on_drag_over:
				m_panel->m_dragdrop = true;
				break;
			}
		}
	}
	return S_OK;
}

HRESULT ScriptHost::InitScriptEngine()
{
	static constexpr CLSID jscript9clsid = { 0x16d51579, 0xa30b, 0x4c8b,{ 0xa2, 0x76, 0x0f, 0xf4, 0xdc, 0x41, 0xe7, 0x55 } };
	static constexpr DWORD classContext = CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER;
	HRESULT hr = m_script_engine.CreateInstance(jscript9clsid, nullptr, classContext);

	if (FAILED(hr))
	{
		FB2K_console_formatter() << jsp::component_name << ": This component requires a system with IE9 or later.";
		return hr;
	}

	VARIANT scriptLangVersion;
	scriptLangVersion.vt = VT_I4;
	scriptLangVersion.lVal = SCRIPTLANGUAGEVERSION_5_8 + 1;

	IActiveScriptProperty* pActScriProp = nullptr;
	m_script_engine->QueryInterface(IID_IActiveScriptProperty, reinterpret_cast<void**>(&pActScriProp));
	pActScriProp->SetProperty(SCRIPTPROP_INVOKEVERSIONING, nullptr, &scriptLangVersion);
	pActScriProp->Release();
	return S_OK;
}

HRESULT ScriptHost::ParseScripts(IActiveScriptParsePtr& parser)
{
	HRESULT hr = S_OK;
	std::wstring path;
	string8 code;
	const size_t count = m_info.m_imports.size();
	size_t import_errors = 0;

	for (size_t i = 0; i <= count; ++i)
	{
		if (i < count) // import
		{
			path = m_info.m_imports[i];
			code = FileHelper(path).read();
			if (code.is_empty())
			{
				if (import_errors == 0)
				{
					FB2K_console_formatter() << m_info.m_build_string;
					import_errors++;
				}
				FB2K_console_formatter() << "Error: Failed to load " << path.data();
			}
		}
		else // main
		{
			path = L"<main>";
			code = m_panel->m_config.m_code;
		}

		const DWORD source_context = GenerateSourceContext(path);
		hr = parser->ParseScriptText(to_wide(code).data(), nullptr, nullptr, nullptr, source_context, 0, SCRIPTTEXT_HOSTMANAGESSOURCE | SCRIPTTEXT_ISVISIBLE, nullptr, nullptr);
		if (FAILED(hr)) break;
	}
	return hr;
}

STDMETHODIMP ScriptHost::GetDocVersionString(BSTR*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::GetItemInfo(LPCOLESTR name, DWORD mask, IUnknown** ppunk, ITypeInfo** ppti)
{
	if (ppti) *ppti = nullptr;
	if (ppunk) *ppunk = nullptr;
	if (mask & SCRIPTINFO_IUNKNOWN)
	{
		if (!name) return E_INVALIDARG;
		if (!ppunk) return E_POINTER;

		if (wcscmp(name, L"console") == 0)
		{
			(*ppunk) = m_console.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"fb") == 0)
		{
			(*ppunk) = m_fb.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"gdi") == 0)
		{
			(*ppunk) = m_gdi.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"plman") == 0)
		{
			(*ppunk) = m_plman.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"utils") == 0)
		{
			(*ppunk) = m_utils.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
		else if (wcscmp(name, L"window") == 0)
		{
			(*ppunk) = m_window.get_ptr();
			(*ppunk)->AddRef();
			return S_OK;
		}
	}
	return TYPE_E_ELEMENTNOTFOUND;
}

STDMETHODIMP ScriptHost::GetLCID(LCID*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::OnEnterScript()
{
	return S_OK;
}

STDMETHODIMP ScriptHost::OnLeaveScript()
{
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptError(IActiveScriptError* err)
{
	if (!err) return E_POINTER;

	m_engine_inited = false;
	m_has_error = true;

	DWORD ctx = 0;
	EXCEPINFO excep = { 0 };
	LONG charpos = 0;
	ULONG line = 0;
	_bstr_t sourceline;
	pfc::string_formatter formatter;

	formatter << m_info.m_build_string << "\n";

	if (SUCCEEDED(err->GetExceptionInfo(&excep)))
	{
		if (excep.pfnDeferredFillIn)
		{
			(*excep.pfnDeferredFillIn)(&excep);
		}

		if (excep.bstrSource)
		{
			formatter << from_wide(excep.bstrSource) << ":\n";
			SysFreeString(excep.bstrSource);
		}

		if (excep.bstrDescription)
		{
			formatter << from_wide(excep.bstrDescription) << "\n";
			SysFreeString(excep.bstrDescription);
		}
		else
		{
			string8 errorMessage;
			if (uFormatSystemErrorMessage(errorMessage, excep.scode))
			{
				formatter << errorMessage << "\n";
			}
			else
			{
				formatter << "Unknown error code: 0x" << pfc::format_hex_lowercase(to_uint(excep.scode)) << "\n";
			}
		}

		if (excep.bstrHelpFile)
		{
			SysFreeString(excep.bstrHelpFile);
		}
	}

	if (SUCCEEDED(err->GetSourcePosition(&ctx, &line, &charpos)))
	{
		if (m_context_to_path_map.contains(ctx))
		{
			formatter << "File: " << m_context_to_path_map.at(ctx).data() << "\n";
		}
		formatter << "Line: " << (line + 1) << ", Col: " << (charpos + 1) << "\n";
	}

	if (SUCCEEDED(err->GetSourceLineText(sourceline.GetAddress())))
	{
		formatter << from_wide(sourceline.GetBSTR());
	}

	FB2K_console_formatter() << formatter;

	fb2k::inMainThread([=]()
		{
			popup_message::g_show(formatter, PFC_string_formatter() << jsp::component_name << " v" << jsp::component_version);
		});

	if (m_script_engine) m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);

	MessageBeep(MB_ICONASTERISK);

	m_panel->repaint();
	m_panel->unload_script();
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptTerminate(const VARIANT*, const EXCEPINFO*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::OnStateChange(SCRIPTSTATE)
{
	return E_NOTIMPL;
}

bool ScriptHost::HasError()
{
	return m_has_error;
}

bool ScriptHost::InvokeMouseRBtnUp(VariantArgs& args)
{
	bool ret = false;
	const CallbackID id = CallbackID::on_mouse_rbtn_up;
	if (Ready() && m_callback_map.contains(id))
	{
		DISPPARAMS params = { args.data(), nullptr, args.size(), 0 };
		_variant_t result;
		m_script_root->Invoke(m_callback_map.at(id), IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &result, nullptr, nullptr);
		if (SUCCEEDED(VariantChangeType(&result, &result, 0, VT_BOOL)))
		{
			ret = to_bool(result.boolVal);
		}
	}
	return ret;
}

bool ScriptHost::Ready()
{
	return m_script_root && m_engine_inited && m_script_engine;
}

void ScriptHost::InvokeCallback(CallbackID id)
{
	if (Ready() && m_callback_map.contains(id))
	{
		DISPPARAMS params{};
		m_script_root->Invoke(m_callback_map.at(id), IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}
}

void ScriptHost::InvokeCallback(CallbackID id, VariantArgs& args)
{
	if (Ready() && m_callback_map.contains(id))
	{
		DISPPARAMS params = { args.data(), nullptr, args.size(), 0 };
		m_script_root->Invoke(m_callback_map.at(id), IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}
}

void ScriptHost::Stop()
{
	if (Ready())
	{
		IActiveScriptGarbageCollector* gc = nullptr;
		if (SUCCEEDED(m_script_engine->QueryInterface(IID_IActiveScriptGarbageCollector, reinterpret_cast<void**>(&gc))))
		{
			gc->CollectGarbage(SCRIPTGCTYPE_EXHAUSTIVE);
			gc->Release();
		}

		m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		m_script_engine->SetScriptState(SCRIPTSTATE_CLOSED);
		m_script_engine->Close();
		m_engine_inited = false;
	}

	m_context_to_path_map.clear();
	m_callback_map.clear();

	if (m_script_engine)
	{
		m_script_engine.Release();
	}

	if (m_script_root)
	{
		m_script_root.Release();
	}
}
