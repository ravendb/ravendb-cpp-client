#pragma once
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::commands
{
	namespace explain_query_command
	{
		struct ExplainQueryResult
		{
			std::string index{};
			std::string reason{};
		};

		void from_json(const nlohmann::json& j, ExplainQueryResult& eqr);
	}

	class ExplainQueryCommand :public http::RavenCommand<std::vector<explain_query_command::ExplainQueryResult>>
	{
	private:		
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const queries::IndexQuery _index_query;

	public:
		~ExplainQueryCommand() override = default;

		ExplainQueryCommand(std::shared_ptr<conventions::DocumentConventions> conventions, queries::IndexQuery index_query);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
