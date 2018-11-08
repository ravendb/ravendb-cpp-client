#pragma once
#include "RavenCommand.h"

namespace ravendb
{
	class VoidRavenCommand : public RavenCommand<void*>
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

