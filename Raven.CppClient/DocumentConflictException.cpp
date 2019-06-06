#include "stdafx.h"
#include "DocumentConflictException.h"
#include "BadResponseException.h"

namespace ravendb::client::exceptions::documents
{
	DocumentConflictException::DocumentConflictException() = default;

	DocumentConflictException::DocumentConflictException(const std::string& message, std::optional<std::string> doc_id,
		int64_t etag)
		: ConflictException(message)
		, doc_id(std::move(doc_id))
		, largest_etag(etag)
	{}

	DocumentConflictException DocumentConflictException::from_message(const std::string& message)
	{
		return DocumentConflictException(message, {}, 0);
	}

	DocumentConflictException DocumentConflictException::from_json(const std::string& json_str)
	{
		try
		{
			auto json = nlohmann::json::parse(json_str);
			const auto& doc_id = json.at("DocId");
			const auto& message = json.at("Message");
			const auto& largest_etag = json.at("LargestEtag");

			return DocumentConflictException(message.get<std::string>(), doc_id.get<std::string>(),
				largest_etag.get<int64_t>());
		}
		catch (...)
		{
			std::throw_with_nested(BadResponseException("Unable to parse server response: "));
		}
	}
}
