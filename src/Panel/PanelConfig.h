#pragma once
#include "PanelProperties.h"

class PanelConfig
{
public:
	PanelConfig() : m_properties(std::make_unique<PanelProperties>())
	{
		reset();
	}

	enum class EdgeStyle : char
	{
		none,
		sunken,
		grey,
	};

	DWORD get_edge_style()
	{
		switch (m_style)
		{
		case EdgeStyle::sunken: return WS_EX_CLIENTEDGE;
		case EdgeStyle::grey: return WS_EX_STATICEDGE;
		default: return 0;
		}
	}

	void set(stream_reader* reader, size_t size, abort_callback& abort)
	{
		reset();

		if (size > sizeof(size_t))
		{
			size_t ver = 0;
			try
			{
				reader->read_object_t(ver, abort);
				reader->skip_object(sizeof(bool), abort); // HACK: skip over "delay load"
				reader->skip_object(sizeof(GUID), abort); // HACK: skip over "GUID"
				reader->read_object(&m_style, sizeof(char), abort);
				m_properties->set(reader, abort);
				reader->skip_object(sizeof(bool), abort); // HACK: skip over "disable before"
				reader->skip_object(sizeof(bool), abort); // HACK: skip over "grab focus"
				reader->skip_object(sizeof(WINDOWPLACEMENT), abort); // HACK: skip over window placement
				reader->read_string(m_engine, abort); // unused but leave for downgrading
				reader->read_string(m_code, abort);
				reader->read_object_t(m_transparent, abort);
			}
			catch (...)
			{
				reset();
				FB2K_console_formatter() << jsp::component_name << ": Configuration has been corrupted. All settings have been reset.";
			}
		}
	}

	void reset()
	{
		m_code = helpers::get_resource_text(IDR_SCRIPT);
		m_engine = "Chakra";
		m_style = EdgeStyle::none;
		m_transparent = false;
	}

	void get(stream_writer* writer, abort_callback& abort) const
	{
		try
		{
			WINDOWPLACEMENT wndpl;
			writer->write_object_t(jsp::version, abort);
			writer->write_object_t(false, abort); // HACK: write this in place of "delay load"
			writer->write_object_t(pfc::guid_null, abort); // HACK: write this in place of "GUID"
			writer->write_object(&m_style, sizeof(char), abort);
			m_properties->get(writer, abort);
			writer->write_object_t(false, abort); // HACK: write this in place of "disable before"
			writer->write_object_t(true, abort); // HACK: write this in place of "grab focus"
			writer->write_object(&wndpl, sizeof(WINDOWPLACEMENT), abort); // HACK: write this in place of window placement
			writer->write_string(m_engine, abort); // unused but leave for downgrading
			writer->write_string(m_code, abort);
			writer->write_object_t(m_transparent, abort);
		}
		catch (...) {}
	}

	EdgeStyle m_style = EdgeStyle::none;
	bool m_transparent = false;
	std::unique_ptr<PanelProperties> m_properties;
	string8 m_code, m_engine;
};
