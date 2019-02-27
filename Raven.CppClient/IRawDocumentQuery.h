#pragma once
#include "IQueryBase.h"
#include "IDocumentQueryBaseSingle.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IRawDocumentQuery : 
		public IQueryBase<TThis>, 
		public IDocumentQueryBaseSingle<T, TThis>
	{
	private:
	TThis* cast_down()
	{
		static_assert(std::is_base_of_v<IDocumentQueryBaseSingle, TThis>,
			"'TThis' should be derived from IRawDocumentQuery");
		return static_cast<TThis*>(this);
	}

	protected:
		IRawDocumentQuery() = default;

	public:
		std::vector<std::shared_ptr<T>> to_list()
		{
			return 	cast_down()->to_list();
		}
};
}
