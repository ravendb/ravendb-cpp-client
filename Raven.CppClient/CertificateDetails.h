#pragma once

namespace ravendb::client::impl
{
	struct CertificateDetails
	{
		std::string certificate{};
		std::string key{};
		std::optional<std::string> key_password{};
	};
}
