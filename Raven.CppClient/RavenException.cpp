#include "stdafx.h"
#include "RavenException.h"

namespace ravendb::client::exceptions
{
	RavenException::RavenException()
		: std::runtime_error("")
	{}

	RavenException::RavenException(const std::string& message)
		: std::runtime_error(message)
	{}

	bool RavenException::is_reached_leader() const
	{
		return reached_leader;
	}

	void RavenException::set_reached_leader(bool reached_leader)
	{
		this->reached_leader = reached_leader;
	}

	RavenException RavenException::generic(const std::string& error, const std::string& json)
	{
		std::ostringstream msg{};
		msg << error << ". Response: " << json;
		return RavenException(msg.str());
	}
}
