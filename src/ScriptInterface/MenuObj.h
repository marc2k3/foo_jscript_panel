#pragma once

class MenuObj : public JSDisposableImpl<IMenuObj>
{
protected:
	MenuObj(CWindow wnd_parent);
	~MenuObj();

	void FinalRelease() override;

public:
	STDMETHODIMP get__HMENU(HMENU* out) override;
	STDMETHODIMP AppendMenuItem(UINT flags, UINT item_id, BSTR text) override;
	STDMETHODIMP AppendMenuSeparator() override;
	STDMETHODIMP AppendTo(IMenuObj* obj, UINT flags, BSTR text) override;
	STDMETHODIMP CheckMenuItem(UINT item_id, VARIANT_BOOL check) override;
	STDMETHODIMP CheckMenuRadioItem(UINT first, UINT last, UINT selected) override;
	STDMETHODIMP TrackPopupMenu(int x, int y, UINT flags, UINT* out) override;

private:
	CWindow m_wnd_parent;
	HMENU m_hMenu = nullptr;
	bool m_has_detached = false;
};
