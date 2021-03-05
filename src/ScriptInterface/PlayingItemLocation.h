#pragma once

class PlayingItemLocation : public JSDispatchImpl<IPlayingItemLocation>
{
public:
	PlayingItemLocation(bool isValid, size_t playlistIndex, size_t playlistItemIndex);

	STDMETHODIMP get_IsValid(VARIANT_BOOL* out) override;
	STDMETHODIMP get_PlaylistIndex(int* out) override;
	STDMETHODIMP get_PlaylistItemIndex(int* out) override;

private:
	bool m_isValid = false;
	size_t m_playlistIndex = SIZE_MAX;
	size_t m_playlistItemIndex = SIZE_MAX;
};
