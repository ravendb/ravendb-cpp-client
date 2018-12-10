#pragma once
#include "stdafx.h"
#include "RavenCommand.h"

namespace ravendb::client::http
{
	using VoidRavenCommandBase = RavenCommand<void*>;

	class VoidRavenCommand : public VoidRavenCommandBase
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
	inline VoidRavenCommand::~VoidRavenCommand() = default;
}

