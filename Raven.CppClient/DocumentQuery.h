#pragma once
#include "IDocumentQuery.h"

namespace ravendb::client::documents::session
{
	template<typename T>
	class DocumentQuery : public AbstractDocumentQuery<T>, public IDocumentQuery<T, DocumentQuery<T>>
	{
	private:
		std::weak_ptr<DocumentQuery> _weak_this{};

		DocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, std::optional<tokens::DeclareToken> declare_token,
			/*std::vector<DeclareToken>*/void* load_tokens, std::optional<std::string> from_alias);

	public:
		static std::shared_ptr<DocumentQuery> create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, std::optional<tokens::DeclareToken> declare_token,
			/*std::vector<DeclareToken>*/void* load_tokens, std::optional<std::string> from_alias);

		static std::shared_ptr<DocumentQuery> create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
			std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by);

		std::shared_ptr<DocumentConventions> get_conventions() const
		{
			return AbstractDocumentQuery<T>::get_conventions();
		}

		std::vector<std::shared_ptr<T>> to_list()
		{
			return 	AbstractDocumentQuery<T>::to_list();
		}

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval)
		{
			return AbstractDocumentQuery<T>::lazily(on_eval);
		}

		std::shared_ptr<DocumentQuery> skip(int32_t count)
		{
			AbstractDocumentQuery<T>::_skip(count);
			return _weak_this.lock();
		}

		std::shared_ptr<DocumentQuery> take(int32_t count)
		{
			AbstractDocumentQuery<T>::_take(count);
			return _weak_this.lock();
		}

		std::shared_ptr<DocumentQuery> order_by(const std::string& field);

		std::shared_ptr<DocumentQuery> order_by(const std::string& field, OrderingType ordering);

	};


	template <typename T>
	DocumentQuery<T>::DocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
		std::optional<std::string> collection_name, bool is_group_by, std::optional<tokens::DeclareToken> declare_token,
		void* load_tokens, std::optional<std::string> from_alias)
		: AbstractDocumentQuery<T>(session, std::move(index_name), std::move(collection_name), is_group_by,
			std::move(declare_token), load_tokens, std::move(from_alias))
	{}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::optional<std::string> index_name, std::optional<std::string> collection_name, bool is_group_by)
	{
		return create(session, std::move(index_name), std::move(collection_name), 
			is_group_by, {}, {}, {});
	}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::order_by(const std::string& field)
	{
		return order_by(field, OrderingType::STRING);
	}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::order_by(const std::string& field, OrderingType ordering)
	{
		AbstractDocumentQuery<T>::_order_by(field, ordering);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<DocumentQuery<T>> DocumentQuery<T>::create(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::optional<std::string> index_name, std::optional<std::string> collection_name, bool is_group_by,
		std::optional<tokens::DeclareToken> declare_token, void* load_tokens, std::optional<std::string> from_alias)
	{
		auto new_object = std::shared_ptr<DocumentQuery<T>>(new DocumentQuery<T>(
			session, std::move(index_name), std::move(collection_name), is_group_by,
			std::move(declare_token), load_tokens, std::move(from_alias)));
		new_object->_weak_this = new_object;

		return new_object;
	}


}
