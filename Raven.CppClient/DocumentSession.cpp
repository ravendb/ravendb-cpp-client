#include "stdafx.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::documents::session
{
	std::shared_ptr<DocumentSessionImpl> DocumentSession::get_session_implementation() const
	{
		return _session_impl;
	}

	DocumentSession::~DocumentSession() = default;

	DocumentSession::DocumentSession(std::shared_ptr<DocumentSessionImpl> session_impl)
		: _session_impl(session_impl)
	{}

	AdvancedSessionOperations DocumentSession::advanced()
	{
		return AdvancedSessionOperations(_session_impl);
	}

	const AdvancedSessionOperations DocumentSession::advanced() const
	{
		return AdvancedSessionOperations(_session_impl);
	}

	void DocumentSession::delete_document(const std::string& id, const std::string& expected_change_vector,
	                                      const DocumentInfo::ToJsonConverter& to_json)
	{
		_session_impl->delete_document(id, expected_change_vector, to_json);
	}

	void DocumentSession::save_changes()
	{
		_session_impl->save_changes();
	}

	loaders::LoaderWithInclude DocumentSession::include(const std::string& path)
	{
		return _session_impl->include(path);
	}
}
