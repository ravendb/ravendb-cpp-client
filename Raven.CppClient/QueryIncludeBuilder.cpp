#include "stdafx.h"
#include "QueryIncludeBuilder.h"

namespace ravendb::client::documents::session::loaders
{
	QueryIncludeBuilder::QueryIncludeBuilder(std::shared_ptr<conventions::DocumentConventions> conventions)
		: IncludeBuilderBase(conventions)
	{}

	std::shared_ptr<QueryIncludeBuilder> QueryIncludeBuilder::create(
		std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		auto object = std::shared_ptr<QueryIncludeBuilder>(new QueryIncludeBuilder(conventions));
		object->_weak_this = object;
		return object;
	}

	std::shared_ptr<IQueryIncludeBuilder<QueryIncludeBuilder>> QueryIncludeBuilder::include_documents(std::string path)
	{
		IncludeBuilderBase::_include_documents(std::move(path));
		return _weak_this.lock();
	}
}
