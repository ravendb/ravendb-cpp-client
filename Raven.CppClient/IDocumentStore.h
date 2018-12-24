#pragma once

namespace ravendb::client::documents
{
	struct IDocumentStore
	{
		virtual ~IDocumentStore() = 0;
	};

	inline IDocumentStore::~IDocumentStore() = default;

}
