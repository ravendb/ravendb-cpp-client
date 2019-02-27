#pragma once
#include "GetRequest.h"
#include "DocumentConventions.h"
#include "IndexQuery.h"

namespace ravendb::client::documents::session::operations::lazy
{
	class IndexQueryContent : public commands::multi_get::GetRequest::IContent
	{
	private:
		const conventions::DocumentConventions _conventions;
		const queries::IndexQuery _query;

	public:
		~IndexQueryContent() override = default;

		IndexQueryContent(conventions::DocumentConventions conventions, queries::IndexQuery query)
			: _conventions(std::move(conventions))
			, _query(std::move(query))
		{}

		void write_content(nlohmann::json& json) const override
		{
			json = _query;
		}
	};
}
