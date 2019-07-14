#include "stdafx.h"
#include "PromoteDatabaseNodeOperation.h"

namespace ravendb::client::serverwide::operations
{
	PromoteDatabaseNodeOperation::~PromoteDatabaseNodeOperation() = default;

	PromoteDatabaseNodeOperation::PromoteDatabaseNodeOperation(std::string database_name, std::string node)
		: _database_name(std::move(database_name))
		, _node(std::move(node))
	{}

	std::unique_ptr<http::RavenCommand<DatabasePutResult>> PromoteDatabaseNodeOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<PromoteDatabaseNodeCommand>(_database_name, _node);
	}

	PromoteDatabaseNodeOperation::PromoteDatabaseNodeCommand::~PromoteDatabaseNodeCommand() = default;

	PromoteDatabaseNodeOperation::PromoteDatabaseNodeCommand::PromoteDatabaseNodeCommand(std::string database_name,
		std::string node)
		: _database_name(std::move(database_name))
		, _node(std::move(node))
	{}

	void PromoteDatabaseNodeOperation::PromoteDatabaseNodeCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/admin/databases/promote?name=" << _database_name <<
			"&node=" << _node;

		curl_ref.method = constants::methods::POST;

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, curl_off_t(0));

		url_ref.emplace(path_builder.str());
	}

	void PromoteDatabaseNodeOperation::PromoteDatabaseNodeCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if (!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool PromoteDatabaseNodeOperation::PromoteDatabaseNodeCommand::is_read_request() const
	{
		return false;
	}
}

