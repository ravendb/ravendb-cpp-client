#pragma once

#include "stdafx.h"

#include "types.h"
#include "utils.h"
#include "Topology.h"
#include "RavenCommand.h"

namespace ravenDB
{
	struct CurlGlobalRAII final
	{
		CurlGlobalRAII()
		{
			auto res = curl_global_init(CURL_GLOBAL_ALL);
			if (res)
			{
				throw std::runtime_error("curl_global_init failed !");
			}
		}

		~CurlGlobalRAII() { curl_global_cleanup(); }
	};

	//makes global initiations
	//call make_global_initializations() in start of your application
	class RavenGlobalInit final
	{
	public:

		static void make_global_initializations()
		{
			//TODO put here more functions/static RAII objects for global initializations.
			static CurlGlobalRAII cgr;
		}

		RavenGlobalInit() = delete;
	};
}
