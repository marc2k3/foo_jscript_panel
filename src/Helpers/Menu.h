#pragma once

class ContextMenuCommand
{
public:
	ContextMenuCommand(const std::wstring& command) : m_command(from_wide(command)) {}

	bool execute_recur(contextmenu_node* parent, jstring p = "")
	{
		for (uint32_t i = 0; i < parent->get_num_children(); ++i)
		{
			contextmenu_node* child = parent->get_child(i);
			string8 path = p;
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

private:
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
				s_group_guid_map.emplace(hash(ptr->get_guid()), ptr);
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
			for (uint32_t i = 0; i < ptr->get_command_count(); ++i)
			{
				string8 path = build_parent_path(ptr->get_parent());

				mainmenu_commands_v2::ptr v2_ptr;
				if (ptr->cast(v2_ptr) && v2_ptr->is_command_dynamic(i))
				{
					mainmenu_node::ptr node = v2_ptr->dynamic_instantiate(i);
					if (execute_recur(node, path))
					{
						return true;
					}
				}
				else
				{
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
	bool execute_recur(mainmenu_node::ptr node, jstring p)
	{
		string8 path = p;
		string8 text;
		uint32_t flags;
		node->get_display(text, flags);
		path.add_string(text);

		switch (node->get_type())
		{
		case mainmenu_node::type_group:
			if (text.get_length()) path.add_char('/');
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
			mainmenu_group::ptr group_ptr = s_group_guid_map.at(hash(parent));
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

	uint64_t hash(const GUID& g)
	{
		return hasher_md5::get()->process_single_string(pfc::print_guid(g).get_ptr()).xorHalve();
	}

	inline static std::unordered_map<uint64_t, mainmenu_group::ptr> s_group_guid_map;
	string8 m_command;
};
