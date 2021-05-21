#pragma once

class CDialogPlaylistLock : public CDialogImpl<CDialogPlaylistLock>
{
public:
	CDialogPlaylistLock(size_t playlistIndex, uint32_t flags) : m_playlistIndex(playlistIndex), m_flags(flags) {}

	BEGIN_MSG_MAP_EX(CDialogPlaylistLock)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_PLAYLIST_LOCK };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		const std::map<int, uint32_t> id_filter_map =
		{
			{ IDC_CHECK_FILTER_ADD, playlist_lock::filter_add },
			{ IDC_CHECK_FILTER_REMOVE, playlist_lock::filter_remove },
			{ IDC_CHECK_FILTER_REORDER, playlist_lock::filter_reorder },
			{ IDC_CHECK_FILTER_REPLACE, playlist_lock::filter_replace },
			{ IDC_CHECK_FILTER_RENAME, playlist_lock::filter_rename },
			{ IDC_CHECK_FILTER_REMOVE_PLAYLIST, playlist_lock::filter_remove_playlist },
		};

		for (const auto& [id, filter] : id_filter_map)
		{
			Item item;
			item.hwnd = GetDlgItem(id);
			item.hwnd.SetCheck((m_flags & filter));
			item.filter = filter;
			m_items.emplace_back(item);
		}

		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		if (nID == IDOK)
		{
			uint32_t flags = 0;

			for (const auto& [hwnd, filter] : m_items)
			{
				if (hwnd.IsChecked()) flags |= filter;
			}

			if (flags != m_flags)
			{
				PlaylistLock::remove(m_playlistIndex);
				PlaylistLock::add(m_playlistIndex, flags);
			}
		}

		EndDialog(nID);
	}


private:
	struct Item
	{
		CCheckBox hwnd;
		uint32_t filter = 0;
	};

	size_t m_playlistIndex = 0;
	std::vector<Item> m_items;
	uint32_t m_flags = 0;
};
