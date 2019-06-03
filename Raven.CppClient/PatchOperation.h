#pragma once
#include "IOperation.h"
#include "PatchResult.h"
#include "PatchRequest.h"
#include "utils.h"

namespace ravendb::client::documents::operations
{
	class PatchOperation : public IOperation<PatchResult>
	{
	public:
		template<typename TDocument>
		using Result = std::pair<PatchStatus, TDocument>;

	private:
		const std::string _id;
		const std::optional<std::string> _change_vector;
		const PatchRequest _patch;
		const std::optional<PatchRequest> _patch_if_missing;
		const bool _skip_patch_if_change_vector_mismatch;

	public:
		~PatchOperation() override = default;

		PatchOperation(std::string id, std::optional<std::string> change_vector, PatchRequest patch,
			std::optional<PatchRequest> patch_if_missing = {}, bool skip_patch_if_change_vector_mismatch = false)
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

		std::unique_ptr<http::RavenCommand<PatchResult>> get_command
			(std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override
		{
			return std::make_unique<PatchCommand>(conventions, _id, _change_vector, _patch, _patch_if_missing,
				_skip_patch_if_change_vector_mismatch, false, false);
		}

		std::unique_ptr<http::RavenCommand<PatchResult>> get_command
			(std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache,
			 bool return_debug_information, bool test) const
		{
			return std::make_unique<PatchCommand>(conventions, _id, _change_vector, _patch, _patch_if_missing,
				_skip_patch_if_change_vector_mismatch, return_debug_information, test);
		}

		class PatchCommand : public http::RavenCommand<PatchResult>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _id;
			const std::optional<std::string> _change_vector;
			const std::string _patch_str;
			const bool _skip_patch_if_change_vector_mismatch;
			const bool _return_debug_information;
			const bool _test;

		public:
			~PatchCommand() override = default;

			PatchCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
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
				, _patch_str([&]
			{
				if (impl::utils::is_blank(patch.script))
					throw std::invalid_argument("Patch script must have a non empty value");

				if (patch_if_missing && impl::utils::is_blank(patch_if_missing->script))
					throw std::invalid_argument("PatchIfMissing script must have a non empty value");
				
				nlohmann::json j;
				j["Patch"] = patch;
				if(patch_if_missing)
				{
					j["PatchIfMissing"] = *patch_if_missing;
				}else
				{
					j["PatchIfMissing"] = nlohmann::detail::value_t::null;
				}
				return j.dump();
			}())
				, _skip_patch_if_change_vector_mismatch(skip_patch_if_change_vector_mismatch)
				, _return_debug_information(return_debug_information)
				, _test(test)
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database <<
					"/docs?id=" << http::url_encode(curl, _id);
				if(_skip_patch_if_change_vector_mismatch)
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

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_patch_str);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_patch_str.length());
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
				curl_ref.method = constants::methods::PATCH;

				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");
				add_change_vector_if_not_null(curl_ref, _change_vector);

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					return;
				}
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}
