#include "stdafx.h"
#include "BatchCommand.h"

namespace ravendb::client::documents::commands::batches
{
	void BatchCommand::append_options(std::ostringstream& path_builder) const
	{
		if (!_options)
		{
			return;
		}

		path_builder << "?";
		if (_options->replication_options)
		{
			path_builder << "&waitForReplicasTimeout=" <<
				impl::utils::MillisToTimeSpanConverter(_options->replication_options->wait_for_replicas_timeout);
			if (_options->replication_options->throw_on_timeout_in_wait_for_replicas)
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
				impl::utils::MillisToTimeSpanConverter(_options->index_options->wait_for_indexes_timeout);
			if (_options->index_options->throw_on_timeout_in_wait_for_indexes)
			{
				path_builder << "&waitForIndexThrow=true";
			}
			else
			{
				path_builder << "&waitForIndexThrow=false";
			}
			if (!_options->index_options->wait_for_specific_indexes.empty())
			{
				for (const auto& specific_index : _options->index_options->wait_for_specific_indexes)
				{
					path_builder << "&waitForSpecificIndex=" << specific_index;
				}
			}
		}
	}

	BatchCommand::BatchCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
		const std::list<std::shared_ptr<CommandDataBase>>& commands, std::optional<BatchOptions> options,
		session::TransactionMode mode)
		: _conventions(conventions)
		, _commands(commands.cbegin(), commands.cend())
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
		std::unordered_set<std::istream*> attachment_streams{};

		for (const auto& command : _commands)
		{
			if (auto command_data = std::dynamic_pointer_cast<PutAttachmentCommandData>(command))
			{
				bool is_stream_unique{};
				std::tie(std::ignore, is_stream_unique) = attachment_streams.insert(&command_data->get_stream());
				if (is_stream_unique)
				{
					_put_attachments_commands.emplace_back(command_data);
				}
				else
				{
					PutAttachmentCommandHelper::throw_stream_was_already_used();
				}
			}
		}

		_mime_part_headers.append("Command-Type: AttachmentStream");
	}

	void BatchCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database << "/bulk_docs";
		append_options(path_builder);

		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.method = constants::methods::POST;

		if (_put_attachments_commands.empty())
		{
			curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_request_entity_str.size()));
			curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _request_entity_str.c_str());
		}
		else
		{
			_curl_multipart.reset(curl_mime_init(curl));

			//setting main json(serialized commands)
			auto part = curl_mime_addpart(_curl_multipart.get());
			curl_mime_name(part, "main");
			curl_mime_type(part, "application/json");
			curl_mime_data(part, _request_entity_str.c_str(), _request_entity_str.size());

			//setting attachments
			size_t counter{ 0 };
			for (auto& command : _put_attachments_commands)
			{
				part = curl_mime_addpart(_curl_multipart.get());
				std::ostringstream name{};
				name << "attachment" << ++counter;
				curl_mime_name(part, name.str().c_str());
				if (const auto& content_type =/*assignment*/ command->get_content_type())
				{
					curl_mime_type(part, content_type->c_str());
				}
				curl_mime_headers(part, _mime_part_headers.get(), 0);
				curl_mime_data_cb(part, 0, stream_read_callback, nullptr, nullptr, &command->get_stream());
			}
			curl_easy_setopt(curl, CURLOPT_MIMEPOST, _curl_multipart.get());
		}

		url_ref.emplace(path_builder.str());
	}

	void BatchCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			throw std::runtime_error("Got null response from the server after doing a batch,"
				" something is very wrong. Probably a garbled response.");
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool BatchCommand::is_read_request() const
	{
		return false;
	}
}
