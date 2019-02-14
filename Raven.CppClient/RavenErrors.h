#pragma once

namespace ravendb::client
{
	class RavenError : public std::runtime_error
	{
	public:
		enum class ErrorType : uint8_t
		{
			GENERIC,
			DATABASE_DOES_NOT_EXIST,
			BAD_URL,
			INTERNAL_SERVER_ERROR,
			SERVICE_NOT_AVAILABLE,
			UNEXPECTED_RESPONSE,
			INVALID_RESPONSE,
		};
	private:
		ErrorType _error_type = ErrorType::GENERIC;

	public:
		RavenError(const std::string& what_arg, ErrorType error_type = ErrorType::GENERIC)
			: std::runtime_error(what_arg)
			, _error_type(error_type)
		{}

		RavenError(const char* what_arg, ErrorType error_type = ErrorType::GENERIC)
			: std::runtime_error(what_arg)
			, _error_type(error_type)
		{}

		const ErrorType& get_error_type() const noexcept
		{
			return _error_type;
		}
	};
}
