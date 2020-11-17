#pragma once

class PanelProperties
{
public:
	using PropertyMap = std::map<pfc::string_simple, _variant_t, StricmpAscii>;

	static int g_sizeof(VARTYPE vt);
	static void g_get(const PropertyMap& data, stream_writer* writer, abort_callback& abort) throw();
	static void g_set(PropertyMap& data, stream_reader* reader, abort_callback& abort) throw();

	bool get_property(const char* key, VARIANT& out);
	void get(stream_writer* writer, abort_callback& abort) const throw();
	void set(stream_reader* reader, abort_callback& abort) throw();
	void set_property(const char* key, const VARIANT& val);

	PropertyMap m_map;
};
