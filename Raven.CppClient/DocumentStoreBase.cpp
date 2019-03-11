#include "stdafx.h"
#include "DocumentStoreBase.h"
#include "DocumentConventions.h"

namespace ravendb::client::documents
{
	DocumentStoreBase::~DocumentStoreBase() = default;

	std::shared_ptr<conventions::DocumentConventions> DocumentStoreBase::get_conventions() const
	{
		return _conventions ? _conventions : _conventions = std::make_shared<conventions::DocumentConventions>();
	}

	void DocumentStoreBase::set_conventions(std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		assert_not_initialized("conventions");
		_conventions = conventions;
	}

	const std::vector<std::string>& DocumentStoreBase::get_urls() const
	{
		return urls;
	}

	void DocumentStoreBase::set_urls(std::vector<std::string> urls_param)
	{
		assert_not_initialized("urls");

		if(urls_param.empty())
		{
			throw std::invalid_argument("urls can't be empty");
		}
		for(auto& url : urls_param)
		{
			if (url.empty())
			{
				throw std::invalid_argument("url can't be empty");
			}

			if (url.back() == '/')
			{
				url.pop_back();
			}
		}
		urls = std::move(urls_param);
	}

	void DocumentStoreBase::assert_initialized() const
	{
		if(!is_initialized)
		{
			throw std::runtime_error("You cannot open a session or access the database commands before initializing the document store."
				" Did you forget calling initialize()?");
		}
	}

	void DocumentStoreBase::assert_not_initialized(const std::string& property) const
	{
		if(is_initialized)
		{
			std::ostringstream message{};
			message << "You cannot set '" << property << "' after the document store has been initialized.";
			throw std::runtime_error(message.str());
		}
	}

	const std::string& DocumentStoreBase::get_database() const
	{
		return database;
	}

	void DocumentStoreBase::set_database(std::string database_param)
	{
		assert_not_initialized("database");
		database = std::move(database_param);
	}

	const std::optional<impl::CertificateDetails>& DocumentStoreBase::get_certificate_details() const
	{
		return _certificate_details;
	}

	void DocumentStoreBase::set_certificate_details(impl::CertificateDetails certificate_details)
	{
		_certificate_details = std::move(certificate_details);
	}

	void DocumentStoreBase::set_before_perform(impl::CurlOptionsSetter options_setter)
	{
		_set_before_perform = std::move(options_setter);
	}

	void DocumentStoreBase::set_after_perform(impl::CurlOptionsSetter options_setter)
	{
		_set_after_perform = std::move(options_setter);
	}
}
