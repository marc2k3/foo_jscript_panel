#pragma once

namespace helpers
{
	static std::unordered_map<uint64_t, mainmenu_group::ptr> g_group_guid_map;

	static uint64_t hash(const GUID& g)
	{
		return hasher_md5::get()->process_single_string(pfc::print_guid(g).get_ptr()).xorHalve();
	}

	static string8 build_parent_path(GUID parent)
	{
		string8 path;
		while (parent != pfc::guid_null)
		{
			mainmenu_group::ptr group_ptr = g_group_guid_map.at(hash(parent));
			mainmenu_group_popup::ptr group_popup_ptr;

			if (group_ptr->cast(group_popup_ptr))
			{
				string8 str;
				group_popup_ptr->get_display_string(str);
				path = PFC_string_formatter() << str << "/" << path;
			}
			parent = group_ptr->get_parent();
		}
		return path;
	};

	static bool execute_mainmenu_command_recur(mainmenu_node::ptr node, stringp command, stringp p)
	{
		string8 path, text;
		uint32_t flags;
		node->get_display(text, flags);
		path << p << text;

		switch (node->get_type())
		{
		case mainmenu_node::type_group:
			if (text.get_length()) path.add_char('/');
			for (uint32_t i = 0; i < node->get_children_count(); ++i)
			{
				mainmenu_node::ptr child = node->get_child(i);
				if (execute_mainmenu_command_recur(child, command, path))
				{
					return true;
				}
			}
			break;
		case mainmenu_node::type_command:
			if (_stricmp(command, path) == 0)
			{
				node->execute(nullptr);
				return true;
			}
			break;
		}
		return false;
	}

	static bool execute_mainmenu_command(stringp command)
	{
		if (g_group_guid_map.empty())
		{
			for (auto e = service_enum_t<mainmenu_group>(); !e.finished(); ++e)
			{
				auto ptr = *e;
				g_group_guid_map.emplace(hash(ptr->get_guid()), ptr);
			}
		}

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
					if (execute_mainmenu_command_recur(node, command, path))
					{
						return true;
					}
				}
				else
				{
					string8 name;
					ptr->get_name(i, name);
					path.add_string(name);
					if (_stricmp(command, path) == 0)
					{
						ptr->execute(i, nullptr);
						return true;
					}
				}
			}
		}
		return false;
	}

	static bool execute_context_command_recur(contextmenu_node* parent, stringp command, stringp cpath = "")
	{
		for (uint32_t i = 0; i < parent->get_num_children(); ++i)
		{
			contextmenu_node* child = parent->get_child(i);
			string8 path = cpath.get_ptr();
			path.add_string(child->get_name());

			switch (child->get_type())
			{
			case contextmenu_item_node::type_group:
				path.add_char('/');
				if (execute_context_command_recur(child, command, path))
				{
					return true;
				}
				break;
			case contextmenu_item_node::type_command:
				if (_stricmp(command, path) == 0)
				{
					child->execute();
					return true;
				}
				break;
			}
		}
		return false;
	}
}
