#pragma once

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IEnumerableQuery
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IEnumerableQuery, TThis>,
				"'TThis' should be derived from IEnumerableQuery");
			return static_cast<TThis*>(this);
		}

	protected:
		IEnumerableQuery() = default;

	public:
		std::vector<std::shared_ptr<T>> to_list();
	};

	template <typename T, class TThis>
	std::vector<std::shared_ptr<T>> IEnumerableQuery<T, TThis>::to_list()
	{
		return cast_down()->to_list();
	}
}
