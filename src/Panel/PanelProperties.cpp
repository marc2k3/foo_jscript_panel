#include "stdafx.h"
#include "PanelProperties.h"

bool PanelProperties::get_property(const char* key, VARIANT& out)
{
	if (m_data.count(key))
	{
		return SUCCEEDED(VariantCopy(&out, &m_data.at(key)));
	}
	return false;
}

int PanelProperties::g_sizeof(VARTYPE vt)
{
	switch (vt)
	{
	case VT_BSTR:
		return 0;
	case VT_I1:
	case VT_UI1:
		return sizeof(BYTE);
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		return sizeof(short);
	case VT_I4:
	case VT_UI4:
	case VT_R4:
		return sizeof(LONG);
	case VT_I8:
	case VT_UI8:
		return sizeof(LONGLONG);
	case VT_R8:
		return sizeof(double);
	default:
		return -1;
	}
}

void PanelProperties::g_get(stream_writer* writer, const PropertyData& data, abort_callback& abort) throw()
{
	try
	{
		writer->write_lendian_t(data.size(), abort);

		for (const auto& [key, value] : data)
		{
			writer->write_string(key, abort);
			writer->write_lendian_t(value.vt, abort);
			const int cbWrite = g_sizeof(value.vt);

			if (cbWrite > 0)
			{
				writer->write(&value.bVal, cbWrite, abort);
			}
			else if (cbWrite == 0)
			{
				writer->write_string(from_wide(value.bstrVal), abort);
			}
		}
	}
	catch (...) {}
}

void PanelProperties::g_set(stream_reader* reader, PropertyData& data, abort_callback& abort) throw()
{
	data.clear();

	try
	{
		size_t count;
		reader->read_lendian_t(count, abort);

		for (size_t i = 0; i < count; ++i)
		{
			string8 key;
			VARTYPE vt;
			reader->read_string(key, abort);
			reader->read_lendian_t(vt, abort);

			const int cbRead = g_sizeof(vt);

			_variant_t val;
			val.vt = vt;

			if (cbRead > 0)
			{
				reader->read(&val.bVal, cbRead, abort);
			}
			else
			{
				string8 str;
				reader->read_string(str, abort);
				val.bstrVal = to_bstr(str);
			}

			data.emplace(key, val);
		}
	}
	catch (...) {}
}

void PanelProperties::get(stream_writer* writer, abort_callback& abort) const throw()
{
	g_get(writer, m_data, abort);
}

void PanelProperties::set(stream_reader* reader, abort_callback& abort) throw()
{
	g_set(reader, m_data, abort);
}

void PanelProperties::set_property(const char* key, const VARIANT& val)
{
	if (g_sizeof(val.vt) != -1)
	{
		m_data[key] = val;
	}
	else
	{
		m_data.erase(key);
	}
}
