#pragma once
#include <optional>
#include "IDocumentStore.h"
#include "CertificateDetails.h"
#include "CurlHandlesHolder.h"
#include "EventHandler.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents
{
	//TODO complete !
	//Contains implementation of some IDocumentStore operations shared by DocumentStore implementations
	class DocumentStoreBase : public IDocumentStore
	{
	private:
		mutable std::shared_ptr<conventions::DocumentConventions> _conventions{};

		std::vector<primitives::EventHandler> on_before_store{};
		std::vector<primitives::EventHandler> on_after_save_changes{};
		std::vector<primitives::EventHandler> on_before_delete{};
		std::vector<primitives::EventHandler> on_before_query{};
		std::vector<primitives::EventHandler> on_session_created{};

	protected:
		std::weak_ptr<DocumentStoreBase> _weak_this;

		bool disposed = false;

		std::vector<std::string> urls{};
		bool is_initialized = false;
		std::string database{};

		std::optional<impl::CertificateDetails> _certificate_details{};
		impl::CurlOptionsSetter _set_before_perform = {};
		impl::CurlOptionsSetter _set_after_perform = {};

	protected:
		void ensure_not_closed() const;

		void assert_not_initialized(const std::string& property) const;

		void register_events(session::InMemoryDocumentSessionOperations& session) const;

		void after_session_created(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) const;

	public:
		~DocumentStoreBase() override = 0;

		virtual void close() = 0;

		virtual void add_before_close_listener(primitives::EventHandler handler) = 0;

		virtual void remove_before_close_listener(const primitives::EventHandler& handler) = 0;

		void add_after_close_listener(primitives::EventHandler handler) override = 0;

		void remove_after_close_listener(const primitives::EventHandler& handler) override = 0;

		bool is_disposed() const override;

		std::string get_identifier() const override = 0;

		void set_identifier(std::string identifier) override = 0;

		std::shared_ptr<IDocumentStore> initialize() override = 0;

		session::DocumentSession open_session() override = 0;
		session::DocumentSession open_session(const std::string& database) override = 0;
		session::DocumentSession open_session(const session::SessionOptions& options) override = 0;

		void execute_index(std::shared_ptr<indexes::AbstractIndexCreationTaskBase> task,
			std::optional<std::string> database = {}) override;

		void execute_indexes(std::vector<std::shared_ptr<indexes::AbstractIndexCreationTaskBase>>& tasks,
			std::optional<std::string> database = {}) override;
		
		std::shared_ptr<conventions::DocumentConventions> get_conventions() const override;

		void set_conventions(std::shared_ptr<conventions::DocumentConventions> conventions);

		const std::vector<std::string>& get_urls() const override;

		void set_urls(std::vector<std::string> urls);

		std::optional<int64_t> get_last_transaction_index(const std::string& database) const
		{
			//TODO implement
			return {};
		}

		void set_last_transaction_index(const std::string& database, std::optional<int64_t> index)
		{
			//TODO implement
		}

		void assert_initialized() const;

		void add_before_store_listener(primitives::EventHandler handler) override;

		void remove_before_store_listener(const primitives::EventHandler& handler) override;

		void add_after_save_changes_listener(primitives::EventHandler handler) override;

		void remove_after_save_changes_listener(const primitives::EventHandler& handler) override;

		void add_before_delete_listener(primitives::EventHandler handler) override;

		void remove_before_delete_listener(const primitives::EventHandler& handler) override;

		void add_before_query_listener(primitives::EventHandler handler) override;

		void remove_before_query_listener(const primitives::EventHandler& handler) override;

		const std::string& get_database() const override;

		void set_database(std::string database_param);

		const std::optional<impl::CertificateDetails>& get_certificate_details() const override;

		void set_certificate_details(impl::CertificateDetails certificate_details);

		std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const override = 0;

		std::shared_ptr<operations::MaintenanceOperationExecutor> maintenance() override = 0;

		std::shared_ptr<operations::OperationExecutor> operations() override = 0;;

		const impl::CurlOptionsSetter& get_before_perform() const;
		void set_before_perform(impl::CurlOptionsSetter options_setter);

		const impl::CurlOptionsSetter& get_after_perform() const;
		void set_after_perform(impl::CurlOptionsSetter options_setter);
	};

}
