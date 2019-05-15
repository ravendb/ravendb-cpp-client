#include "stdafx.h"
#include "JavaScriptException.h"

namespace ravendb::client::exceptions::documents::patching
{
	JavaScriptException::JavaScriptException() = default;

	JavaScriptException::JavaScriptException(const std::string& message)
		: RavenException(message)
	{}
}
