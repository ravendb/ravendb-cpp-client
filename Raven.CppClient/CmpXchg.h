#pragma once
#include "MethodCall.h"

namespace ravendb::client::documents::session
{
	struct CmpXchg : MethodCall
	{
		~CmpXchg() override = default;

		explicit CmpXchg(std::string key)
		{
			args.push_back(std::move(key));
		}
	};
}
