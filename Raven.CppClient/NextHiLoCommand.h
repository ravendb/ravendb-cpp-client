#pragma once
#include "RavenCommand.h"
#include "HiLoResult.h"

namespace ravendb::client::documents::commands
{
	class NextHiLoCommand : public http::RavenCommand<identity::HiLoResult>
	{
	private:
		const std::string _tag;
		const int64_t _last_batch_size;
		const impl::DateTimeOffset _last_range_at;
		const std::string _identity_parts_separator;
		const int64_t _last_range_max;

	public:
		~NextHiLoCommand() override = default;

		NextHiLoCommand(std::string tag, int64_t last_batch_size, impl::DateTimeOffset last_range_at,
		                std::string identity_part_separator, int64_t last_range_max);


		void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

		bool is_read_request() const noexcept override;
	};
}
