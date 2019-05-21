#pragma once
#include "RequestExecutor.h"

namespace ravendb::client::http
{
	class ClusterRequestExecutor : public RequestExecutor
	{
	private:
		std::timed_mutex _cluster_topology_mutex{};

	protected:
		ClusterRequestExecutor(
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		void perform_health_check(std::shared_ptr<const ServerNode> server_node, int32_t node_index) override;

		std::future<void> update_client_configuration_async() override;

		void throw_exceptions(std::string details) const override;

	public:
		static std::shared_ptr<ClusterRequestExecutor> create_for_single_node(const std::string& url,
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		static std::shared_ptr<ClusterRequestExecutor> create(const std::vector<std::string>& initial_urls,
			std::optional<impl::CertificateDetails> certificate_details,			
			std::shared_ptr<impl::TasksScheduler> scheduler,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions = {},
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		std::future<bool> update_topology_async(std::shared_ptr<const ServerNode> node, int32_t timeout, bool force_update, std::optional<std::string> debug_tag, bool run_async) override;
	};
}
