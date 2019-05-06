#pragma once
#include "RavenCommand.h"

namespace ravendb::client::http
{
	using VoidRavenCommandBase = RavenCommand<void>;

	class VoidRavenCommand : public VoidRavenCommandBase
	{
	protected:
		VoidRavenCommand();

	public:
		~VoidRavenCommand() override = 0;

		bool is_read_request() const override;

		//TODO check if necessary
		//void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;
	};

	inline VoidRavenCommand::VoidRavenCommand()
	{
		this->_response_type = RavenCommandResponseType::EMPTY;
	}

	inline VoidRavenCommand::~VoidRavenCommand() = default;

	inline bool VoidRavenCommand::is_read_request() const
	{
		return false;
	}

	//inline void VoidRavenCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	//{
	//	_result.reset();
	//}
}

