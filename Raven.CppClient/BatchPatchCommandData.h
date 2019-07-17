#pragma once
#include "CommandDataBase.h"
#include "CompareStringsLessThanIgnoreCase.h"
#include "PatchRequest.h"
#include <set>

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;
}

namespace ravendb::client::documents::commands::batches
{
	//Commands that patches multiple documents using same patch script
	//CAUTION: This command does not update session state after save_changes() call
	class BatchPatchCommandData : public CommandDataBase
	{
	public:
		friend class session::InMemoryDocumentSessionOperations;

		struct IdAndChangeVector
		{
			std::string id{};
			std::optional<std::string> change_vector{};

			static IdAndChangeVector create(std::string id_param, std::optional<std::string> change_vector_param);
		};

	private:
		std::set<std::string, impl::utils::CompareStringsLessThanIgnoreCase> _seen_ids{};

		std::vector<IdAndChangeVector> _ids{};

		const operations::PatchRequest _patch;
		const std::optional<operations::PatchRequest> _patch_if_missing;

	private:
		BatchPatchCommandData(operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing);

		void add_id(std::string id, std::optional<std::string> change_vector = {});

	public:
		BatchPatchCommandData(operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing,
			const std::vector<std::string>& ids);

		BatchPatchCommandData(operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing,
			const std::vector<IdAndChangeVector>& ids);

		const std::vector<IdAndChangeVector>& get_ids() const;

		const operations::PatchRequest& get_patch() const;

		const std::optional<operations::PatchRequest>& get_patch_if_missing() const;

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override;
	};
}
