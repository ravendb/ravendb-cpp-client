#pragma once

namespace ravendb::client
{
	namespace http
	{
		template<typename TResult>
		class RavenCommand;
	}
	namespace documents::conventions
	{
		class DocumentConventions;
	}
}

namespace ravendb::client::serverwide::operations
{
	template<typename TResult>
	struct IServerOperation
	{
		virtual ~IServerOperation() = 0;

		//using std::unique_ptr for polymorphism
		virtual std::unique_ptr<http::RavenCommand<TResult>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) = 0;
	};

	template <typename TResult>
	IServerOperation<TResult>::~IServerOperation() = default;
}
