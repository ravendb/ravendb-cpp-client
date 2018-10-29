#pragma once
#include <locale>
#include "CurlHandlesHolder.h"

namespace raven
{
	class GetDatabaseTopologyCommand :public RavenCommand<Topology>
	{
	public:

		~GetDatabaseTopologyCommand() override = default;
		GetDatabaseTopologyCommand() = default;

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream urlBuilder;
			urlBuilder << node.url << "/topology?name=" << node.database;

			//WON'T WORK WITH CURL !!!
			//{
			//	std::string lowCaseUrl = node.url;
			//	std::transform(lowCaseUrl.begin(), lowCaseUrl.end(), lowCaseUrl.begin(), [](std::string::value_type c) {return std::tolower(c); });
			//	if (std::string::npos != lowCaseUrl.find(".fiddler"))
			//	{
			//		// we want to keep the '.fiddler' stuff there so we'll keep tracking request
			//		// so we are going to ask the server to respect it
			//		urlBuilder << "&localUrl=" << raven::impl::Utils::url_escape(curl, node.url);
			//	}
			//}
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			url = urlBuilder.str();
		}
		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			if (statusCode == 200)
				_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return true;
		}
	};
}
