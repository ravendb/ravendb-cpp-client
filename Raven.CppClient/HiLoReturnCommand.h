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

		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;
	};
}
