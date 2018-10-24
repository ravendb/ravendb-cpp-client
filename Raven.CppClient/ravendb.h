#pragma once

#include "stdafx.h"

#include "types.h"
#include "commands.h"
#include "utils.h"

namespace raven {

	struct CurlGlobalRAII final
	{
		CurlGlobalRAII()
		{
			auto res = curl_global_init(CURL_GLOBAL_ALL);
			if(res)
			{
				throw std::runtime_error("curl_global_init failed !");
			}
		}
		~CurlGlobalRAII() { curl_global_cleanup(); }
	};

	namespace impl {
		// used when user wants to customize curl settings
		using CreateCurlHandleMethod_t =  CURL*(*)(void*);

		class CurlHolder 
		{
			struct CurlCleanup 
			{
				void operator () (CURL* curl) const {curl_easy_cleanup(curl);}
			};

			using CurlHandleUPtr = std::unique_ptr<CURL, CurlCleanup>;

			CreateCurlHandleMethod_t _createCurlHandleMethod;
			void* _state = nullptr;

			std::mutex _curlHandlesLock;
			// protected by _curlHandlesLock
			std::vector<CurlHandleUPtr> _curlHandles;

			void returnCurlToHolder(CurlHandleUPtr curl)
			{
				std::lock_guard<std::mutex> lg(_curlHandlesLock);
				_curlHandles.push_back(std::move(curl));
			}
		public:

			struct ReturnCurl 
			{
				std::unique_ptr<CURL, CurlCleanup> curl;
				CurlHolder* parent;

				CURL* operator->() {
					return curl.operator->();
				}

				CURL* get() {
					return curl.get();
				}

				ReturnCurl(std::unique_ptr<CURL, CurlCleanup> c, CurlHolder* p) :
					curl(std::move(c)), parent(p) {
				}

				ReturnCurl(ReturnCurl&& other) noexcept : curl(std::move(other.curl)), parent(other.parent) {
					other.parent = NULL;
				}

				~ReturnCurl() {
					if (parent)
					{
						parent->returnCurlToHolder(std::move(curl));
						parent = NULL;
					}
				}
			};

			ReturnCurl checkout_curl();

			CurlHolder(CreateCurlHandleMethod_t create_curl_instance, void* state);
		};
	}

	class RequestExecutor
	{
	private:
		std::string _dbName;
		std::vector<std::string> _initialUrls;
		std::shared_ptr<Topology> _topology;
		std::string _certificate;
		std::unique_ptr<impl::CurlHolder> _curl_holder;


		std::optional<RavenError> validate_urls();

		std::optional<RavenError> first_topology_update();

		template<typename TCommand, typename TResult>
		Result<TResult> execute(ServerNode& node, TCommand& cmd)
		{
			static_assert(std::is_base_of<RavenCommand<TResult>, TCommand>::value, "type parameter of this class must derive from RavenCommand");

			char error_buffer[CURL_ERROR_SIZE];
			error_buffer[0] = '\0';

			std::string output_buffer;
			std::map<std::string, std::string> headers;
			auto ch = _curl_holder->checkout_curl();
			auto curl = ch.get();

			std::shared_ptr<Topology> topology_local = std::atomic_load(&_topology);

			std::string url;
			cmd.createRequest(node, url, curl);

			//std::cout << "new created url is " << url << std::endl;

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, raven::impl::Utils::push_to_buffer);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_buffer);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, raven::impl::Utils::push_headers);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);

			RavenError err;
			CURLcode res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				long statusCode;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
				std::string errors;
				errors.append("Failed request to: ")
					.append(url)
					.append(", status code: ")
					.append(std::to_string(statusCode))
					.append("\n")
					.append(error_buffer)
					.append("\n");

				err = { errors, RavenError::generic };
				return Result<TResult>(err);
			}

			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			switch (statusCode)
			{
			case 200:
			case 201:
			{
				auto result = nlohmann::json::parse(output_buffer);
				cmd.setResponse(curl, result);
				break;
			}
			case 404:
				cmd.setResponseNotFound(curl);
				break;
			case 503:
				if (headers.find("Database-Missing") != headers.end()) {
					err = { output_buffer, RavenError::database_does_not_exists };
					return Result<TResult>(err);
				}
				err = { output_buffer, RavenError::service_not_available };
				return Result<TResult>(err);
			case 500:
				err = { output_buffer, RavenError::internal_server_error };
				return Result<TResult>(err);
			default:
				err = { output_buffer, RavenError::unexpected_response };
				return Result<TResult>(err);
			}

			auto result = cmd.getResult();
			return Result<TResult>(result, RavenError::success);
		}

		RequestExecutor(
			std::vector<std::string> initialUrls,
			std::string db,
			std::string certificate,
			std::unique_ptr<impl::CurlHolder> curl_holder);

		RequestExecutor(const RequestExecutor& other) = delete;
		RequestExecutor(RequestExecutor&& other) = delete;

		RequestExecutor& operator=(const RequestExecutor& other) = delete;
		RequestExecutor& operator=(RequestExecutor&& other) = delete;

	public:

		template<typename TCommand, typename TResult>
		Result<TResult> execute(TCommand& cmd) {
			std::string errors;

			std::shared_ptr<Topology> topology_local = std::atomic_load(&_topology);

			for (auto node : topology_local->nodes) {

				Result<TResult> result = execute<TCommand, TResult>(node, cmd);

				if (result.error.has_value())
				{
					errors.append(result.error.value().text).append("\n");
					if (result.error.value().type == RavenError::database_does_not_exists)
						return result;
					continue;
				}

				return result;
			}

			RavenError err = { errors, RavenError::generic };
			return Result<TResult>(err);
		}



		static Result<std::unique_ptr<RequestExecutor>> create(
			std::vector<std::string> urls,
			std::string db,
			std::string certificate = {},
			std::pair<raven::impl::CreateCurlHandleMethod_t, void*> create_curl = {});
	};

}