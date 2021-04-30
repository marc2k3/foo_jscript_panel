#pragma once

class DropTargetImpl : public ImplementCOMRefCounter<IDropTarget>
{
public:
	DropTargetImpl(PanelWindow* panel) : m_panel(panel), m_action(new ImplementCOMRefCounter<DropAction>()) {}

	QI_HELPER(IDropTarget)

	STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override
	{
		if (!pDataObj) return E_FAIL;
		if (!pdwEffect) return E_POINTER;

		m_action->Reset();

		bool native;
		if (SUCCEEDED(ole_interaction::get()->check_dataobject(pDataObj, m_allowed_effect, native)))
		{
			if (native && (*pdwEffect & DROPEFFECT_MOVE))
			{
				m_allowed_effect |= DROPEFFECT_MOVE;
			}
		}
		else
		{
			m_allowed_effect = DROPEFFECT_NONE;
		}

		m_action->m_effect = *pdwEffect & m_allowed_effect;
		invoke(CallbackID::on_drag_enter, grfKeyState, pt);
		*pdwEffect = m_action->m_effect;
		return S_OK;
	}

	STDMETHODIMP DragLeave() override
	{
		m_panel->m_script_host->InvokeCallback(CallbackID::on_drag_leave);
		return S_OK;
	}

	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override
	{
		if (!pdwEffect) return E_POINTER;

		m_action->m_effect = *pdwEffect & m_allowed_effect;
		invoke(CallbackID::on_drag_over, grfKeyState, pt);
		*pdwEffect = m_action->m_effect;
		return S_OK;
	}

	STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override
	{
		if (!pDataObj) return E_FAIL;
		if (!pdwEffect) return E_POINTER;

		m_action->m_effect = *pdwEffect & m_allowed_effect;
		invoke(CallbackID::on_drag_drop, grfKeyState, pt);

		if (*pdwEffect == DROPEFFECT_NONE || m_action->m_effect == DROPEFFECT_NONE)
		{
			*pdwEffect = DROPEFFECT_NONE;
			return S_OK;
		}

		dropped_files_data_impl droppedData;
		if (SUCCEEDED(ole_interaction::get()->parse_dataobject(pDataObj, droppedData)))
		{
			auto obj = fb2k::service_new<ProcessLocationsNotify>(m_action->m_playlist_idx, m_action->m_base, m_action->m_to_select);
			droppedData.to_handles_async_ex(playlist_incoming_item_filter_v2::op_flag_delay_ui, core_api::get_main_window(), obj);
		}

		*pdwEffect = m_action->m_effect;
		return S_OK;
	}

private:
	void invoke(CallbackID id, DWORD grfKeyState, const POINTL& pt)
	{
		CPoint point(pt.x, pt.y);
		m_panel->m_hwnd.ScreenToClient(&point);

		VariantArgs args = { grfKeyState , point.y, point.x, m_action.get_ptr() };
		m_panel->m_script_host->InvokeCallback(id, args);
	}

	DWORD m_allowed_effect = DROPEFFECT_NONE;
	PanelWindow* m_panel;
	pfc::com_ptr_t<DropAction> m_action;
};
