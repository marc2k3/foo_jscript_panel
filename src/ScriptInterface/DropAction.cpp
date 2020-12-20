#include "stdafx.h"
#include "DropAction.h"

DropAction::DropAction() {}
DropAction::~DropAction() {}

STDMETHODIMP DropAction::get_Effect(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_effect;
	return S_OK;
}

STDMETHODIMP DropAction::put_Base(UINT base)
{
	m_base = base;
	return S_OK;
}

STDMETHODIMP DropAction::put_Effect(UINT effect)
{
	m_effect = effect;
	return S_OK;
}

STDMETHODIMP DropAction::put_Playlist(UINT id)
{
	m_playlist_idx = id;
	return S_OK;
}

STDMETHODIMP DropAction::put_ToSelect(VARIANT_BOOL select)
{
	m_to_select = to_bool(select);
	return S_OK;
}

void DropAction::Reset()
{
	m_playlist_idx = 0;
	m_base = 0;
	m_to_select = true;
	m_effect = DROPEFFECT_NONE;
}
