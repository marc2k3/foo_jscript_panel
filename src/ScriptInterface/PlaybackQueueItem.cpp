#include "stdafx.h"
#include "PlaybackQueueItem.h"

PlaybackQueueItem::PlaybackQueueItem(const t_playback_queue_item& playback_queue_item) : m_playback_queue_item(playback_queue_item) {}
PlaybackQueueItem::~PlaybackQueueItem() {}

STDMETHODIMP PlaybackQueueItem::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = &m_playback_queue_item;
	return S_OK;
}

STDMETHODIMP PlaybackQueueItem::get_Handle(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<MetadbHandle>(m_playback_queue_item.m_handle);
	return S_OK;
}

STDMETHODIMP PlaybackQueueItem::get_PlaylistIndex(int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(m_playback_queue_item.m_playlist);
	return S_OK;
}

STDMETHODIMP PlaybackQueueItem::get_PlaylistItemIndex(int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(m_playback_queue_item.m_item);
	return S_OK;
}

void PlaybackQueueItem::FinalRelease()
{
	m_playback_queue_item.m_handle.release();
	m_playback_queue_item.m_playlist = 0;
	m_playback_queue_item.m_item = 0;
}
