#include "stdafx.h"
#include "MainMenuManager.h"

STDMETHODIMP MainMenuManager::BuildMenu(IMenuObj* obj, UINT base_id)
{
	if (m_mm.is_empty()) return E_POINTER;

	HMENU menu;
	obj->get__HMENU(&menu);
	m_mm->generate_menu_win32(menu, base_id, SIZE_MAX, mainmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP MainMenuManager::ExecuteByID(UINT id, VARIANT_BOOL* out)
{
	if (m_mm.is_empty() || !out) return E_POINTER;

	*out = to_variant_bool(m_mm->execute_command(id));
	return S_OK;
}

STDMETHODIMP MainMenuManager::Init(BSTR root_name)
{
	static const std::vector<ValidRootName> valid_root_names =
	{
		{ L"file", &mainmenu_groups::file },
		{ L"edit", &mainmenu_groups::edit },
		{ L"view", &mainmenu_groups::view },
		{ L"playback", &mainmenu_groups::playback },
		{ L"library", &mainmenu_groups::library },
		{ L"help", &mainmenu_groups::help }
	};

	const auto it = std::ranges::find_if(valid_root_names, [&](const ValidRootName& item) { return _wcsicmp(root_name, item.name.data()) == 0; });
	if (it == valid_root_names.end()) return E_INVALIDARG;

	m_mm = mainmenu_manager::get();
	m_mm->instantiate(*it->guid);
	return S_OK;
}

void MainMenuManager::FinalRelease()
{
	m_mm.release();
}
