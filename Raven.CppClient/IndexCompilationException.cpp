#include "stdafx.h"
#include "IndexCompilationException.h"

namespace ravendb::client::exceptions::documents::compilation
{
	IndexCompilationException::IndexCompilationException() = default;

	IndexCompilationException::IndexCompilationException(const std::string& message)
		: CompilationException(message)
	{}
}
