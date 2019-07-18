#pragma once
#include "DisableDatabaseToggleResult.h"
#include "IServerOperation.h"

namespace ravendb::client::documents::operations
{
	namespace toggle_databases_state_operation
	{
		struct Parameters
		{
			std::vector<std::string> database_names{};
		};

		void to_json(nlohmann::json& j, const Parameters& p);
	}

	class ToggleDatabasesStateOperation : public serverwide::operations::IServerOperation<DisableDatabaseToggleResult>
	{
	private:
		const bool _disabled;
		const toggle_databases_state_operation::Parameters _parameters;

	public:
		~ToggleDatabasesStateOperation() override;

		ToggleDatabasesStateOperation(std::string database_name, bool disable);

		ToggleDatabasesStateOperation(std::vector<std::string> database_names, bool disable);

		ToggleDatabasesStateOperation(toggle_databases_state_operation::Parameters parameters, bool disable);

		std::unique_ptr<http::RavenCommand<DisableDatabaseToggleResult>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class ToggleDatabaseStateCommand : public http::RavenCommand<DisableDatabaseToggleResult>
		{
		private:
			const bool _disabled;
			const toggle_databases_state_operation::Parameters _parameters;
			const std::string _parameters_json_str;

		public:
			~ToggleDatabaseStateCommand() override;

			ToggleDatabaseStateCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions,
				toggle_databases_state_operation::Parameters parameters, bool disabled);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
