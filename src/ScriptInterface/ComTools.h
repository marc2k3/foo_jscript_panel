#pragma once

extern pfc::com_ptr_t<ITypeLib> g_type_lib;
struct IDisposable;

#define QI_HELPER_BEGIN(first) \
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override \
	{ \
		if (!ppv) return E_POINTER; \
		IUnknown* temp = nullptr; \
		if (riid == __uuidof(IUnknown)) temp = static_cast<IUnknown*>(static_cast<first*>(this)); \
		else if (riid == __uuidof(first)) temp = static_cast<first*>(this);

#define QI_HELPER_ENTRY(entry) \
		else if (riid == __uuidof(entry)) temp = static_cast<entry*>(this);

#define QI_HELPER_END() \
		else { *ppv = nullptr; return E_NOINTERFACE; } \
		temp->AddRef(); \
		*ppv = temp; \
		return S_OK; \
	}

#define QI_HELPER(what) QI_HELPER_BEGIN(what) QI_HELPER_END()
#define QI_HELPER_DISPATCH(what) QI_HELPER_BEGIN(IDispatch) QI_HELPER_ENTRY(what) QI_HELPER_END()
#define QI_HELPER_DISPOSABLE(what) QI_HELPER_BEGIN(IDispatch) QI_HELPER_ENTRY(IDisposable) QI_HELPER_ENTRY(what) QI_HELPER_END()

struct TypeInfoCache
{
	pfc::com_ptr_t<ITypeInfo> m_type_info;
	std::unordered_map<ULONG, DISPID> m_cache;
};

template <class T>
class JSDispatchImplBase : public T
{
protected:
	JSDispatchImplBase<T>()
	{
		if (g_type_info_cache.m_type_info.is_empty())
		{
			g_type_lib->GetTypeInfoOfGuid(__uuidof(T), g_type_info_cache.m_type_info.receive_ptr());
		}
	}

	virtual ~JSDispatchImplBase<T>() {}

	virtual void FinalRelease() {}

	static TypeInfoCache g_type_info_cache;

public:
	STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR** names, UINT cNames, LCID, DISPID* dispids) override
	{
		if (!dispids) return E_POINTER;
		for (uint32_t i = 0; i < cNames; ++i)
		{
			const ULONG hash = LHashValOfName(LANG_NEUTRAL, names[i]);
			const auto& it = g_type_info_cache.m_cache.find(hash);
			if (it != g_type_info_cache.m_cache.end())
			{
				dispids[i] = it->second;
			}
			else
			{
				const HRESULT hr = g_type_info_cache.m_type_info->GetIDsOfNames(&names[i], 1, &dispids[i]);
				if (FAILED(hr)) return hr;
				g_type_info_cache.m_cache.emplace(hash, dispids[i]);
			}
		}
		return S_OK;
	}

	STDMETHODIMP GetTypeInfo(UINT i, LCID, ITypeInfo** out) override
	{
		if (!out) return E_POINTER;
		if (i != 0) return DISP_E_BADINDEX;
		g_type_info_cache.m_type_info->AddRef();
		*out = g_type_info_cache.m_type_info.get_ptr();
		return S_OK;
	}

	STDMETHODIMP GetTypeInfoCount(UINT* n) override
	{
		if (!n) return E_POINTER;
		*n = 1;
		return S_OK;
	}

	STDMETHODIMP Invoke(DISPID dispid, REFIID, LCID, WORD flags, DISPPARAMS* params, VARIANT* result, EXCEPINFO* excep, UINT* err) override
	{
		const HRESULT hr = g_type_info_cache.m_type_info->Invoke(this, dispid, flags, params, result, excep, err);
		PFC_ASSERT(hr != RPC_E_WRONG_THREAD);
		return hr;
	}
};

template <class T>
FOOGUIDDECL TypeInfoCache JSDispatchImplBase<T>::g_type_info_cache;

template <class T>
class JSDispatchImpl : public JSDispatchImplBase<T>
{
protected:
	JSDispatchImpl<T>() {}
	~JSDispatchImpl<T>() {}

public:
	QI_HELPER_DISPATCH(T)
};

template <class T>
class JSDisposableImpl : public JSDispatchImplBase<T>
{
protected:
	JSDisposableImpl<T>() {}
	~JSDisposableImpl<T>() {}

public:
	QI_HELPER_DISPOSABLE(T)

	STDMETHODIMP Dispose() override
	{
		this->FinalRelease();
		return S_OK;
	}
};

template <typename _Base>
class ComObjectImpl : public _Base
{
public:
	template <typename ... Args>
	ComObjectImpl(Args && ... args) : _Base(std::forward<Args>(args)...) {}

	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return ++m_counter;
	}

	ULONG STDMETHODCALLTYPE Release() override
	{
		const long n = --m_counter;
		if (n == 0)
		{
			this->FinalRelease();
			delete this;
		}
		return n;
	}

private:
	~ComObjectImpl() {}

	pfc::refcounter m_counter = 1;
};

template <class T>
class ComObjectSingleton
{
public:
	static T* instance()
	{
		if (instance_.is_empty())
		{
			instance_ = new ImplementCOMRefCounter<T>();
		}

		return reinterpret_cast<T*>(instance_.get_ptr());
	}

private:
	static pfc::com_ptr_t<IDispatch> instance_;

	PFC_CLASS_NOT_COPYABLE_EX(ComObjectSingleton)
};

template <class T>
FOOGUIDDECL pfc::com_ptr_t<IDispatch> ComObjectSingleton<T>::instance_;
