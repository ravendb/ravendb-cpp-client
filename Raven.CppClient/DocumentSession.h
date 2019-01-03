#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "LoadOperation.h"

namespace ravendb::client::documents::session
{
	class DocumentSession : public InMemoryDocumentSessionOperations
	{
	public:
		DocumentSession(SessionOptions options)
			: InMemoryDocumentSessionOperations(std::move(options))
		{}

		template<typename T>
		std::shared_ptr<T> load(const std::string& id)
		{
			if (impl::utils::is_blank(id))
			{
				throw std::invalid_argument("Id mast be a non empty string");
			}

			auto load_op = operations::LoadOperation(*this);
			load_op.by_id(id);
			auto cmd = load_op.create_request();

			if (cmd)
			{
				_request_executor->execute(*cmd);
				load_op.set_result(cmd->get_result());
			}

			return load_op.get_document<T>();
		}




	};
}
