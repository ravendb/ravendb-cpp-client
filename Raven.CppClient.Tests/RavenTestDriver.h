#pragma once
#include <atomic>
#include "ConnectionDetailsHolder.h"
#include "DocumentStore.h"
#include "DatabaseRecord.h"


namespace ravendb::client::tests::driver
{
	class RavenTestDriver : public ::testing::Test
	{
	private:
		static const infrastructure::ConnectionDetailsHolder secured_connection_details;
		static const infrastructure::ConnectionDetailsHolder unsecured_connection_details;

		static std::shared_ptr<documents::IDocumentStore> global_server;

		static std::shared_ptr<documents::IDocumentStore> global_secured_server;

		static std::mutex documents_stores_guard;
		static std::unordered_map<std::string, std::shared_ptr<documents::DocumentStore>> document_stores;

		static std::atomic_uint64_t index;

		virtual std::string generate_database_name() const;

		std::shared_ptr<documents::IDocumentStore> get_global_server(bool secured) const;

	protected:
		bool disposed = false;

		virtual void customise_db_record(serverwide::DatabaseRecord& db_record) const
		{}

		virtual void customise_store(std::shared_ptr<documents::DocumentStore> store);

		virtual void set_up_database(std::shared_ptr<documents::IDocumentStore> store) const
		{}

		void TearDown() override;

	public:
		virtual ~RavenTestDriver() = 0;

		bool is_disposed() const;

		std::shared_ptr<documents::DocumentStore> get_secured_document_store();

		std::shared_ptr<documents::DocumentStore> get_secured_document_store(const std::string& database);

		std::shared_ptr<documents::DocumentStore> get_document_store();

		std::shared_ptr<documents::DocumentStore> get_document_store(const std::string& database);

		std::shared_ptr<documents::DocumentStore> get_document_store(
			const std::string& database,
			bool secured,
			const std::optional<std::chrono::milliseconds>& wait_for_indexing_timeout);

		static void wait_for_indexing(
			std::shared_ptr<documents::IDocumentStore> store,
			const std::optional<std::string>& database = {},
			const std::optional<std::chrono::milliseconds>& timeout = {});

		void close();

	};
}
