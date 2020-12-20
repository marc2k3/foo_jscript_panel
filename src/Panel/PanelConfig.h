#pragma once
#include "PanelProperties.h"

class PanelConfig
{
public:
	PanelConfig()
	{
		reset();
	}

	DWORD get_edge_style()
	{
		switch (m_style)
		{
		case 1: return WS_EX_CLIENTEDGE; // sunken
		case 2: return WS_EX_STATICEDGE; // grey
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
				if (ver < 2500)
				{
					reader->skip_object(sizeof(bool), abort);
					reader->skip_object(sizeof(GUID), abort);

					char style = 0;
					reader->read_object_t(style, abort);
					m_style = style;

					m_properties.set(reader, abort);
					reader->skip_object(sizeof(bool), abort);
					reader->skip_object(sizeof(bool), abort);
					reader->skip_object(sizeof(WINDOWPLACEMENT), abort);

					string8 engine;
					reader->read_string(engine, abort);

					reader->read_string(m_code, abort);
					reader->read_object_t(m_transparent, abort);
				}
				else
				{
					m_properties.set(reader, abort);
					reader->read_string(m_code, abort);
					reader->read_object_t(m_transparent, abort);
					reader->read_object_t(m_style, abort);
				}
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
		m_code = Component::get_resource_text(IDR_SCRIPT);
		m_style = 0;
		m_transparent = false;
	}

	void get(stream_writer* writer, abort_callback& abort) const
	{
		try
		{
			writer->write_object_t(jsp::version, abort);
			m_properties.get(writer, abort);
			writer->write_string(m_code, abort);
			writer->write_object_t(m_transparent, abort);
			writer->write_object_t(m_style, abort);
		}
		catch (...) {}
	}

	PanelProperties m_properties;
	bool m_transparent = false;
	int m_style = 0;
	string8 m_code;
};
