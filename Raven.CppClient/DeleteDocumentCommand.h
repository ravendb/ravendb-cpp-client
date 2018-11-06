#pragma once
#include "RavenCommand.h"
#include "VoidRavenCommand.h"

namespace ravenDB
{
	class DeleteDocumentCommand : public VoidRavenCommand
	{
	private:
		std::string _id;
		std::string _change_vector;
		mutable struct curl_slist * _headers_list = nullptr;

	public:

		~DeleteDocumentCommand() override
		{
			curl_slist_free_all(_headers_list);
		}

		DeleteDocumentCommand(std::string id, std::string change_vector = {})
			: _id([&id]
			{
				if (id.empty()) throw std::invalid_argument("id cannot be empty");
				return std::move(id);
			}())
			, _change_vector(std::move(change_vector))
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database
				<< "/docs?id=" << ravenDB::impl::Utils::url_escape(curl, _id);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

			if (not _change_vector.empty())
			{
				std::ostringstream change_vector_header;
				change_vector_header << "If-Match:" << '"' << _change_vector << '"';
				
				_headers_list = curl_slist_append(_headers_list, change_vector_header.str().c_str());
				if (_headers_list == nullptr)
				{
					throw std::runtime_error("error in curl_slist_append");
				}
			}
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list);

			url = pathBuilder.str();
		}
	};
}
