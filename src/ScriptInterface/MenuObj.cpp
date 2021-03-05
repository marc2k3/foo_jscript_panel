#include "stdafx.h"
#include "MenuObj.h"

MenuObj::MenuObj(CWindow wnd_parent) : m_wnd_parent(wnd_parent), m_hMenu(CreatePopupMenu()) {}

STDMETHODIMP MenuObj::get__HMENU(HMENU* out)
{
	if (!m_hMenu || !out) return E_POINTER;

	*out = m_hMenu;
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuItem(UINT flags, UINT item_id, BSTR text)
{
	if (!m_hMenu) return E_POINTER;
	if (flags & MF_POPUP || flags & MF_OWNERDRAW || flags & MF_BITMAP) return E_INVALIDARG;

	::AppendMenu(m_hMenu, flags, item_id, text);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuSeparator()
{
	if (!m_hMenu) return E_POINTER;

	::AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendTo(IMenuObj* obj, UINT flags, BSTR text)
{
	if (!m_hMenu) return E_POINTER;

	MenuObj* parent = static_cast<MenuObj*>(obj);
	if (::AppendMenu(parent->m_hMenu, flags | MF_STRING | MF_POPUP, UINT_PTR(m_hMenu), text))
	{
		m_has_detached = true;
	}
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuItem(UINT item_id, VARIANT_BOOL check)
{
	if (!m_hMenu) return E_POINTER;

	::CheckMenuItem(m_hMenu, item_id, to_bool(check) ? MF_CHECKED : MF_UNCHECKED);
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuRadioItem(UINT first, UINT last, UINT selected)
{
	if (!m_hMenu) return E_POINTER;

	::CheckMenuRadioItem(m_hMenu, first, last, selected, MF_BYCOMMAND);
	return S_OK;
}

STDMETHODIMP MenuObj::TrackPopupMenu(int x, int y, UINT flags, UINT* out)
{
	if (!m_hMenu || !out) return E_POINTER;

	flags |= TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON;
	flags &= ~TPM_RECURSE;

	CPoint pt(x, y);
	m_wnd_parent.ClientToScreen(&pt);
	*out = ::TrackPopupMenu(m_hMenu, flags, pt.x, pt.y, 0, m_wnd_parent, nullptr);
	return S_OK;
}

void MenuObj::FinalRelease()
{
	if (!m_has_detached && m_hMenu && IsMenu(m_hMenu))
	{
		DestroyMenu(m_hMenu);
		m_hMenu = nullptr;
	}
}
