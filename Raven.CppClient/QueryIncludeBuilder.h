#pragma once
#include "IncludeBuilderBase.h"
#include "IQueryIncludeBuilder.h"

namespace ravendb::client::documents::session::loaders
{
	class QueryIncludeBuilder :
		public IncludeBuilderBase,
		public IQueryIncludeBuilder<QueryIncludeBuilder>
	{
	private:
		std::weak_ptr<QueryIncludeBuilder> _weak_this;

	private:
		QueryIncludeBuilder(std::shared_ptr<conventions::DocumentConventions> conventions);

	public:
		static std::shared_ptr<QueryIncludeBuilder> create(std::shared_ptr<conventions::DocumentConventions> conventions);

		std::shared_ptr<IQueryIncludeBuilder<QueryIncludeBuilder>> include_documents(std::string path);
	};
}
