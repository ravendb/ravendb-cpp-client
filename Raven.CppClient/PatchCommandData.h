#pragma once
#include "CommandDataBase.h"
#include "json_utils.h"
#include "PatchRequest.h"


namespace ravendb::client::documents::commands::batches
{
	class PatchCommandData : public CommandDataBase
	{
	private:
		const operations::PatchRequest _patch;
		const std::optional<operations::PatchRequest> _patch_if_missing;

	public:
		bool return_document{};

		PatchCommandData(std::string id, std::optional<std::string> change_vector,
			operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing);
			
		const operations::PatchRequest& get_patch() const
		{
			return _patch;
		}

		const std::optional<operations::PatchRequest>& get_patch_if_missing() const
		{
			return _patch_if_missing;
		}

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override;
	};
}
