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
		std::vector<std::string> urls{};
		bool is_initialized = false;
		std::string database{};

		std::optional<impl::CertificateDetails> _certificate_details{};
		impl::CurlOptionsSetter _set_before_perform = {};
		impl::CurlOptionsSetter _set_after_perform = {};

		void assert_initialized() const;
		void assert_not_initialized(const std::string& property) const;

	public:
		~DocumentStoreBase() override = 0;

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

		void set_before_perform(impl::CurlOptionsSetter options_setter);
		void set_after_perform(impl::CurlOptionsSetter options_setter);
	};

}
