#pragma once
#include "IAdvancedSessionOperations.h"
#include "DocumentInfo.h"

namespace ravendb::client::documents::session
{
	struct IDocumentSession
	{
		virtual ~IDocumentSession() = 0;

		std::reference_wrapper<IAdvancedSessionOperations> advanced();

		virtual void delete_entity(std::shared_ptr<void> entity) = 0;

		virtual void delete_entity(const std::string& id) = 0;

		virtual void delete_entity(const std::string& id, const std::string& expected_change_vector) = 0;

		virtual void save_changes() = 0;

		virtual void store(std::shared_ptr<void> entity) = 0;

	protected:
		virtual std::shared_ptr<void> load(const std::string& id, DocumentInfo::FromJsonConverter from_json) = 0;
	public:
		template<typename T>
		friend std::shared_ptr<T> load(IDocumentSession& session, const std::string& id)
		{
			return std::static_pointer_cast<T>(session.load(id, DocumentInfo::default_from_json<T>));
		}

	};

	inline IDocumentSession::~IDocumentSession() = default;

}
