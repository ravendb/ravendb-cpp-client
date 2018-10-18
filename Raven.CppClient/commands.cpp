#include "stdafx.h"


std::string raven::GetDatabaseTopologyCommand::create_request(raven::ServerNode& node, CURL* curl) {
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	return node.url + "/topology?name=" + node.database;
}
void raven::GetDatabaseTopologyCommand::set_response(CURL* curl, const nlohmann::json& response) {
	long statusCode;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
	if (statusCode == 200)
		_topology = response;
}


std::string raven::GetDocumentsCommand::create_request(ServerNode& node, CURL* curl) {
	auto url = node.url + "/databases/" + node.database + "/docs?";

	if (_start.has_value())
		url += "&start=" + std::to_string(_start.value());
	if (_pageSize.has_value())
		url += "&pageSize=" + std::to_string(_pageSize.value());
	if (_metadataOnly)
		url += "&metadataOnly=true";

	if (_startWith.empty() == false) {
		url += "&startsWith=" + raven::impl::Utils::url_escape(curl, _startWith);
		if (_matches.empty() == false) 
			url += "&matches=" + raven::impl::Utils::url_escape(curl, _matches);
		if(_exclude.empty() == false)
			url += "&exclude=" + raven::impl::Utils::url_escape(curl, _exclude);
		if (_startAfter.empty() == false)
			url += "&startAfter=" + raven::impl::Utils::url_escape(curl, _startAfter);
	}

	for (auto include : _includes) {
		url += "&include=" + include;
	}

	auto total_len = std::accumulate(_ids.begin(), _ids.end(), 0,
		[](int size, std::string val) {
			return size + val.length();
		});

	if (total_len < 1024) {
		// can use GET
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		for (auto id : _ids) {
			url += "&id=" + raven::impl::Utils::url_escape(curl, id);
		}
	}
	else {
		// ids too big, must use POST
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
		auto json_str = nlohmann::json({
			{"Ids", _ids}
		}).dump();
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());
	}
	return url;
}

void raven::GetDocumentsCommand::set_response(CURL* curl/*what for ?*/, const nlohmann::json& response)
{
	_result = response;
}

