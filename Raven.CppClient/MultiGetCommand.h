#pragma once
#include "RavenCommand.h"
#include "GetResponse.h"
#include "HttpCache.h"
#include "GetRequest.h"

namespace ravendb::client::documents::commands::multi_get
{
	class MultiGetCommand : public http::RavenCommand<std::vector<GetResponse>>
	{
	private:
		const std::shared_ptr<http::HttpCache> _cache;
		const std::reference_wrapper<const std::vector<GetRequest>> _commands;

		std::string _base_url{};

	private:
		std::string get_cached_key(const GetRequest& command, std::optional<std::string>& request_url) const;

		void maybe_read_from_cache(GetResponse& get_response, const GetRequest& command) const;

		void maybe_set_cache(const GetResponse& get_response, const GetRequest& command) const;

	public:
		MultiGetCommand(std::shared_ptr<http::HttpCache> cache, const std::vector<GetRequest>& commands);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;


		bool is_read_request() const override;

		void set_response_raw(const impl::CurlResponse& response) override;//TODO implement

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;
	};
}
