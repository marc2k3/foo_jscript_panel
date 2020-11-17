#pragma once

class DropSourceImpl : public ImplementCOMRefCounter<IDropSource>
{
public:
	QI_HELPER(IDropSource)

	STDMETHODIMP GiveFeedback(DWORD dwEffect) override
	{
		m_effect = dwEffect;
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override
	{
		if (fEscapePressed || (grfKeyState & MK_RBUTTON) || (grfKeyState & MK_MBUTTON))
		{
			return DRAGDROP_S_CANCEL;
		}

		if (!(grfKeyState & MK_LBUTTON))
		{
			return m_effect == DROPEFFECT_NONE ? DRAGDROP_S_CANCEL : DRAGDROP_S_DROP;
		}

		return S_OK;
	}

private:
	DWORD m_effect = DROPEFFECT_NONE;
};
