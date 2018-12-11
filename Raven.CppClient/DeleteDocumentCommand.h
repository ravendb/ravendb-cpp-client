#pragma once
#include "stdafx.h"
#include "VoidRavenCommand.h"

using
	ravendb::client::http::VoidRavenCommand,
	ravendb::client::http::ServerNode;

namespace ravendb::client::documents::commands
{
	class DeleteDocumentCommand : public VoidRavenCommand
	{
	private:
		std::string _id;
		std::string _change_vector;
		struct curl_slist * _headers_list = nullptr;

		void reset_headers_list()
		{
			curl_slist_free_all(_headers_list);
			_headers_list = nullptr;
		}

	public:
		~DeleteDocumentCommand() override
		{
			reset_headers_list();
		}

		DeleteDocumentCommand(std::string id, std::string change_vector = {})
			: _id([&id]
			{
				if (id.empty()) throw std::invalid_argument("id cannot be empty");
				return std::move(id);
			}())
			, _change_vector(std::move(change_vector))
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database
				<< "/docs?id=" << impl::utils::url_escape(curl, _id);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

			reset_headers_list();
			if (! _change_vector.empty())
			{
				std::ostringstream change_vector_header;
				change_vector_header << "If-Match:" << '"' << _change_vector << '"';

				auto temp_headers_list = curl_slist_append(_headers_list, change_vector_header.str().c_str());
				if (temp_headers_list == nullptr)
				{
					reset_headers_list();
					throw std::runtime_error("error in curl_slist_append");
				}
				_headers_list = temp_headers_list;
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list);
			url = pathBuilder.str();
		}
	};
}
