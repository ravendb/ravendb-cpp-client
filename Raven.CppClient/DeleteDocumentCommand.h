#pragma once
#include "VoidRavenCommand.h"
#include "utils.h"

namespace ravendb::client::documents::commands
{
	class DeleteDocumentCommand : public http::VoidRavenCommand
	{
	private:
		std::string _id;
		std::optional<std::string> _change_vector;
		
	public:
		~DeleteDocumentCommand() override = default;
		
		DeleteDocumentCommand(std::string id, std::optional<std::string> change_vector = {})
			: _id([&id]
			{
				if (id.empty()) throw std::invalid_argument("id cannot be empty");
				return std::move(id);
			}())
			, _change_vector(std::move(change_vector))
		{}

		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
		{
			std::ostringstream path_builder;
			path_builder << node.url << "/databases/" << node.database
				<< "/docs?id=" << impl::utils::url_escape(curl, _id);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
			add_change_vector_if_not_null(curl, _change_vector);

			url = path_builder.str();
		}
	};
}
