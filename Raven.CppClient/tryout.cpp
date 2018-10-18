#include "stdafx.h"

using namespace raven;

int main(int argc, char** argv) {
	curl_global_init(CURL_GLOBAL_DEFAULT);

	auto maybe_rq = RequestExecutor::create({ "http://live-test.ravendb.net" }, "dates");

	if (maybe_rq.error.has_value())
	{
		printf("%d %s", maybe_rq.error.value().type, maybe_rq.error.value().text.c_str());
		return 1;
	}

	auto rq = std::move(maybe_rq.value);

	GetDocumentsCommand cmd("users/1-A");
	auto result = rq->execute<GetDocumentsCommand, GetDocumentsResult>(cmd);

	if (result.error.has_value()) {
		printf("%d %s", result.error.value().type, result.error.value().text.c_str());
		return 2;
	}

	for (auto doc : result.value.results)
	{
		auto id = doc.at("@metadata").at("@id").get<std::string>();
		auto name = doc.at("Name").get<std::string>();
		printf("%s - %s\n", id.c_str(), name.c_str());
	}

	return 0;
}