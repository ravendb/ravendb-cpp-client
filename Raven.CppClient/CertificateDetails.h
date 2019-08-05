#pragma once

#if __cplusplus == 201703L
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
#if __cplusplus == 201703L
		std::filesystem::path ca_path{};
#else
        std::experimental::filesystem::path ca_path{};
#endif
	};
}
