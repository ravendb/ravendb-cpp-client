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

		void to_json(nlohmann::json& j, const Parameters& p);
	}

	class SetIndexesLockOperation : public IVoidMaintenanceOperation
	{
	private:
		const set_indexes_lock_op::Parameters _parameters;
		
	public:
		~SetIndexesLockOperation() override = default;

		SetIndexesLockOperation(std::string index_name, documents::indexes::IndexLockMode mode);

		explicit SetIndexesLockOperation(set_indexes_lock_op::Parameters parameters);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class SetIndexLockCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _parameters_json_str;

		public:
			~SetIndexLockCommand() override = default;

			explicit SetIndexLockCommand(const set_indexes_lock_op::Parameters& parameters);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};

		static bool index_is_auto(const std::string& index_name);

		// Check for auto-indexes - we do not set lock for auto-indexes
		static void filter_auto_indexes(const set_indexes_lock_op::Parameters& parameters);
	};
}

