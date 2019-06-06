#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::revisions
{
	class RevisionsDisabledException : public RavenException
	{
	public:
		~RevisionsDisabledException() override = default;

		RevisionsDisabledException();

		explicit RevisionsDisabledException(const std::string& message);
	};
}
