#include "stdafx.h"
#include "SetIndexesPriorityOperation.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	void set_indexes_priority_op::to_json(nlohmann::json& j, const Parameters& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "IndexNames", p.index_names);
		set_val_to_json(j, "Priority", p.priority);
	}

	SetIndexesPriorityOperation::SetIndexesPriorityOperation(std::string index_name,
		documents::indexes::IndexPriority priority)
		: SetIndexesPriorityOperation(set_indexes_priority_op::Parameters{ {std::move(index_name)}, priority })
	{}

	SetIndexesPriorityOperation::SetIndexesPriorityOperation(set_indexes_priority_op::Parameters parameters)
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
		if (parameters.priority == documents::indexes::IndexPriority::UNSET)
		{
			throw std::invalid_argument("Priority must not be UNSET");
		}
		return std::move(parameters);
	}())
	{}

	std::unique_ptr<http::VoidRavenCommandBase> SetIndexesPriorityOperation::get_command(
		std::shared_ptr<conventions::DocumentConventions> conventions) const
	{
		return std::make_unique<SetIndexPriorityCommand>(_parameters);
	}

	SetIndexesPriorityOperation::SetIndexPriorityCommand::SetIndexPriorityCommand(
		const set_indexes_priority_op::Parameters parameters)
		: _parameters_json_str(nlohmann::json(parameters).dump())
	{}

	void SetIndexesPriorityOperation::SetIndexPriorityCommand::create_request(
		impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/indexes/set-priority";

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_parameters_json_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _parameters_json_str.c_str());

		curl_ref.method = constants::methods::POST;
		curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

		url_ref.emplace(path_builder.str());
	}
}
