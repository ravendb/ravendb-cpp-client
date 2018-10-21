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
	struct Result 
	{
		std::optional<RavenError> error;
		T value;

		Result(RavenError err) : error(std::move(err))
		{}

		Result(T value, RavenError::ErrorType /*???*/_) : value(std::move(value))
		{

		}
	};

	struct ServerNode 
	{
		std::string url{};
		std::string database{};
		std::string clusterTag{};

		ServerNode() = default;
		ServerNode(const ServerNode& other) = default;
		ServerNode(ServerNode&& other) = default;
		ServerNode& operator=(const ServerNode& other) = default;
		ServerNode& operator=(ServerNode&& other) = default;

		ServerNode(std::string url, std::string db, std::string tag)
		: url(std::move(url))
		, database(std::move(db))
		, clusterTag(std::move(tag))
		{}
	};

	void from_json(const nlohmann::json& j, ServerNode& p);


	struct Topology
	{
		std::vector<ServerNode> nodes;
		int64_t etag = -1;//TODO : set default value ! 
		
		Topology() = default;
		Topology(const Topology& other) = default;
		Topology(Topology&& other) = default;
		Topology& operator=(const Topology& other) = default;
		Topology& operator=(Topology&& other) = default;
	};


	void from_json(const nlohmann::json& j, Topology& p);

	struct GetDocumentsResult 
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int next_page_start;
	};
	

	void from_json(const nlohmann::json& j, GetDocumentsResult& p);
}