#pragma once

class PlaybackQueueItem : public JSDisposableImpl<IPlaybackQueueItem>
{
protected:
	PlaybackQueueItem(const t_playback_queue_item& playback_queue_item);
	~PlaybackQueueItem();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** out) override;
	STDMETHODIMP get_Handle(IMetadbHandle** out) override;
	STDMETHODIMP get_PlaylistIndex(int* out) override;
	STDMETHODIMP get_PlaylistItemIndex(int* out) override;

private:
	t_playback_queue_item m_playback_queue_item;
};
