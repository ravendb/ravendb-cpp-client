#pragma once

namespace ravendb::client::documents::session
{
	struct MethodCall
	{
		virtual ~MethodCall();;

		std::vector<nlohmann::json> args{};
		std::optional<std::string> access_path{};
	};

	inline MethodCall::~MethodCall() = default;
}
