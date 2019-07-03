#include "stdafx.h"
#include "IncludeBuilderBase.h"

namespace ravendb::client::documents::session::loaders
{
	IncludeBuilderBase::IncludeBuilderBase(std::shared_ptr<conventions::DocumentConventions> conventions)
		: _conventions(conventions)
	{}

	void IncludeBuilderBase::_include_documents(std::string path)
	{
		documents_to_include.insert(std::move(path));
	}
}
