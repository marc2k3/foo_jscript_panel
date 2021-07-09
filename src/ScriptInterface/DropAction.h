#pragma once

class DropAction : public JSDispatchImpl<IDropAction>
{
public:
	STDMETHODIMP get_Effect(UINT* out) override;
	STDMETHODIMP put_Base(UINT base) override;
	STDMETHODIMP put_Effect(UINT effect) override;
	STDMETHODIMP put_Playlist(UINT id) override;
	STDMETHODIMP put_ToSelect(VARIANT_BOOL to_select) override;

	void Reset();

	DWORD m_effect = DROPEFFECT_NONE;
	bool m_to_select = true;
	uint32_t m_base = 0, m_playlist_idx = 0;
};
