#pragma once

#include "stdafx.h"
#include "ServerNode.h"

namespace raven {
	

	class RavenError : public std::runtime_error
	{
	public:
		enum class ErrorType {
			success,
			generic,
			database_does_not_exists,
			bad_url,
			internal_server_error,
			service_not_available,
			unexpected_response
		};
	private:
		ErrorType _error_type = ErrorType::generic;

	public:
		RavenError(const std::string& what_arg, ErrorType error_type = ErrorType::generic)
			: std::runtime_error(what_arg)
			, _error_type(error_type)
		{}
		RavenError(const char* what_arg, ErrorType error_type)
			: RavenError(std::string(what_arg), error_type)
		{}

		const ErrorType& get_error_type() const noexcept
		{
			return _error_type;
		}
	};

	//template<typename T>
	//struct Result 
	//{
	//	std::optional<RavenError> error;
	//	T value;

	//	Result(RavenError err) : error(std::move(err))
	//	{}

	//	Result(T value, RavenError::ErrorType /*???*/_) : value(std::move(value))
	//	{

	//	}
	//};


	struct GetDocumentsResult 
	{
		nlohmann::json::object_t includes;
		nlohmann::json::array_t results;
		int next_page_start;
	};
	

	void from_json(const nlohmann::json& j, GetDocumentsResult& p);
}