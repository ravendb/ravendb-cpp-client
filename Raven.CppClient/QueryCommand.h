#pragma once
#include "stdafx.h"
#include "QueryResult.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::commands
{
	class QueryCommand : public http::RavenCommand<queries::QueryResult>
	{
	private:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const queries::IndexQuery _index_query;
		bool _metadata_only = false;
		bool _index_entries_only = false;

	public:
		~QueryCommand() override = default;

		QueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions, queries::IndexQuery index_query,
			bool metadata_only, bool index_entries_only);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;
		
		bool is_read_request() const override;
	};
}
