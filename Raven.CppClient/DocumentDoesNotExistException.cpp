#include "stdafx.h"
#include "DocumentDoesNotExistException.h"

namespace ravendb::client::exceptions::documents
{
	DocumentDoesNotExistException::DocumentDoesNotExistException() = default;

	DocumentDoesNotExistException::DocumentDoesNotExistException(const std::string& id)
		: RavenException([&]()->std::string
	{
		std::ostringstream msg{};
		msg << "Document '" << id << "' does not exist.";
		return msg.str();
	}())
	{}
}
