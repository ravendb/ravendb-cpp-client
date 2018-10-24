#pragma once


#include "stdafx.h"
#include "utils.h"
#include "Topology.h"


namespace raven
{

	template<typename TResult>
	class RavenCommand
	{

	protected:
		TResult _result;

	public:
		virtual ~RavenCommand() = 0;

		virtual void createRequest(const ServerNode& node, std::string& url, CURL* curl) const = 0;

		virtual void setResponse(CURL* curl, const nlohmann::json& response) = 0;

		virtual void setResponseNotFound(CURL* curl)
		{
			/*_result = TResult();*/
		}

		TResult getResult() const
		{
			return _result;
		}
	};

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;
}
	

	
