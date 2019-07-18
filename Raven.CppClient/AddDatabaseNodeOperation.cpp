#include "stdafx.h"
#include "AddDatabaseNodeOperation.h"

namespace ravendb::client::serverwide::operations
{
	AddDatabaseNodeOperation::~AddDatabaseNodeOperation() = default;

	AddDatabaseNodeOperation::AddDatabaseNodeOperation(std::string database_name, std::optional<std::string> node)
		: _database_name(std::move(database_name))
		, _node(std::move(node))
	{}

	std::unique_ptr<http::RavenCommand<DatabasePutResult>> AddDatabaseNodeOperation::get_command(
		std::shared_ptr<documents::conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<AddDatabaseNodeCommand>(_database_name, _node);
	}

	AddDatabaseNodeOperation::AddDatabaseNodeCommand::~AddDatabaseNodeCommand() = default;

	AddDatabaseNodeOperation::AddDatabaseNodeCommand::AddDatabaseNodeCommand(std::string database_name,
		std::optional<std::string> node)
		: _database_name(std::move(database_name))
		, _node(std::move(node))
	{}

	void AddDatabaseNodeOperation::AddDatabaseNodeCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/admin/databases/node?name=" << _database_name;

		if (_node)
		{
			path_builder << "&node=" << *_node;
		}
		std::stringstream dummy{};

		curl_ref.method = constants::methods::PUT;

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READDATA, &dummy);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE, curl_off_t(0));

		url_ref.emplace(path_builder.str());
	}

	void AddDatabaseNodeOperation::AddDatabaseNodeCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		if(!response)
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool AddDatabaseNodeOperation::AddDatabaseNodeCommand::is_read_request() const
	{
		return false;
	}
}
