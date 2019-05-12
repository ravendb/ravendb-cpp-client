#pragma once
#include "VoidRavenCommand.h"

namespace ravendb::client::documents::commands
{
	class HiLoReturnCommand : public http::VoidRavenCommand
	{
	private:
		const std::string _tag;
		const int64_t _last;
		const int64_t _end;

	public:
		~HiLoReturnCommand() override = default;

		HiLoReturnCommand(std::string tag, int64_t last, int64_t end);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;
	};
}
