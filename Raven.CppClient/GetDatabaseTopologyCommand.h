#pragma once
#include <locale>

namespace raven
{
	class GetDatabaseTopologyCommand :public RavenCommand<Topology>
	{
	public:

		//~GetDatabaseTopologyCommand() override = default;
		GetDatabaseTopologyCommand() = default;

		void createRequest(const ServerNode& node, std::string& url, CURL* curl) const override
		{
			std::ostringstream urlBuilder;

			//for fiddler
			curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");


			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

			urlBuilder << node.url << "/topology?name=" << node.database;

			{
				std::string lowCaseUrl = node.url;
				std::transform(lowCaseUrl.begin(), lowCaseUrl.end(), lowCaseUrl.begin(), [](std::string::value_type c) {return std::tolower(c); });
				if (std::string::npos != lowCaseUrl.find(".fiddler"))
				{
					// we want to keep the '.fiddler' stuff there so we'll keep tracking request
					// so we are going to ask the server to respect it
					urlBuilder << "&localUrl=" << raven::impl::Utils::url_escape(curl, node.url);
				}
			}

			url = urlBuilder.str();
		}
		void setResponse(CURL* curl, const nlohmann::json& response) override
		{
			long statusCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
			if (statusCode == 200)
				_result = response;
		}
	};
}
