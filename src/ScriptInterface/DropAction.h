#pragma once

class DropAction : public JSDispatchImpl<IDropAction>
{
protected:
	DropAction();
	~DropAction();

public:
	STDMETHODIMP get_Effect(UINT* p) override;
	STDMETHODIMP put_Base(UINT base) override;
	STDMETHODIMP put_Effect(UINT effect) override;
	STDMETHODIMP put_Playlist(UINT id) override;
	STDMETHODIMP put_ToSelect(VARIANT_BOOL to_select) override;

	void Reset();

	DWORD m_effect = DROPEFFECT_NONE;
	bool m_to_select = true;
	size_t m_base = 0;
	size_t m_playlist_idx = 0;
};
