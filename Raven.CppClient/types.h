#pragma once

#include "stdafx.h"

namespace raven {
	struct RavenError {
		enum ErrorType {
			success,
			generic,
			database_does_not_exists,
			bad_url,
			internal_server_error,
			service_not_available,
			unexpected_response
		};

		std::string text;
		ErrorType type;

	};

	template<typename T>
	struct Result {
		std::optional<RavenError> error;
		T value;

		Result(RavenError err) : error(err) {

		}

		Result(T value, RavenError::ErrorType _) : value(std::move(value)) {

		}
	};

	struct ServerNode {
		std::string url, database, cluster_tag;

		ServerNode(const ServerNode& other) = default;
		ServerNode(ServerNode&& other) = default;

		ServerNode() {

		}

		ServerNode(std::string url, std::string db, std::string tag) :
			url(url), database(db), cluster_tag(tag) {

		}

		ServerNode operator=(const ServerNode& other) {
			url = other.url;
			database = other.database;
			cluster_tag = other.cluster_tag;
			return *this;
		}
	};


	void from_json(const nlohmann::json& j, ServerNode& p);


	struct Topology {
		std::vector<ServerNode> nodes;
		long etag;

		Topology(const Topology& other) : nodes(other.nodes), etag(etag) {

		}

		Topology(Topology&& other) : nodes(std::move(other.nodes)), etag(etag) {

		}

		Topology() {

		}

		Topology& operator=(const Topology& other) {
			nodes = other.nodes;
			etag = other.etag;
			return *this;
		}
	};


	void from_json(const nlohmann::json& j, Topology& p);

	struct GetDocumentsResult {
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int next_page_start;
	};
	

	void from_json(const nlohmann::json& j, GetDocumentsResult& p);
}