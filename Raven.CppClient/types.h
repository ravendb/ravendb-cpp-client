#pragma once

#include "stdafx.h"
#include "ServerNode.h"

namespace raven {
	struct RavenError
	{
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
		ErrorType type = generic;

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


	struct GetDocumentsResult 
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int next_page_start;
	};
	

	void from_json(const nlohmann::json& j, GetDocumentsResult& p);
}