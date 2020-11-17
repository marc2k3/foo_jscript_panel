#pragma once

class PlaybackQueueItem : public JSDisposableImpl<IPlaybackQueueItem>
{
protected:
	PlaybackQueueItem(const t_playback_queue_item& playback_queue_item);
	~PlaybackQueueItem();

	void FinalRelease() override;

public:
	STDMETHODIMP get__ptr(void** pp) override;
	STDMETHODIMP get_Handle(IMetadbHandle** pp) override;
	STDMETHODIMP get_PlaylistIndex(int* p) override;
	STDMETHODIMP get_PlaylistItemIndex(int* p) override;

private:
	t_playback_queue_item m_playback_queue_item;
};
