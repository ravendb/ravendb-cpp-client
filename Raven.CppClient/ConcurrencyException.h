#pragma once
#include "ConflictException.h"

namespace ravendb::client::exceptions
{
	class ConcurrencyException : public ConflictException
	{
	public:
		int64_t expected_etag{};
		int64_t actual_etag{};

		std::string expected_change_vector{};
		std::string actual_change_vector{};

	public:
		~ConcurrencyException() override = default;

		ConcurrencyException();

		explicit ConcurrencyException(const std::string& message);
	};
}
