#pragma once
#include "RavenException.h"
#include "CurlResponse.h"
#include "json_utils.h"

namespace ravendb::client::exceptions
{
	struct ExceptionSchema
	{
		std::string url{};
		std::string type{};
		std::string message{};
		std::string error{};
	};

	void from_json(const nlohmann::json & j, ExceptionSchema& es);

	class ExceptionDispatcher
	{
	private:
		static void throw_conflict(const ExceptionSchema& schema, const std::string& json);

		static std::exception_ptr get_raven_exception_from(const ExceptionSchema& schema,
			const std::string& message = "",
			std::exception_ptr inner = nullptr);

	public:
		~ExceptionDispatcher() = delete;
		ExceptionDispatcher() = delete;

		static std::exception_ptr get(const ExceptionSchema& schema, int32_t code, std::exception_ptr inner = nullptr);

		static void throw_exception(const impl::CurlResponse& response);
	};
}
