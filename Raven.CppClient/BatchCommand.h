#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "TransactionMode.h"
#include "BatchCommandResult.h"
#include "CommandDataBase.h"
#include "BatchOptions.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode;

namespace ravendb::client::documents::commands::batches
{
	class BatchCommand : public RavenCommand<json::BatchCommandResult>
	{
	private:
		const conventions::DocumentConventions _conventions;
		const std::vector<std::shared_ptr<CommandDataBase>> _commands;
		//TODO add std::unordered_set<std::istream> _attachment_streams{};
		const std::optional<BatchOptions> _options;
		const session::TransactionMode _mode;
		const std::string _request_entity_str;

		void append_options(std::ostringstream& path_builder) const
		{
			if(!_options)
			{
				return;
			}

			path_builder << "?";
			if(_options->replication_options)
			{
				path_builder << "&waitForReplicasTimeout=" <<
					impl::utils::millis_to_timespan(_options->replication_options->wait_for_replicas_timeout);
				if(_options->replication_options->throw_on_timeout_in_wait_for_replicas)
				{
					path_builder << "&throwOnTimeoutInWaitForReplicas=true";
				}
				path_builder << "&numberOfReplicasToWaitFor=" <<
					(_options->replication_options->majority ? "majority" :
						std::to_string(_options->replication_options->number_of_replicas_to_wait_for));
			}
			if (_options->index_options)
			{
				path_builder << "&waitForIndexesTimeout=" <<
					impl::utils::millis_to_timespan(_options->index_options->wait_for_indexes_timeout);
				if (_options->index_options->throw_on_timeout_in_wait_for_indexes)
				{
					path_builder << "&waitForIndexThrow=true";
				}else
				{
					path_builder << "&waitForIndexThrow=false";
				}
				if(!_options->index_options->wait_for_specific_indexes.empty())
				{
					for (const auto& specific_index : _options->index_options->wait_for_specific_indexes)
					{
						path_builder << "&waitForSpecificIndex=" << specific_index;
					}
				}
			}
		}

	public:
		~BatchCommand() override = default;

		BatchCommand(conventions::DocumentConventions conventions, std::vector<std::shared_ptr<CommandDataBase>> commands,
			std::optional<BatchOptions> options = {}, session::TransactionMode mode = session::TransactionMode::SINGLE_NODE)
			: _conventions(std::move(conventions))
			, _commands(std::move(commands))
			//, _attachment_streams()
			, _options(std::move(options))
			, _mode(mode)
			, _request_entity_str([this]()
		{
			std::vector<nlohmann::json> serialized_commands{};
			serialized_commands.reserve(_commands.size());
			for (auto& command : _commands)
			{
				serialized_commands.emplace_back(command->serialize());
			}
			nlohmann::json request_entity{};
			impl::utils::json_utils::set_val_to_json(request_entity, "Commands", serialized_commands);
			if (_mode == session::TransactionMode::CLUSTER_WIDE)
			{
				impl::utils::json_utils::set_val_to_json(request_entity, "TransactionMode", "ClusterWide");
			}
			return request_entity.dump();
		}())
		{
			//TODO populate _attachment_streams
		}

		
		void create_request(CURL* curl, const ServerNode& node, std::string& url) override
		{
			std::ostringstream path_builder;
			path_builder << node.url << "/databases/" << node.database << "/bulk_docs";
			append_options(path_builder);

			curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
			_headers_list.append("Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list.get());
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, _request_entity_str.c_str());

			url = path_builder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}
	};
}
