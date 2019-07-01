#pragma once
#include "IVoidMaintenanceOperation.h"
#include "IndexLockMode.h"

namespace ravendb::client::documents::operations::indexes
{
	namespace set_indexes_lock_op
	{
		struct Parameters
		{
			std::vector<std::string> index_names{};
			documents::indexes::IndexLockMode mode = documents::indexes::IndexLockMode::UNSET;
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

		SetIndexesLockOperation(std::string index_name, documents::indexes::IndexLockMode mode)
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
			if (parameters.mode == documents::indexes::IndexLockMode::UNSET)
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
		class SetIndexLockCommand : public http::VoidRavenCommand
		{
		private:
			const nlohmann::json _parameters_json;

		public:
			~SetIndexLockCommand() override = default;

			explicit SetIndexLockCommand(const set_indexes_lock_op::Parameters parameters)
				: _parameters_json(parameters)
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/set-lock";

				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

				auto&& json_str = _parameters_json.dump();

				curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
				curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());
				curl_ref.method = constants::methods::POST;

				url_ref.emplace(path_builder.str());
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

