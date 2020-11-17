#include "stdafx.h"
#include "Tooltip.h"

Tooltip::Tooltip(CWindow tooltip, CWindow parent) : m_tooltip(tooltip), m_parent(parent)
{
	memset(&m_ti, 0, sizeof(m_ti));
	m_ti.cbSize = sizeof(m_ti);
	m_ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
	m_ti.hinst = core_api::get_my_instance();
	m_ti.hwnd = m_parent;
	m_ti.uId = reinterpret_cast<UINT_PTR>(m_parent.m_hWnd);
	m_ti.lpszText = m_tip_buffer.data();

	m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_tooltip.SendMessage(TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&m_ti));
	m_tooltip.SendMessage(TTM_ACTIVATE);
}

Tooltip::~Tooltip() {}

STDMETHODIMP Tooltip::Activate()
{
	m_tooltip.SendMessage(TTM_ACTIVATE, TRUE);
	return S_OK;
}

STDMETHODIMP Tooltip::Deactivate()
{
	m_tooltip.SendMessage(TTM_ACTIVATE);
	return S_OK;
}

STDMETHODIMP Tooltip::GetDelayTime(int type, int* p)
{
	if (!p) return E_POINTER;
	if (type < TTDT_AUTOMATIC || type > TTDT_INITIAL) return E_INVALIDARG;

	*p = m_tooltip.SendMessage(TTM_GETDELAYTIME, type);
	return S_OK;
}

STDMETHODIMP Tooltip::SetDelayTime(int type, int time)
{
	if (type < TTDT_AUTOMATIC || type > TTDT_INITIAL) return E_INVALIDARG;

	m_tooltip.SendMessage(TTM_SETDELAYTIME, type, time);
	return S_OK;
}

STDMETHODIMP Tooltip::SetMaxWidth(int width)
{
	m_tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, width);
	return S_OK;
}

STDMETHODIMP Tooltip::TrackPosition(int x, int y)
{
	CPoint pt(x, y);
	m_parent.ClientToScreen(&pt);
	m_tooltip.SendMessage(TTM_TRACKPOSITION, 0, MAKELONG(pt.x, pt.y));
	return S_OK;
}

STDMETHODIMP Tooltip::get_Text(BSTR* p)
{
	if (!p) return E_POINTER;

	*p = SysAllocString(m_tip_buffer.data());
	return S_OK;
}

STDMETHODIMP Tooltip::put_Text(BSTR text)
{
	m_tip_buffer = text;
	m_ti.lpszText = m_tip_buffer.data();
	m_tooltip.SendMessage(TTM_SETTOOLINFO, 0, reinterpret_cast<LPARAM>(&m_ti));
	return S_OK;
}

STDMETHODIMP Tooltip::put_TrackActivate(VARIANT_BOOL activate)
{
	if (to_bool(activate))
	{
		m_ti.uFlags |= TTF_TRACK | TTF_ABSOLUTE;
	}
	else
	{
		m_ti.uFlags &= ~(TTF_TRACK | TTF_ABSOLUTE);
	}

	m_tooltip.SendMessage(TTM_TRACKACTIVATE, to_bool(activate) ? TRUE : FALSE, reinterpret_cast<LPARAM>(&m_ti));
	return S_OK;
}
