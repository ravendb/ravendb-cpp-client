#pragma once
#include <filesystem>

namespace ravendb::client::impl
{
	struct CertificateDetails
	{
		std::string certificate{};
		std::string key{};

		//optional , may be empty
		std::string key_password{};

		//full path including the filename
		//optional , may be empty
		std::filesystem::path ca_path{};
	};
}
