#pragma once
#include "json.hpp"

namespace ravendb::client::documents::commands::batches
{
	enum class CommandType
	{
		NONE,
		PUT,
		PATCH,
		DELETE_,
		ATTACHMENT_PUT,
		ATTACHMENT_DELETE,
		ATTACHMENT_MOVE,
		ATTACHMENT_COPY,
		COMPARE_EXCHANGE_PUT,
		COMPARE_EXCHANGE_DELETE,

		COUNTERS,

		BATCH_PATCH,

		CLIENT_ANY_COMMAND,
		CLIENT_MODIFY_DOCUMENT_COMMAND,

		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(CommandType,
		{
			{CommandType::UNSET, nullptr},
			{CommandType::NONE, "None"},
			{CommandType::PUT, "PUT"},
			{CommandType::PATCH, "PATCH"},
			{CommandType::DELETE_, "DELETE"},
			{CommandType::ATTACHMENT_PUT, "AttachmentPUT"},
			{CommandType::ATTACHMENT_DELETE, "AttachmentDELETE"},
			{CommandType::ATTACHMENT_MOVE, "AttachmentMOVE"},
			{CommandType::ATTACHMENT_COPY, "AttachmentCOPY"},
			{CommandType::COMPARE_EXCHANGE_PUT, "CompareExchangePUT"},
			{CommandType::COMPARE_EXCHANGE_DELETE, "CompareExchangeDELETE"},
			{CommandType::COUNTERS, "Counters"},
			{CommandType::BATCH_PATCH, "BatchPATCH"},
			{CommandType::CLIENT_ANY_COMMAND, "ClientAnyCommand"},
			{CommandType::CLIENT_MODIFY_DOCUMENT_COMMAND, "ClientModifyDocumentCommand"}
		})
}
