#include "stdafx.h"
#include "SetIndexesLockOperation.h"
#include "json_utils.h"
#include "CompareStringsEqualIgnoreCase.h"

namespace ravendb::client::documents::operations::indexes
{
	void set_indexes_lock_op::to_json(nlohmann::json& j, const Parameters& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "IndexNames", p.index_names);
		set_val_to_json(j, "Mode", p.mode);
	}

	SetIndexesLockOperation::SetIndexesLockOperation(std::string index_name, documents::indexes::IndexLockMode mode)
		: SetIndexesLockOperation(set_indexes_lock_op::Parameters{ {std::move(index_name)}, mode })
	{}

	SetIndexesLockOperation::SetIndexesLockOperation(set_indexes_lock_op::Parameters parameters)
		: _parameters([&]
	{
		if (parameters.index_names.empty())
		{
			throw std::invalid_argument("Indexes list must not be empty");
		}
		for (const auto& name : parameters.index_names)
		{
			if (name.empty())
			{
				throw std::invalid_argument("Index name must have a non empty value");
			}
		}
		if (parameters.mode == documents::indexes::IndexLockMode::UNSET)
		{
			throw std::invalid_argument("Lock mode must not be UNSET");
		}
		filter_auto_indexes(parameters);
		return std::move(parameters);
	}())
	{}

	std::unique_ptr<http::VoidRavenCommandBase> SetIndexesLockOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<SetIndexLockCommand>(_parameters);
	}

	SetIndexesLockOperation::SetIndexLockCommand::SetIndexLockCommand(const set_indexes_lock_op::Parameters& parameters)
		: _parameters_json_str(nlohmann::json(parameters).dump())
	{}

	void SetIndexesLockOperation::SetIndexLockCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/set-lock";

		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_parameters_json_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _parameters_json_str.c_str());
		curl_ref.method = constants::methods::POST;

		url_ref.emplace(path_builder.str());
	}

	bool SetIndexesLockOperation::index_is_auto(const std::string& index_name)
	{
		static const std::string AUTO_INDEX_PREFIX = "auto/";

		return impl::utils::CompareStringsEqualIgnoreCase()(index_name.substr(0, AUTO_INDEX_PREFIX.length()),
			AUTO_INDEX_PREFIX);
	}

	void SetIndexesLockOperation::filter_auto_indexes(const set_indexes_lock_op::Parameters& parameters)
	{
		for (const auto& name : parameters.index_names)
		{
			if (index_is_auto(name))
			{
				throw std::invalid_argument("Indexes list contains Auto-Indexes. Lock Mode is not set for Auto-Indexes.");
			}
		}
	}
}
