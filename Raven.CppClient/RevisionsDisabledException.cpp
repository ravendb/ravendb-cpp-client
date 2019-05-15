#include "stdafx.h"
#include "RevisionsDisabledException.h"

namespace ravendb::client::exceptions::documents::revisions
{
	RevisionsDisabledException::RevisionsDisabledException() = default;

	RevisionsDisabledException::RevisionsDisabledException(const std::string& message)
		: RavenException(message)
	{}
}
