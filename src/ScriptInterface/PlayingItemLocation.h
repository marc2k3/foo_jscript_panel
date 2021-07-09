#pragma once

class PlayingItemLocation : public JSDispatchImpl<IPlayingItemLocation>
{
public:
	PlayingItemLocation(bool isValid, uint32_t playlistIndex, uint32_t playlistItemIndex);

	STDMETHODIMP get_IsValid(VARIANT_BOOL* out) override;
	STDMETHODIMP get_PlaylistIndex(int* out) override;
	STDMETHODIMP get_PlaylistItemIndex(int* out) override;

private:
	bool m_isValid = false;
	uint32_t m_playlistIndex = 0, m_playlistItemIndex = 0;
};
