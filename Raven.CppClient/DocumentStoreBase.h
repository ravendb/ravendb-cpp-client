#pragma once
#include "IDocumentStore.h"
#include "CertificateDetails.h"
#include "CurlHandlesHolder.h"
#include <optional>

namespace ravendb::client::documents
{
	//TODO complete !
	class DocumentStoreBase : public IDocumentStore
	{
	private:
		mutable std::shared_ptr<conventions::DocumentConventions> _conventions{};

	protected:
		std::weak_ptr<DocumentStoreBase> _weak_this;

		bool disposed = false;

		std::vector<std::string> urls{};
		bool is_initialized = false;
		std::string database{};

		std::optional<impl::CertificateDetails> _certificate_details{};
		impl::CurlOptionsSetter _set_before_perform = {};
		impl::CurlOptionsSetter _set_after_perform = {};

		void ensure_not_closed() const;

		void assert_initialized() const;
		void assert_not_initialized(const std::string& property) const;

	public:
		~DocumentStoreBase() override = 0;

		bool is_disposed() const;

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

		const std::string& get_database() const override;

		void set_database(std::string database_param);

		const std::optional<impl::CertificateDetails>& get_certificate_details() const;

		void set_certificate_details(impl::CertificateDetails certificate_details);

		const impl::CurlOptionsSetter& get_before_perform() const;
		void set_before_perform(impl::CurlOptionsSetter options_setter);

		const impl::CurlOptionsSetter& get_after_perform() const;
		void set_after_perform(impl::CurlOptionsSetter options_setter);
	};

}
