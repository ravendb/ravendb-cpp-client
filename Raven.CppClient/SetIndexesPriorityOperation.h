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

		void to_json(nlohmann::json& j, const Parameters& p);
	}

	class SetIndexesPriorityOperation : public IVoidMaintenanceOperation
	{
	private:
		const set_indexes_priority_op::Parameters _parameters;

	public:
		~SetIndexesPriorityOperation() override = default;

		SetIndexesPriorityOperation(std::string index_name, documents::indexes::IndexPriority priority);

		explicit SetIndexesPriorityOperation(set_indexes_priority_op::Parameters parameters);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class SetIndexPriorityCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _parameters_json_str;

		public:
			~SetIndexPriorityCommand() override = default;

			explicit SetIndexPriorityCommand(const set_indexes_priority_op::Parameters parameters);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

