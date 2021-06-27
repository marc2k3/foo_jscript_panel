#pragma once

class ContextMenuCommand
{
public:
	ContextMenuCommand(const std::wstring& command) : m_command(from_wide(command))
	{
		if (playback_control::get()->is_playing())
		{
			m_inited = true;
			contextmenu_manager::g_create(m_cm);
			m_cm->init_context_now_playing(contextmenu_manager::flag_view_full);
		}
	}

	ContextMenuCommand(const std::wstring& command, metadb_handle_list_cref handles) : m_command(from_wide(command))
	{
		if (handles.get_count() > 0)
		{
			m_inited = true;
			contextmenu_manager::g_create(m_cm);
			m_cm->init_context(handles, contextmenu_manager::flag_view_full);
		}
	}

	bool execute()
	{
		if (!m_inited) return false;
		return execute_recur(m_cm->get_root());
	}

private:
	bool execute_recur(contextmenu_node* parent, jstring parent_path = "")
	{
		for (uint32_t i = 0; i < parent->get_num_children(); ++i)
		{
			contextmenu_node* child = parent->get_child(i);
			string8 path = parent_path;
			path.add_string(child->get_name());

			switch (child->get_type())
			{
			case contextmenu_item_node::type_group:
				path.add_char('/');
				if (execute_recur(child, path))
				{
					return true;
				}
				break;
			case contextmenu_item_node::type_command:
				if (_stricmp(m_command, path) == 0)
				{
					child->execute();
					return true;
				}
				break;
			}
		}
		return false;
	}

	bool m_inited = false;
	contextmenu_manager::ptr m_cm;
	string8 m_command;
};

class MainMenuCommand
{
public:
	MainMenuCommand(const std::wstring& command) : m_command(from_wide(command))
	{
		if (s_group_guid_map.empty())
		{
			for (auto e = service_enum_t<mainmenu_group>(); !e.finished(); ++e)
			{
				auto ptr = *e;
				s_group_guid_map.emplace(hash_guid(ptr->get_guid()), ptr);
			}
		}
	}

	bool execute()
	{
		// Ensure commands on the Edit menu are enabled
		ui_edit_context_manager::get()->set_context_active_playlist();

		for (auto e = service_enum_t<mainmenu_commands>(); !e.finished(); ++e)
		{
			auto ptr = *e;
			mainmenu_commands_v2::ptr v2_ptr;
			ptr->cast(v2_ptr);

			const string8 parent_path = build_parent_path(ptr->get_parent());

			for (uint32_t i = 0; i < ptr->get_command_count(); ++i)
			{
				if (v2_ptr.is_valid() && v2_ptr->is_command_dynamic(i))
				{
					mainmenu_node::ptr node = v2_ptr->dynamic_instantiate(i);
					if (execute_recur(node, parent_path))
					{
						return true;
					}
				}
				else
				{
					string8 path = parent_path;
					string8 name;
					ptr->get_name(i, name);
					path.add_string(name);
					if (_stricmp(m_command, path) == 0)
					{
						ptr->execute(i, nullptr);
						return true;
					}
				}
			}
		}
		return false;
	}

private:
	bool execute_recur(mainmenu_node::ptr node, jstring parent_path)
	{
		string8 path = parent_path;
		string8 text;
		uint32_t flags = 0;
		node->get_display(text, flags);
		path.add_string(text);

		switch (node->get_type())
		{
		case mainmenu_node::type_group:
			path.end_with('/');
			for (uint32_t i = 0; i < node->get_children_count(); ++i)
			{
				mainmenu_node::ptr child = node->get_child(i);
				if (execute_recur(child, path))
				{
					return true;
				}
			}
			break;
		case mainmenu_node::type_command:
			if (_stricmp(m_command, path) == 0)
			{
				node->execute(nullptr);
				return true;
			}
			break;
		}
		return false;
	}

	string8 build_parent_path(GUID parent)
	{
		string8 path;
		while (parent != pfc::guid_null)
		{
			mainmenu_group::ptr group_ptr = s_group_guid_map.at(hash_guid(parent));
			mainmenu_group_popup::ptr group_popup_ptr;

			if (group_ptr->cast(group_popup_ptr))
			{
				string8 str;
				group_popup_ptr->get_display_string(str);
				str.add_char('/');
				str.add_string(path);
				path = str;
			}
			parent = group_ptr->get_parent();
		}
		return path;
	};

	inline static std::unordered_map<uint64_t, mainmenu_group::ptr> s_group_guid_map;
	string8 m_command;
};
