#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "DocumentConventions.h"
#include "TransactionMode.h"
#include "BatchCommandResult.h"
#include "CommandDataBase.h"
#include "BatchOptions.h"
#include "PutAttachmentCommandData.h"
#include "PutAttachmentCommandHelper.h"
#include "CurlSListHolder.h"

namespace ravendb::client::documents::commands::batches
{
	class BatchCommand : public http::RavenCommand<json::BatchCommandResult>
	{
	private:
		class CurlMimeDeleter
		{
		public:
			void operator()(curl_mime* mime) const
			{
				curl_mime_free(mime);
			}
		};

	private:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const std::vector<std::shared_ptr<CommandDataBase>> _commands;
		std::vector<std::shared_ptr<PutAttachmentCommandData>> _put_attachments_commands{};
		const std::optional<BatchOptions> _options;
		const session::TransactionMode _mode;
		const std::string _request_entity_str;
		std::unique_ptr<curl_mime, CurlMimeDeleter> _curl_multipart{};
		impl::CurlSListHolder _mime_part_headers{};

	private:
		void append_options(std::ostringstream& path_builder) const;

	public:
		~BatchCommand() override = default;

		BatchCommand(std::shared_ptr<conventions::DocumentConventions> conventions, const std::list<std::shared_ptr<CommandDataBase>>& commands,
			std::optional<BatchOptions> options = {}, session::TransactionMode mode = session::TransactionMode::SINGLE_NODE);
		
		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
			std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
