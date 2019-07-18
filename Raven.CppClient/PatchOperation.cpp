#include "stdafx.h"
#include "PatchOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations
{
	PatchOperation::PatchOperation(std::string id, std::optional<std::string> change_vector, PatchRequest patch,
		std::optional<PatchRequest> patch_if_missing, bool skip_patch_if_change_vector_mismatch)
		: _id([&]
	{
		if (impl::utils::is_blank(id))
			throw std::invalid_argument("Id must have a non empty value");
		else
			return std::move(id);
	}())
		, _change_vector(std::move(change_vector))
		, _patch([&]
	{
		if (impl::utils::is_blank(patch.script))
			throw std::invalid_argument("Patch script must have a non empty value");
		else
			return std::move(patch);
	}())
		, _patch_if_missing([&]
	{
		if (patch_if_missing && impl::utils::is_blank(patch_if_missing->script))
			throw std::invalid_argument("PatchIfMissing script must have a non empty value");
		else
			return std::move(patch_if_missing);
	}())
		, _skip_patch_if_change_vector_mismatch(skip_patch_if_change_vector_mismatch)
	{}

	std::unique_ptr<http::RavenCommand<PatchResult>> PatchOperation::get_command(std::shared_ptr<IDocumentStore> store,
		std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const
	{
		return std::make_unique<PatchCommand>(conventions, _id, _change_vector, _patch, _patch_if_missing,
			_skip_patch_if_change_vector_mismatch, false, false);
	}

	std::unique_ptr<http::RavenCommand<PatchResult>> PatchOperation::get_command(std::shared_ptr<IDocumentStore> store,
		std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache,
		bool return_debug_information, bool test) const
	{
		return std::make_unique<PatchCommand>(conventions, _id, _change_vector, _patch, _patch_if_missing,
			_skip_patch_if_change_vector_mismatch, return_debug_information, test);
	}

	PatchOperation::PatchCommand::PatchCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
		std::string id, std::optional<std::string> change_vector, const PatchRequest& patch,
		const std::optional<PatchRequest>& patch_if_missing, bool skip_patch_if_change_vector_mismatch,
		bool return_debug_information, bool test)
		: _conventions(conventions)
		, _id([&]
	{
		if (impl::utils::is_blank(id))
			throw std::invalid_argument("Id must have a non empty value");
		else
			return std::move(id);
	}())
		, _change_vector(std::move(change_vector))
		, _patch_stream([&]
	{
		if (impl::utils::is_blank(patch.script))
			throw std::invalid_argument("Patch script must have a non empty value");

		if (patch_if_missing && impl::utils::is_blank(patch_if_missing->script))
			throw std::invalid_argument("PatchIfMissing script must have a non empty value");

		nlohmann::json j{};

		using impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Patch", patch);
		set_val_to_json(j, "PatchIfMissing", patch_if_missing, true);

		return j.dump();
	}())
		, _skip_patch_if_change_vector_mismatch(skip_patch_if_change_vector_mismatch)
		, _return_debug_information(return_debug_information)
		, _test(test)
	{}

	void PatchOperation::PatchCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database <<
			"/docs?id=" << http::url_encode(curl_ref, _id);

		if (_skip_patch_if_change_vector_mismatch)
		{
			path_builder << "&skipPatchIfChangeVectorMismatch=true";
		}
		if (_return_debug_information)
		{
			path_builder << "&debug=true";
		}
		if (_test)
		{
			path_builder << "&test=true";
		}

		curl_ref.method = constants::methods::PATCH;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_patch_stream);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

		add_change_vector_if_not_null(curl_ref, _change_vector);

		_patch_stream.clear();
		_patch_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}

	void PatchOperation::PatchCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (!response)
		{
			return;
		}
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool PatchOperation::PatchCommand::is_read_request() const
	{
		return false;
	}
}
