#include "stdafx.h"
#include "BatchPatchCommandData.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands::batches
{
	BatchPatchCommandData::IdAndChangeVector BatchPatchCommandData::IdAndChangeVector::create(std::string id_param,
		std::optional<std::string> change_vector_param)
	{
		return { std::move(id_param), std::move(change_vector_param) };
	}

	BatchPatchCommandData::BatchPatchCommandData(operations::PatchRequest patch,
		std::optional<operations::PatchRequest> patch_if_missing)
		: CommandDataBase({}, {}, {}, CommandType::BATCH_PATCH)
		, _patch(std::move(patch))
		, _patch_if_missing(std::move(patch_if_missing))
	{}

	void BatchPatchCommandData::add_id(std::string id, std::optional<std::string> change_vector)
	{
		if(impl::utils::is_blank(id))
		{
			throw std::invalid_argument("'id' cannot be blank");
		}

		if(auto res = _seen_ids.insert(id);
			! res.second )
		{
			std::ostringstream msg{};
			msg << "Could not add ID '" << id << "' because item with the same ID was already added";
			throw std::runtime_error(msg.str());
		}

		_ids.emplace_back(IdAndChangeVector::create(std::move(id), std::move(change_vector)));
	}

	BatchPatchCommandData::BatchPatchCommandData(operations::PatchRequest patch,
		std::optional<operations::PatchRequest> patch_if_missing, const std::vector<std::string>& ids)
		: BatchPatchCommandData(std::move(patch), std::move(patch_if_missing))
	{
		if(ids.empty())
		{
			throw std::invalid_argument("'ids cannot be empty");
		}
		for(const auto& id : ids)
		{
			add_id(id);
		}
	}

	BatchPatchCommandData::BatchPatchCommandData(operations::PatchRequest patch,
		std::optional<operations::PatchRequest> patch_if_missing, const std::vector<IdAndChangeVector>& ids)
		: BatchPatchCommandData(std::move(patch), std::move(patch_if_missing))
	{
		if (ids.empty())
		{
			throw std::invalid_argument("'ids cannot be empty");
		}
		for (const auto& id : ids)
		{
			add_id(id.id, id.change_vector);
		}
	}

	const std::vector<BatchPatchCommandData::IdAndChangeVector>& BatchPatchCommandData::get_ids() const
	{
		return _ids;
	}

	const operations::PatchRequest& BatchPatchCommandData::get_patch() const
	{
		return _patch;
	}

	const std::optional<operations::PatchRequest>& BatchPatchCommandData::get_patch_if_missing() const
	{
		return _patch_if_missing;
	}

	nlohmann::json BatchPatchCommandData::serialize() const
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		nlohmann::json json = nlohmann::json::object();

		nlohmann::json array = nlohmann::json::array();
		for(const auto& id : _ids)
		{
			nlohmann::json j = nlohmann::json::object();
			set_val_to_json(j, "Id", id.id);
			set_val_to_json(j, "ChangeVector", id.change_vector, false);

			array.emplace_back(std::move(j));
		}
		json.emplace("Ids" , std::move(array));

		set_val_to_json(json, "Patch", _patch);
		set_val_to_json(json, "Type", CommandType::BATCH_PATCH);
		set_val_to_json(json, "PatchIfMissing", _patch_if_missing, false);

		return json;
	}

	void BatchPatchCommandData::on_before_save_changes(
		std::shared_ptr<session::InMemoryDocumentSessionOperations> session)
	{// this command does not update session state after SaveChanges call!
	}
}
