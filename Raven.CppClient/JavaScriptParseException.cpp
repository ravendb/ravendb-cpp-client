#include "stdafx.h"
#include "JavaScriptParseException.h"

namespace ravendb::client::exceptions::documents::patching
{
	JavaScriptParseException::JavaScriptParseException() = default;

	JavaScriptParseException::JavaScriptParseException(const std::string& message)
		: JavaScriptException(message)
	{}
}
