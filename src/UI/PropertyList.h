#pragma once
#include <libPPUI/CListControlComplete.h>
#include <libPPUI/CListControl-Cells.h>

class PropertyList : public CListControlComplete
{
public:
	struct ListItem
	{
		string8 key, value;
		bool bool_value = false;
		bool is_bool = false;
		bool is_string = false;
	};

	using ListItems = std::vector<ListItem>;

	BEGIN_MSG_MAP_EX(PropertyList)
		CHAIN_MSG_MAP(CListControlComplete);
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CONTEXTMENU(OnContextMenu)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT)
	{
		AddColumn("Name", MulDiv(360, m_dpi.cx, 96));
		AddColumnAutoWidth("Value");

		m_btn_clear = GetParent().GetDlgItem(IDC_BTN_CLEAR);
		m_btn_export = GetParent().GetDlgItem(IDC_BTN_EXPORT);
		return 0;
	}

	void OnContextMenu(CWindow, CPoint point)
	{
		if (m_data.empty()) return;

		point = this->GetContextMenuPoint(point);
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_SELECTALL, L"Select all\tCtrl+A");
		menu.AppendMenu(MF_STRING, ID_SELECTNONE, L"Select none");
		menu.AppendMenu(MF_STRING, ID_INVERTSEL, L"Invert selection");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(GetSelectedCount() ? MF_STRING : MF_GRAYED, ID_REMOVE, L"Remove\tDel");

		const int idx = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, m_hWnd, nullptr);
		switch (idx)
		{
		case ID_SELECTALL:
			this->SelectAll();
			break;
		case ID_SELECTNONE:
			this->SelectNone();
			break;
		case ID_INVERTSEL:
			this->SetSelection(pfc::bit_array_true(), pfc::bit_array_not(this->GetSelectionMask()));
			break;
		case ID_REMOVE:
			RequestRemoveSelection();
			break;
		}
	}

	bool AllowScrollbar(bool vertical) const override
	{
		return true;
	}

	bool CanSelectItem(size_t row) const override
	{
		return true;
	}

	bool GetCellCheckState(size_t row, size_t column) const override
	{
		if (column == 1 && m_data[row].is_bool) return m_data[row].bool_value;
		return false;
	}

	bool GetCellTypeSupported() const override
	{
		return true;
	}

	bool GetSubItemText(size_t row, size_t column, pfc::string_base& out) const override
	{
		switch(column)
		{
		case 0:
			out = m_data[row].key;
			return true;
		case 1:
			if (m_data[row].is_bool)
			{
				return false;
			}
			out = m_data[row].value;
			return true;
		default:
			out = "";
			return true;
		}
	}

	bool TableEdit_IsColumnEditable(size_t column) const override
	{
		return column == 1;
	}

	cellType_t GetCellType(size_t row, size_t column) const override
	{
		if (column == 1 && m_data[row].is_bool)
		{
			return &PFC_SINGLETON(CListCell_Checkbox);
		}
		else
		{
			return &PFC_SINGLETON(CListCell_Text);
		}
	}

	size_t GetItemCount() const override
	{
		return m_data.size();
	}

	void ExecuteDefaultAction(size_t index) override {}

	void OnItemsRemoved(const pfc::bit_array& mask, size_t oldCount) override
	{
		__super::OnItemsRemoved(mask, oldCount);
		if (m_data.empty())
		{
			m_btn_clear.EnableWindow(false);
			m_btn_export.EnableWindow(false);
		}
	}

	void OnSubItemClicked(size_t row, size_t column, CPoint pt) override
	{
		if (column == 1 && !m_data[row].is_bool)
		{
			TableEdit_Start(row, column);
			return;
		}
		__super::OnSubItemClicked(row, column, pt);
	}

	void RemoveMask(const pfc::bit_array& mask)
	{
		const size_t old_count = GetItemCount();
		pfc::remove_mask_t(m_data, mask);
		this->OnItemsRemoved(mask, old_count);
	}

	void RequestRemoveSelection() override
	{
		RemoveMask(GetSelectionMask());
	}

	void RequestReorder(size_t const* order, size_t count) override {}

	void SetCellCheckState(size_t row, size_t column, bool value) override
	{
		if (column == 1 && m_data[row].is_bool)
		{
			m_data[row].bool_value = value;
			__super::SetCellCheckState(row, column, value);
		}
	}

	void TableEdit_SetField(size_t row, size_t column, const char* value) override
	{
		if (column == 1 && !m_data[row].is_bool)
		{
			m_data[row].value = value;
			ReloadItem(row);
		}
	}

	CButton m_btn_clear, m_btn_export;
	ListItems m_data;
};
