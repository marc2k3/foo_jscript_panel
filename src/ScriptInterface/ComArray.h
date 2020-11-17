#pragma once

class ComArrayReader
{
public:
	ComArrayReader() {}
	~ComArrayReader() {}

	bool convert(const VARIANT& v, pfc::bit_array_bittable& out)
	{
		if (!init(v)) return false;
		if (m_data.empty()) out.resize(0);

		for (auto& item : m_data)
		{
			if (!change_type(item, VT_UI4)) return false;
			out.set(item.ulVal, true);
		}
		return true;
	}

	bool convert(const VARIANT& v, pfc::string_list_impl& out)
	{
		if (!init(v)) return false;

		for (auto& item : m_data)
		{
			if (!change_type(item, VT_BSTR)) return false;
			out.add_item(string_utf8_from_wide(item.bstrVal));
		}
		return true;
	}

	bool convert(const VARIANT& v, std::vector<Gdiplus::PointF>& out)
	{
		if (!init(v)) return false;
		if (m_data.size() % 2 != 0) return false;

		const size_t count = m_data.size() >> 1;
		out.resize(count);

		for (size_t i = 0; i < count; ++i)
		{
			_variant_t varX = m_data[i * 2];
			_variant_t varY = m_data[(i * 2) + 1];
			if (!change_type(varX, VT_R4)) return false;
			if (!change_type(varY, VT_R4)) return false;
			out[i] = { varX.fltVal, varY.fltVal };
		}
		return true;
	}

private:
	bool change_type(VARIANT& var, VARTYPE vt)
	{
		return SUCCEEDED(VariantChangeType(&var, &var, 0, vt));
	}

	bool init(const VARIANT& v)
	{
		if (v.vt != VT_DISPATCH || !v.pdispVal) return false;

		IDispatch* pdisp = v.pdispVal;
		DISPID dispId;
		DISPPARAMS params{};
		_variant_t result;

		auto name = const_cast<LPOLESTR>(L"length");

		if (FAILED(pdisp->GetIDsOfNames(IID_NULL, &name, 1, LOCALE_USER_DEFAULT, &dispId))) return false;
		if (FAILED(pdisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr))) return false;
		if (!change_type(result, VT_UI4)) return false;

		const size_t count = result.ulVal;
		m_data.resize(count);

		for (size_t i = 0; i < count; ++i)
		{
			std::wstring num = std::to_wstring(i);
			name = const_cast<LPOLESTR>(num.data());

			if (FAILED(pdisp->GetIDsOfNames(IID_NULL, &name, 1, LOCALE_USER_DEFAULT, &dispId))) return false;
			if (FAILED(pdisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, nullptr, nullptr))) return false;
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

	bool create(LONG count)
	{
		m_count = count;
		m_psa = SafeArrayCreateVector(VT_VARIANT, 0, count);
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
		if (SUCCEEDED(SafeArrayPutElement(m_psa, &idx, &var)))
		{
			return true;
		}
		reset();
		return false;
	}

private:
	void reset()
	{
		m_count = 0;
		if (m_psa)
		{
			SafeArrayDestroy(m_psa);
			m_psa = nullptr;
		}
	}

	LONG m_count = 0;
	SAFEARRAY* m_psa = nullptr;
};
