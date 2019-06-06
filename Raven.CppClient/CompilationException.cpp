#include "stdafx.h"
#include "CompilationException.h"

namespace ravendb::client::exceptions::compilation
{
	CompilationException::CompilationException() = default;

	CompilationException::CompilationException(const std::string& message)
		: RavenException(message)
	{}
}
