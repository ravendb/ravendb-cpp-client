#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::patching
{
	class JavaScriptException : public RavenException
	{
	public:
		~JavaScriptException() override = default;

		JavaScriptException();

		explicit JavaScriptException(const std::string& message);
	};
}
