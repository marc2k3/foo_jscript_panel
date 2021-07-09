#pragma once

namespace db
{
	struct Fields
	{
		uint32_t playcount = 0;
		uint32_t loved = 0;
		string8 first_played;
		string8 last_played;
		uint32_t rating = 0;
	};

	using hash_list = pfc::list_t<metadb_index_hash>;
	using hash_set = std::set<metadb_index_hash>;

	Fields get(metadb_index_hash hash);
	bool hashHandle(const metadb_handle_ptr& handle, metadb_index_hash& hash);
	metadb_index_manager::ptr theAPI();
	void get_hashes(metadb_handle_list_cref handles, hash_set& hashes);
	void refresh(metadb_handle_list_cref handles);
	void set(metadb_index_hash hash, Fields f);
}
