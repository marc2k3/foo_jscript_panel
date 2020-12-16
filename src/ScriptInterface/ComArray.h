#pragma once

class ComArrayReader
{
public:
	ComArrayReader() {}
	~ComArrayReader() {}

	bool convert(const VARIANT& v, pfc::bit_array_bittable& out)
	{
		if (!init(v, VT_UI4)) return false;
		if (m_data.empty()) out.resize(0);

		for (auto& item : m_data)
		{
			out.set(item.ulVal, true);
		}
		return true;
	}

	bool convert(const VARIANT& v, pfc::string_list_impl& out)
	{
		if (!init(v, VT_BSTR)) return false;

		for (auto& item : m_data)
		{
			out.add_item(from_wide(item.bstrVal));
		}
		return true;
	}

	bool convert(const VARIANT& v, std::vector<Gdiplus::PointF>& out)
	{
		if (!init(v, VT_R4)) return false;
		if (m_data.size() % 2 != 0) return false;

		const size_t count = m_data.size() >> 1;
		out.resize(count);

		for (size_t i = 0; i < count; ++i)
		{
			out[i] = { m_data[i * 2].fltVal, m_data[(i * 2) + 1].fltVal };
		}
		return true;
	}

private:
	bool get_property(IDispatch* pdisp, const std::wstring& name, VARTYPE vt, VARIANT& result)
	{
		auto cname = const_cast<LPOLESTR>(name.data());
		DISPID dispId;
		DISPPARAMS params{};
		if (FAILED(pdisp->GetIDsOfNames(IID_NULL, &cname, 1, LOCALE_USER_DEFAULT, &dispId))) return false;
		if (FAILED(pdisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr))) return false;
		if (FAILED(VariantChangeType(&result, &result, 0, vt))) return false;
		return true;
	}

	bool init(const VARIANT& v, VARTYPE vt)
	{
		if (v.vt != VT_DISPATCH || !v.pdispVal) return false;

		IDispatch* pdisp = v.pdispVal;
		_variant_t result;
		if (!get_property(pdisp, L"length", VT_UI4, result)) return false;
		const size_t count = result.ulVal;
		m_data.resize(count);

		for (size_t i = 0; i < count; ++i)
		{
			if (!get_property(pdisp, std::to_wstring(i), vt, result)) return false;
			m_data[i] = result;
		}
		return true;
	}

	std::vector<_variant_t> m_data;
};

class ComArrayWriter
{
public:
	ComArrayWriter() {}
	~ComArrayWriter() {}

	SAFEARRAY* get_ptr()
	{
		return m_psa;
	}

	bool create(ULONG count)
	{
		m_psa = SafeArrayCreateVector(VT_VARIANT, 0L, count);
		return m_psa != nullptr;
	}

	bool put_item(LONG idx, stringp str)
	{
		_variant_t var;
		var.vt = VT_BSTR;
		var.bstrVal = to_bstr(str);
		return put_item(idx, var);
	}

	bool put_item(LONG idx, VARIANT& var)
	{
		if (!m_psa) return false;
		if (SUCCEEDED(SafeArrayPutElement(m_psa, &idx, &var))) return true;
		SafeArrayDestroy(m_psa);
		m_psa = nullptr;
		return false;
	}

private:
	SAFEARRAY* m_psa = nullptr;
};
