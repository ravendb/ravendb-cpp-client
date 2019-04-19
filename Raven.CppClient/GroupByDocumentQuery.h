#pragma once
#include "IGroupByDocumentQuery.h"
#include "AbstractDocumentQuery.h"

namespace ravendb::client::documents::session
{
	template<typename T>
	class DocumentQuery;
}

namespace ravendb::client::documents::session
{
	template<typename T>
	class GroupByDocumentQuery :
		public IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>
	{
	private:
		std::shared_ptr<DocumentQuery<T>> _query{};
		std::weak_ptr<GroupByDocumentQuery> _weak_this{};

		explicit GroupByDocumentQuery(std::shared_ptr<DocumentQuery<T>> query);

	public:
		static std::shared_ptr<GroupByDocumentQuery> create(std::shared_ptr<DocumentQuery<T>> query);
		
		std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> select_key(
			const std::optional<std::string>& field_name = {}, const std::optional<std::string>& projected_name = {});

		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> select_sum(const std::vector<GroupByField>& fields);
			
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> select_count(const std::optional<std::string>& projected_name = "count");
	};

#include "DocumentQuery.h"

	template <typename T>
	GroupByDocumentQuery<T>::GroupByDocumentQuery(std::shared_ptr<DocumentQuery<T>> query)
		: _query(query)
	{}

	template <typename T>
	std::shared_ptr<GroupByDocumentQuery<T>> GroupByDocumentQuery<T>::create(std::shared_ptr<DocumentQuery<T>> query)
	{
		auto object = std::shared_ptr<GroupByDocumentQuery>(new GroupByDocumentQuery(query));
		object->_weak_this = object;

		return object;
	}

	template <typename T>
	std::shared_ptr<IGroupByDocumentQuery<T, GroupByDocumentQuery<T>>> GroupByDocumentQuery<T>::select_key(
		const std::optional<std::string>& field_name, const std::optional<std::string>& projected_name)
	{
		std::static_pointer_cast<AbstractDocumentQuery<T>>(_query)->_group_by_key(field_name, projected_name);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> GroupByDocumentQuery<T>::select_sum(
		const std::vector<GroupByField>& fields)
	{
		if(fields.empty())
		{
			throw std::invalid_argument("fields cannot be empty");
		}

		for(const auto& field : fields)
		{
			std::static_pointer_cast<AbstractDocumentQuery<T>>(_query)->_group_by_sum(
				field.field_name, field.projected_name);
		}

		return _query;
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> GroupByDocumentQuery<T>::select_count(
		const std::optional<std::string>& projected_name)
	{
		std::static_pointer_cast<AbstractDocumentQuery<T>>(_query)->_group_by_count(projected_name);

		return _query;
	}
}
