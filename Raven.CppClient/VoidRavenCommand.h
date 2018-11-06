#pragma once
#include "RavenCommand.h"

namespace ravenDB
{
	class VoidRavenCommand : public RavenCommand<int>
	{
	protected:
		VoidRavenCommand()
		{
			this->_response_type = RavenCommandResponseType::EMPTY;
		}

	public:
		~VoidRavenCommand() override = 0;

		bool is_read_request() const noexcept override
		{
			return false;
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{}
	};
}

