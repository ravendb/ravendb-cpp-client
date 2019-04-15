#pragma once
#include "IQueryBase.h"
#include "IDocumentQueryBaseSingle.h"
#include "IEnumerableQuery.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IRawDocumentQuery : 
		public IQueryBase<T, TThis>, 
		public IDocumentQueryBaseSingle<T, TThis>,
		public IEnumerableQuery<T, TThis>
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IRawDocumentQuery, TThis>,
				"'TThis' should be derived from IRawDocumentQuery");
			return static_cast<TThis*>(this);
		}

	protected:
		IRawDocumentQuery() = default;

	public:
	};
}
