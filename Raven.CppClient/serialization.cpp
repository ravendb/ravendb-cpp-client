
#include "stdafx.h"

void raven::from_json(const nlohmann::json& j, raven::ServerNode& p) {
	p.url = j.at("Url").get<std::string>();
	p.database = j.at("Database").get<std::string>();
	p.clusterTag = j.at("ClusterTag").get<std::string>();
}



void raven::from_json(const nlohmann::json& j, raven::Topology& p) {
	p.etag = j.at("Etag").get<long>();
	auto nodes = j.at("Nodes").get<nlohmann::json::array_t>();
	for (auto node : nodes) {
		p.nodes.push_back(node);
	}
}


void raven::from_json(const nlohmann::json& j, raven::GetDocumentsResult& p) {

	p.includes = j.at("Includes").get<nlohmann::json::object_t>();
	p.results = j.at("Results").get<nlohmann::json::array_t>();
	auto next_page_start = j.find("NextPageStart");
	if (next_page_start != j.end())
		p.next_page_start = next_page_start->get<int>();
}
