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
		m_check_filter_add = GetDlgItem(IDC_CHECK_FILTER_ADD);
		m_check_filter_remove = GetDlgItem(IDC_CHECK_FILTER_REMOVE);
		m_check_filter_reorder = GetDlgItem(IDC_CHECK_FILTER_REORDER);
		m_check_filter_replace = GetDlgItem(IDC_CHECK_FILTER_REPLACE);
		m_check_filter_rename = GetDlgItem(IDC_CHECK_FILTER_RENAME);
		m_check_filter_remove_playlist = GetDlgItem(IDC_CHECK_FILTER_REMOVE_PLAYLIST);

		m_check_filter_add.SetCheck((m_flags & playlist_lock::filter_add));
		m_check_filter_remove.SetCheck((m_flags & playlist_lock::filter_remove));
		m_check_filter_reorder.SetCheck((m_flags & playlist_lock::filter_reorder));
		m_check_filter_replace.SetCheck((m_flags & playlist_lock::filter_replace));
		m_check_filter_rename.SetCheck((m_flags & playlist_lock::filter_rename));
		m_check_filter_remove_playlist.SetCheck((m_flags & playlist_lock::filter_remove_playlist));

		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		if (nID == IDOK)
		{
			m_flags = 0;
			if (m_check_filter_add.IsChecked()) m_flags |= playlist_lock::filter_add;
			if (m_check_filter_remove.IsChecked()) m_flags |= playlist_lock::filter_remove;
			if (m_check_filter_reorder.IsChecked()) m_flags |= playlist_lock::filter_reorder;
			if (m_check_filter_replace.IsChecked()) m_flags |= playlist_lock::filter_replace;
			if (m_check_filter_rename.IsChecked()) m_flags |= playlist_lock::filter_rename;
			if (m_check_filter_remove_playlist.IsChecked()) m_flags |= playlist_lock::filter_remove_playlist;

			PlaylistLock::remove(m_playlistIndex);
			PlaylistLock::add(m_playlistIndex, m_flags);
		}

		EndDialog(nID);
	}

	uint32_t m_flags = 0;

private:
	CCheckBox m_check_filter_add, m_check_filter_remove, m_check_filter_reorder, m_check_filter_replace, m_check_filter_rename, m_check_filter_remove_playlist;
	size_t m_playlistIndex = 0;
};
