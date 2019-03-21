#pragma once
#include "json_utils.h"
#include "DateTimeOffset.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Post
	{
		std::string id;
		std::string title;
		std::string desc;
		std::vector<Post> comments;
		std::string attachments;
		impl::DateTimeOffset created_at{};
	};

	inline void to_json(nlohmann::json& j, const Post& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", p.id);
		set_val_to_json(j, "Title", p.title);
		set_val_to_json(j, "Description", p.desc);
		set_val_to_json(j, "Comments", p.comments);
		set_val_to_json(j, "Attachments", p.attachments);
		set_val_to_json(j, "CreatedAt", p.created_at);
	}

	inline void from_json(const nlohmann::json& j, Post& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", p.id);
		get_val_from_json(j, "Title", p.title);
		get_val_from_json(j, "Description", p.desc);
		get_val_from_json(j, "Comments", p.comments);
		get_val_from_json(j, "Attachments", p.attachments);
		get_val_from_json(j, "CreatedAt", p.created_at);
	}
}

