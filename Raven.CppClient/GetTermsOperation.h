#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "TermsQueryResult.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetTermsOperation : public operations::IMaintenanceOperation<std::vector<std::string>>
	{
	private:
		const std::string _index_name;
		const std::string _field;
		const std::string _from_value;
		const std::optional<int32_t> _page_size;

	public:
		~GetTermsOperation() override = default;

		GetTermsOperation(std::string index_name, std::string field, std::string from_value = {},
			std::optional<int32_t> page_size = {})
			: _index_name([&]
		{
			if (index_name.empty())
			{
				throw std::invalid_argument("Index name must have a non empty value");
			}
			else
			{
				return std::move(index_name);
			}
		}())
			, _field([&]
		{
			if (field.empty())
			{
				throw std::invalid_argument("Field must have a non empty value");
			}
			else
			{
				return std::move(field);
			}
		}())
			, _from_value(std::move(from_value))
			, _page_size(std::move(page_size))
		{}

		std::unique_ptr<http::RavenCommand<std::vector<std::string>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetTermsCommand>(_index_name, _field, _from_value, _page_size);
		}

	private:
		class GetTermsCommand : public http::RavenCommand<std::vector<std::string>>
		{
		private:
			const std::string _index_name;
			const std::string _field;
			const std::string _from_value;
			const std::optional<int32_t> _page_size;

		public:
			~GetTermsCommand() override = default;

			GetTermsCommand(std::string index_name, std::string field, std::string from_value,
				std::optional<int32_t> page_size)
				: _index_name([&]
			{
				if (index_name.empty())
				{
					throw std::invalid_argument("Index name must have a non empty value");
				}
				else
				{
					return std::move(index_name);
				}
			}())
				, _field([&]
			{
				if (field.empty())
				{
					throw std::invalid_argument("Field must have a non empty value");
				}
				else
				{
					return std::move(field);
				}
			}())
				, _from_value(std::move(from_value))
				, _page_size(std::move(page_size))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/terms?name=" << http::url_encode(curl_ref, _index_name)
					<< "&field=" << http::url_encode(curl_ref, _field)
					<< "&fromValue=" << http::url_encode(curl_ref, _from_value)
					<< "&pageSize=";
				if (_page_size)
				{
					path_builder << _page_size.value();
				}

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>();
				auto res = response->get<queries::TermsQueryResult>();
				std::copy(res.terms.begin(), res.terms.end(), std::back_inserter(*_result));
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}



