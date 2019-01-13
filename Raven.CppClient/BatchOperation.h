#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "BatchCommand.h"

namespace ravendb::client::documents::session::operations
{
	class BatchOperation
	{
	private:
		const std::reference_wrapper<InMemoryDocumentSessionOperations> _session;
		std::vector<std::shared_ptr<void>> _entities{};
		size_t _session_commands_count{};
		size_t _all_commands_count{};
		std::unordered_map<std::string, std::shared_ptr<DocumentInfo>> _modifications{};

	public:
		explicit BatchOperation(InMemoryDocumentSessionOperations& session)
			: _session(std::ref(session))
		{}

		std::optional<commands::batches::BatchCommand> create_request()
		{
			auto result = _session.get().prepare_for_save_changes();
			_session_commands_count = result.session_commands.size();
			result.session_commands.insert(result.session_commands.end(), result.deferred_commands.cbegin(), result.deferred_commands.cend());
			
			//TODO _session.get().validate_cluster_transaction();
			_all_commands_count = result.session_commands.size();
			if(_all_commands_count == 0)
			{
				return {};
			}

			_session.get().increment_request_count();
			_entities = result.entities;

			return std::optional<commands::batches::BatchCommand>(std::in_place, /*_session.get().conventions*/ DocumentConventions(),
				result.session_commands, result.options, _session.get().get_transaction_mode() );
		}
	};
}
