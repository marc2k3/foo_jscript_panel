#pragma once

class PlaybackQueueItem : public JSDisposableImpl<IPlaybackQueueItem>
{
public:
	PlaybackQueueItem(const t_playback_queue_item& playback_queue_item);

	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP get_Handle(IMetadbHandle** out) override;
	STDMETHODIMP get_PlaylistIndex(int* out) override;
	STDMETHODIMP get_PlaylistItemIndex(int* out) override;

protected:
	void FinalRelease() override;

private:
	t_playback_queue_item m_playback_queue_item;
};
