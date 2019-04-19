#pragma once
#include "stdafx.h"
#include "IVoidMaintenanceOperation.h"
#include "IndexLockMode.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode,
	ravendb::client::http::VoidRavenCommand,
	ravendb::client::documents::indexes::IndexLockMode;

namespace ravendb::client::documents::operations::indexes
{
	namespace set_indexes_lock_op
	{
		struct Parameters
		{
			std::vector<std::string> index_names{};
			IndexLockMode mode = IndexLockMode::UNSET;
		};

		inline void to_json(nlohmann::json& j, const Parameters& p)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "IndexNames", p.index_names);
			set_val_to_json(j, "Mode", p.mode);
		}
	}

	class SetIndexesLockOperation : public IVoidMaintenanceOperation
	{
	private:
		const set_indexes_lock_op::Parameters _parameters;
		
	public:
		~SetIndexesLockOperation() override = default;

		SetIndexesLockOperation(std::string index_name, IndexLockMode mode)
			: SetIndexesLockOperation(set_indexes_lock_op::Parameters{ {std::move(index_name)}, mode })
		{}

		explicit SetIndexesLockOperation(set_indexes_lock_op::Parameters parameters)
			: _parameters([&]
		{
			if(parameters.index_names.empty())
			{
				throw std::invalid_argument("Indexes list must not be empty");
			}
			for(const auto& name : parameters.index_names)
			{
				if (name.empty())
				{
					throw std::invalid_argument("Index name must have a non empty value");
				}
			}
			if (parameters.mode == IndexLockMode::UNSET)
			{
				throw std::invalid_argument("Lock mode must not be UNSET");
			}
			filter_auto_indexes(parameters);
			return std::move(parameters);
		}())
		{}

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<SetIndexLockCommand>(_parameters);
		}

	private:
		class SetIndexLockCommand : public VoidRavenCommand
		{
		private:
			const nlohmann::json _parameters_json;

		public:
			~SetIndexLockCommand() override = default;

			explicit SetIndexLockCommand(const set_indexes_lock_op::Parameters parameters)
				: _parameters_json(parameters)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes/set-lock";

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, _parameters_json.dump().c_str());

				url = path_builder.str();
			}
		};

		static bool index_is_auto(const std::string& index_name)
		{
			constexpr std::string_view auto_index_prefix = "auto/";

			std::string low_case_index_name = index_name.substr(0, auto_index_prefix.length());
			std::transform(low_case_index_name.begin(), low_case_index_name.end(), low_case_index_name.begin(),
				[](decltype(low_case_index_name)::value_type c) {return std::tolower(c); });

			return low_case_index_name.compare(auto_index_prefix) == 0;

		}

		// Check for auto-indexes - we do not set lock for auto-indexes
		static void filter_auto_indexes(const set_indexes_lock_op::Parameters& parameters)
		{
			for(const auto& name : parameters.index_names)
			{
				if(index_is_auto(name))
				{
					throw std::invalid_argument("Indexes list contains Auto-Indexes. Lock Mode is not set for Auto-Indexes.");
				}
			}
		}
	};
}

