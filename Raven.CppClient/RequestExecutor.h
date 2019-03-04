#pragma once

#include "CurlHandlesHolder.h"
#include "Topology.h"
#include "RavenCommand.h"
#include "utils.h"
#include "CertificateDetails.h"
#include "RavenErrors.h"
#include "DocumentConventions.h"

namespace ravendb::client::http
{
	class RequestExecutor
	{
	private:
		std::string _db_name;
		std::vector<std::string> _initial_urls;
		std::shared_ptr<Topology> _topology;
		std::optional<impl::CertificateDetails> _certificate_details;
		impl::CurlHandlesHolder _curl_holder{};
		impl::CurlOptionsSetter _set_before_perform = {};
		impl::CurlOptionsSetter _set_after_perform = {};

		std::shared_ptr<documents::conventions::DocumentConventions> _conventions{};

		void validate_urls();

		void first_topology_update();

		//TODO move the non-template part out 
		template<typename TResult>
		TResult execute_internal(ServerNode& node, RavenCommand<TResult>& cmd);

		RequestExecutor(
			std::string db_name,
			std::vector<std::string> initial_urls,
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			impl::CurlOptionsSetter set_before_perform,
			impl::CurlOptionsSetter set_after_perform);

	public:
		~RequestExecutor();

		RequestExecutor(const RequestExecutor& other) = delete;
		RequestExecutor(RequestExecutor&& other) = delete;
		RequestExecutor& operator=(const RequestExecutor& other) = delete;
		RequestExecutor& operator=(RequestExecutor&& other) = delete;

		std::shared_ptr<documents::conventions::DocumentConventions> get_conventions() const;

		template<typename Result_t>
		Result_t execute(RavenCommand<Result_t>& cmd);

		template<typename TResult>
		TResult execute(std::unique_ptr<RavenCommand<TResult>> cmd_ptr)
		{
			return execute(*cmd_ptr.get());
		}

		static std::unique_ptr<RequestExecutor> create(
			std::vector<std::string> urls,
			std::string db,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			std::optional<impl::CertificateDetails> certificate_details = {},
			ravendb::client::impl::CurlOptionsSetter set_before_perform = {},
			ravendb::client::impl::CurlOptionsSetter set_after_perform = {});
	};

	template<typename TResult>
	TResult RequestExecutor::execute_internal(ServerNode & node, RavenCommand<TResult>& cmd)
	{
		char error_buffer[CURL_ERROR_SIZE] = { '\0' };
		std::string output_buffer;
		std::map<std::string, std::string> headers;
		auto return_curl = _curl_holder.checkout_curl();
		auto curl = return_curl.get();

		std::string url;
		cmd.create_request(curl, node, url);

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ravendb::client::impl::utils::push_to_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_buffer);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, ravendb::client::impl::utils::push_headers);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
		if (_certificate_details)//using certificate
		{
			curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, ravendb::client::impl::utils::sslctx_function);
			curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, &_certificate_details);
		}

		if (_set_before_perform)
		{
			_set_before_perform(curl);
		}
		auto res = curl_easy_perform(curl);

		long statusCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);

		if (res != CURLE_OK)
		{
			std::ostringstream error_builder;
			error_builder << "Failed request to: "
				<< url
				<< ", status code: "
				<< std::to_string(statusCode)
				<< "\n"
				<< error_buffer
				<< "\n";

			throw RavenError(error_builder.str(), RavenError::ErrorType::GENERIC);
		}
		if (_set_after_perform)
		{
			_set_after_perform(curl);
		}

		cmd.status_code = statusCode;
		switch (statusCode)
		{
		case 200:
		case 201:
		{
			auto result = nlohmann::json::parse(output_buffer);
			cmd.set_response(curl, result, false);
			break;
		}
		case 204:
			break;
		case 304:
			break;
		case 404:
			cmd.set_response_not_found(curl);
			break;
		case 503:
			if (headers.find("Database-Missing") != headers.end())
			{
				throw RavenError(output_buffer, RavenError::ErrorType::DATABASE_DOES_NOT_EXIST);
			}
			throw RavenError(output_buffer, RavenError::ErrorType::SERVICE_NOT_AVAILABLE);
		case 500:
			throw RavenError(output_buffer, RavenError::ErrorType::INTERNAL_SERVER_ERROR);
		default:
			throw RavenError(output_buffer, RavenError::ErrorType::UNEXPECTED_RESPONSE);
		}

		return cmd.get_result();
	}
	template<typename TResult>
	TResult RequestExecutor::execute(RavenCommand<TResult>& cmd)
	{
		std::optional<std::ostringstream> errors;

		std::shared_ptr<Topology> topology_local = std::atomic_load(&_topology);

		for (auto& node : topology_local->nodes)
		{
			try
			{
				return execute_internal(node, cmd);
			}
			catch (RavenError& re)
			{
				if (!errors)
				{
					errors.emplace();
				}
				errors.value() << re.what() << '\n';
				if (re.get_error_type() == RavenError::ErrorType::DATABASE_DOES_NOT_EXIST)
				{
					throw RavenError(errors->str(), RavenError::ErrorType::DATABASE_DOES_NOT_EXIST);
				}
				continue;
			}
		}
		throw RavenError(errors->str(), RavenError::ErrorType::GENERIC);
	}
}
