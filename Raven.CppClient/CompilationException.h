#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::compilation
{
	class CompilationException : public RavenException
	{
	public:
		~CompilationException() override = default;

		CompilationException();

		explicit CompilationException(const std::string& message);
	};
}
