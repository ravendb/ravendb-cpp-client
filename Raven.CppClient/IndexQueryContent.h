#pragma once
#include "GetRequest.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::session::operations::lazy
{
	class IndexQueryContent : public commands::multi_get::GetRequest::IContent
	{
	private:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const queries::IndexQuery _query;

	public:
		~IndexQueryContent() override = default;

		IndexQueryContent(std::shared_ptr<conventions::DocumentConventions> conventions, queries::IndexQuery query)
			: _conventions(conventions)
			, _query(std::move(query))
		{}

		void write_content(nlohmann::json& json) const override
		{
			json = _query;
		}
	};
}
