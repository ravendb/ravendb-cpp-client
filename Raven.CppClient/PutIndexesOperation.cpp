#include "stdafx.h"
#include "PutIndexesOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	PutIndexesOperation::PutIndexesOperation(std::vector<documents::indexes::IndexDefinition> index_to_add)
		: _index_to_add([&]()
	{
		if (index_to_add.size() == 0)
		{
			throw std::runtime_error("index_to_add cannot be empty");
		}
		return std::move(index_to_add);
	}())
	{}

	std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::PutIndexResult>>> PutIndexesOperation::
		get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<PutIndexesCommand>(_index_to_add);
	}

	PutIndexesOperation::PutIndexesCommand::PutIndexesCommand(
		const std::vector<documents::indexes::IndexDefinition>& indexes_to_add)
		: _index_to_add([&]
	{
		if (indexes_to_add.size() == 0)
		{
			throw std::runtime_error("indexes_to_add cannot be empty");
		}
		std::remove_const_t<decltype(_index_to_add)> temp_index_to_add{};
		for (const auto& index : indexes_to_add)
		{
			nlohmann::json index_json = index;
			temp_index_to_add.emplace_back(std::move(index_json));
		}
		return temp_index_to_add;
	}())
		, _index_to_add_stream([this]()
	{
		nlohmann::json json_to_put;
		json_to_put.emplace("Indexes", _index_to_add);
		return json_to_put.dump();
	}())
	{}

	void PutIndexesOperation::PutIndexesCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/admin/indexes";

		curl_ref.method = constants::methods::PUT;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_index_to_add_stream);

		_index_to_add_stream.clear();
		_index_to_add_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}

	void PutIndexesOperation::PutIndexesCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		_result = std::make_shared<ResultType>();
		impl::utils::json_utils::get_val_from_json(*response, "Results", *_result);
	}

	bool PutIndexesOperation::PutIndexesCommand::is_read_request() const
	{
		return false;
	}
}
