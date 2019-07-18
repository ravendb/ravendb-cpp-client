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
			std::optional<PatchRequest> patch_if_missing = {}, bool skip_patch_if_change_vector_mismatch = false);

		std::unique_ptr<http::RavenCommand<PatchResult>> get_command(
			std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			std::shared_ptr<http::HttpCache> cache) const override;

		std::unique_ptr<http::RavenCommand<PatchResult>> get_command(
			std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			std::shared_ptr<http::HttpCache> cache,
			bool return_debug_information,
			bool test) const;

	public:
		class PatchCommand : public http::RavenCommand<PatchResult>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;//TODO currently unused, check later
			const std::string _id;
			const std::optional<std::string> _change_vector;
			std::istringstream _patch_stream;
			const bool _skip_patch_if_change_vector_mismatch;
			const bool _return_debug_information;
			const bool _test;

		public:
			~PatchCommand() override = default;

			PatchCommand(std::shared_ptr<conventions::DocumentConventions> conventions,
				std::string id, std::optional<std::string> change_vector, const PatchRequest& patch,
				const std::optional<PatchRequest>& patch_if_missing, bool skip_patch_if_change_vector_mismatch,
				bool return_debug_information, bool test);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
