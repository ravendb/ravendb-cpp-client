#pragma once
#include "CompilationException.h"

namespace ravendb::client::exceptions::documents::compilation
{
	class IndexCompilationException : public exceptions::compilation::CompilationException
	{
	public:
		std::string index_definition_property{};
		std::string problematic_text{};

		~IndexCompilationException() override = default;

		IndexCompilationException();

		explicit IndexCompilationException(const std::string& message);
	};
}
