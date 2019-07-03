#pragma once
#include <string>
#include <unordered_set>

namespace ravendb::client::documents::conventions
{
	class DocumentConventions;
}

namespace ravendb::client::documents::session::loaders
{
	class IncludeBuilderBase
	{
	protected:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;

	public:
		std::unordered_set<std::string> documents_to_include{};

	protected:
		explicit IncludeBuilderBase(std::shared_ptr<conventions::DocumentConventions> conventions);

		void _include_documents(std::string path);
	};
}
