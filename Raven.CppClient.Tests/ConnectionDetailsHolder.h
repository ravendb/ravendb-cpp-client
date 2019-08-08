#pragma once
#include "CertificateDetails.h"

#ifdef _MSC_VER
	#include <filesystem>
	using namespace std::filesystem;
#else
	#include <experimental/filesystem>
	using namespace std::experimental::filesystem;
#endif

namespace ravendb::client::tests::infrastructure
{
	class ConnectionDetailsHolder
	{
	private:
		std::string url{};
		impl::CertificateDetails cert_details{};

	public:
		~ConnectionDetailsHolder();

		void GetEnvVariableValue(char*& value, size_t& sz, path& filename);

		ConnectionDetailsHolder(const std::string& def_file_name, bool has_certificate);

		const impl::CertificateDetails& get_certificate_details() const;

		const std::string& get_url() const;
	};
}
