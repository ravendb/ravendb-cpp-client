#pragma once
#include "JavaScriptException.h"

namespace ravendb::client::exceptions::documents::patching
{
	class JavaScriptParseException : public JavaScriptException
	{
	public:
		~JavaScriptParseException() override = default;

		JavaScriptParseException();

		explicit JavaScriptParseException(const std::string& message);
	};
}
