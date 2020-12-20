#pragma once

class PlayingItemLocation : public JSDispatchImpl<IPlayingItemLocation>
{
protected:
	PlayingItemLocation(bool isValid, size_t playlistIndex, size_t playlistItemIndex);
	~PlayingItemLocation();

public:
	STDMETHODIMP get_IsValid(VARIANT_BOOL* out) override;
	STDMETHODIMP get_PlaylistIndex(int* out) override;
	STDMETHODIMP get_PlaylistItemIndex(int* out) override;

private:
	bool m_isValid = false;
	size_t m_playlistIndex = SIZE_MAX;
	size_t m_playlistItemIndex = SIZE_MAX;
};
