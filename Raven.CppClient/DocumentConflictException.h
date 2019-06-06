#pragma once
#include "ConflictException.h"

namespace ravendb::client::exceptions::documents
{
	class DocumentConflictException : public ConflictException
	{
	public:
		std::optional<std::string> doc_id{};
		int64_t largest_etag{};

		~DocumentConflictException() override = default;

		DocumentConflictException();

		DocumentConflictException(const std::string& message, std::optional<std::string> doc_id, int64_t etag);

		static DocumentConflictException from_message(const std::string& message);

		static DocumentConflictException from_json(const std::string& json_str);
	};
}
