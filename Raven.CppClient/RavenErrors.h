#pragma once

namespace ravendb::client
{
	class RavenError : public std::runtime_error
	{
	public:
		enum class ErrorType : uint8_t
		{
			success,
			generic,
			database_does_not_exist,
			bad_url,
			internal_server_error,
			service_not_available,
			unexpected_response,
			invalid_response,
		};
	private:
		ErrorType _error_type = ErrorType::generic;

	public:
		RavenError(const std::string& what_arg, ErrorType error_type = ErrorType::generic)
			: std::runtime_error(what_arg)
			, _error_type(error_type)
		{}
		RavenError(const char* what_arg, ErrorType error_type)
			: std::runtime_error(what_arg)
			, _error_type(error_type)
		{}

		const ErrorType& get_error_type() const noexcept
		{
			return _error_type;
		}
	};
}
