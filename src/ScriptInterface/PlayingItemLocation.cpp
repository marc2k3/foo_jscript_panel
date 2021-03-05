#include "stdafx.h"
#include "PlayingItemLocation.h"

PlayingItemLocation::PlayingItemLocation(bool isValid, size_t playlistIndex, size_t playlistItemIndex) : m_isValid(isValid), m_playlistIndex(playlistIndex), m_playlistItemIndex(playlistItemIndex) {}

STDMETHODIMP PlayingItemLocation::get_IsValid(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(m_isValid);
	return S_OK;
}

STDMETHODIMP PlayingItemLocation::get_PlaylistIndex(int* out)
{
	if (!out) return E_POINTER;

	*out = m_isValid ? static_cast<int>(m_playlistIndex) : -1;
	return S_OK;
}

STDMETHODIMP PlayingItemLocation::get_PlaylistItemIndex(int* out)
{
	if (!out) return E_POINTER;

	*out = m_isValid ? static_cast<int>(m_playlistItemIndex) : -1;
	return S_OK;
}
