#pragma once

#ifdef _WIN32
#include <filesystem>
#endif
#ifdef  __unix__
#include <experimental/filesystem>
#endif

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
#ifdef _WIN32
		std::filesystem::path ca_path{};
#endif
#ifdef  __unix__
        std::experimental::filesystem::path ca_path{};
#endif
	};
}
