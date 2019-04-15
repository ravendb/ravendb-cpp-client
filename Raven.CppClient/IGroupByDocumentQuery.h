#pragma once
#include "GroupByField.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IGroupByDocumentQuery
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IGroupByDocumentQuery, TThis>,
				"'TThis' should be derived from IGroupByDocumentQuery");
			return static_cast<TThis*>(this);
		}

	protected:
		IGroupByDocumentQuery() = default;

	public:
		auto select_key(const std::optional<std::string>& field_name = {}, const std::optional<std::string>& projected_name = {});

		auto select_sum(const std::vector<GroupByField>& fields);

		auto select_count(const std::optional<std::string>& projected_name = "count");
	};

	template <typename T, class TThis>
	auto IGroupByDocumentQuery<T, TThis>::select_key(const std::optional<std::string>& field_name,
		const std::optional<std::string>& projected_name)
	{
		return cast_down()->select_key(field_name, projected_name);
	}

	template <typename T, class TThis>
	auto IGroupByDocumentQuery<T, TThis>::select_sum(const std::vector<GroupByField>& fields)
	{
		return cast_down()->select_sum(fields);
	}

	template <typename T, class TThis>
	auto IGroupByDocumentQuery<T, TThis>::select_count(const std::optional<std::string>& projected_name)
	{
		return cast_down()->select_count(projected_name);
	}
}
