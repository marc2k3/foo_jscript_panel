#pragma once

class PanelProperties
{
public:
	using PropertyData = std::map<string8, _variant_t>;

	static int g_sizeof(VARTYPE vt);
	static void g_get(stream_writer* writer, const PropertyData& data, abort_callback& abort);
	static void g_set(stream_reader* reader, PropertyData& data, abort_callback& abort);

	bool get_property(jstring key, VARIANT& out);
	void get(stream_writer* writer, abort_callback& abort) const;
	void set(stream_reader* reader, abort_callback& abort);
	void set_property(jstring key, const VARIANT& val);

	PropertyData m_data;
};
