#pragma once

#ifdef _MSC_VER 
#include <filesystem>
#else
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
#ifdef __MSC_VER 
		std::filesystem::path ca_path{};
#else
        std::experimental::filesystem::path ca_path{};
#endif
	};
}
