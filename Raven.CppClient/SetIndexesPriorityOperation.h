#pragma once
#include "stdafx.h"
#include "IVoidMaintenanceOperation.h"
#include "IndexPriority.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode,
ravendb::client::http::VoidRavenCommand,
ravendb::client::documents::indexes::IndexPriority;

namespace ravendb::client::documents::operations::indexes
{
	namespace set_indexes_priority_op
	{
		struct Parameters
		{
			std::vector<std::string> index_names{};
			IndexPriority priority = IndexPriority::UNSET;
		};

		inline void to_json(nlohmann::json& j, const Parameters& p)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "IndexNames", p.index_names);
			set_val_to_json(j, "Priority", p.priority);
		}
	}

	class SetIndexesPriorityOperation : public IVoidMaintenanceOperation
	{
	private:
		const set_indexes_priority_op::Parameters _parameters;

	public:
		~SetIndexesPriorityOperation() override = default;

		SetIndexesPriorityOperation(std::string index_name, IndexPriority priority)
			: SetIndexesPriorityOperation(set_indexes_priority_op::Parameters{ {std::move(index_name)}, priority })
		{}

		explicit SetIndexesPriorityOperation(set_indexes_priority_op::Parameters parameters)
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
			if (parameters.priority == IndexPriority::UNSET)
			{
				throw std::invalid_argument("Priority must not be UNSET");
			}
			return std::move(parameters);
		}())
		{}

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<SetIndexPriorityCommand>(_parameters);
		}

	private:
		class SetIndexPriorityCommand : public VoidRavenCommand
		{
		private:
			const nlohmann::json _parameters_json;

		public:
			~SetIndexPriorityCommand() override = default;

			explicit SetIndexPriorityCommand(const set_indexes_priority_op::Parameters parameters)
				: _parameters_json(parameters)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes/set-priority";

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, _parameters_json.dump().c_str());

				url = path_builder.str();
			}
		};
	};
}

