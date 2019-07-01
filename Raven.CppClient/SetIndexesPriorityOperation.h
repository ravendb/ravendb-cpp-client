#pragma once
#include "IVoidMaintenanceOperation.h"
#include "IndexPriority.h"

namespace ravendb::client::documents::operations::indexes
{
	namespace set_indexes_priority_op
	{
		struct Parameters
		{
			std::vector<std::string> index_names{};
			documents::indexes::IndexPriority priority = documents::indexes::IndexPriority::UNSET;
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

		SetIndexesPriorityOperation(std::string index_name, documents::indexes::IndexPriority priority)
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
			if (parameters.priority == documents::indexes::IndexPriority::UNSET)
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
		class SetIndexPriorityCommand : public http::VoidRavenCommand
		{
		private:
			const nlohmann::json _parameters_json;

		public:
			~SetIndexPriorityCommand() override = default;

			explicit SetIndexPriorityCommand(const set_indexes_priority_op::Parameters parameters)
				: _parameters_json(parameters)
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/set-priority";

				auto&& json_str = _parameters_json.dump();

				curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
				curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());

				curl_ref.method = constants::methods::POST;
				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

				url_ref.emplace(path_builder.str());
			}
		};
	};
}

