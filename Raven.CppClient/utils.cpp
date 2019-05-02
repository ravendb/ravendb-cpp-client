#include "stdafx.h"
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "utils.h"
#include "CertificateDetails.h"

namespace ravendb::client::impl::utils
{
	CURLcode utils::sslctx_function(CURL* , void *sslctx_void, void *cert_details_void)
	{
		static const auto my_BIO_free = [&](BIO* bio) {if (bio != nullptr) BIO_vfree(bio); };
		static const auto my_X509_free = [&](X509* cert) {if (cert != nullptr) X509_free(cert); };
		static const auto my_RSA_free = [&](RSA *rsa) {if (rsa != nullptr) RSA_free(rsa); };

		auto sslctx = static_cast<SSL_CTX*>(sslctx_void);
		const CertificateDetails& cert_details = *static_cast<const CertificateDetails*>(cert_details_void);

		constexpr size_t ERR_BUF_SIZE = 1024;
		char error_buffer[ERR_BUF_SIZE]{};

		//create a bio for the certificate
		auto cert_bio = std::unique_ptr < BIO, decltype(my_BIO_free) >
			(BIO_new_mem_buf(cert_details.certificate.c_str(), (int)cert_details.certificate.length()),
			 my_BIO_free);
		if (!cert_bio)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("BIO_new_mem_buf failed ") + error_buffer);
		}

		auto cert = std::unique_ptr < X509, decltype(my_X509_free) >
			(PEM_read_bio_X509(cert_bio.get(), nullptr, nullptr, nullptr), my_X509_free);
		if (!cert)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("PEM_read_bio_X509 failed ") + error_buffer);
		}

		//tell SSL to use the X509 certificate
		if (SSL_CTX_use_certificate(sslctx, cert.get()) != 1)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("Use certificate failed ") + error_buffer);
		}

		//create a bio for the RSA key
		auto key_bio = std::unique_ptr < BIO, decltype(my_BIO_free) >
			(BIO_new_mem_buf(cert_details.key.c_str(), (int)cert_details.key.length()), my_BIO_free);
		if (!key_bio)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("BIO_new_mem_buf failed ") + error_buffer);
		}

		//read the key bio into an RSA object
		auto rsa = std::unique_ptr < RSA, decltype(my_RSA_free) >
			(PEM_read_bio_RSAPrivateKey(key_bio.get(), nullptr, nullptr,
				const_cast<char*>(cert_details.key_password.empty() ? nullptr : cert_details.key_password.c_str())),
				my_RSA_free);
		if (!rsa)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("Failed to create key bio ") + error_buffer);
		}

		//tell SSL to use the RSA key
		if (SSL_CTX_use_RSAPrivateKey(sslctx, rsa.get()) != 1)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("Use Key failed") + error_buffer);
		}

		const char* ca_path = cert_details.ca_path.empty() ? DEFAULT_CA_BUNDLE_PATH :
			reinterpret_cast<const char*>(cert_details.ca_path.c_str());
		//tell SSL to use CA certificate (if provided)
		if(SSL_CTX_load_verify_locations(sslctx, ca_path, nullptr) != 1)
		{
			ERR_error_string_n(ERR_peek_last_error(), error_buffer, ERR_BUF_SIZE);
			throw std::runtime_error(std::string("Use CA path failed") + error_buffer);
		}

		// all set to go
		return CURLE_OK;
	}

	bool is_blank(const std::string& str)
	{
		return std::all_of(str.begin(), str.end(),
			[](unsigned char c)->bool {return std::isspace(c); });
	}
}

bool operator==(const std::tm& lhs, const std::tm& rhs)
{
	return lhs.tm_sec == rhs.tm_sec &&
		lhs.tm_min == rhs.tm_min &&
		lhs.tm_hour == rhs.tm_hour &&
		lhs.tm_mday == rhs.tm_mday &&
		lhs.tm_mon == rhs.tm_mon &&
		lhs.tm_year == rhs.tm_year;
}

bool operator!=(const std::tm& lhs, const std::tm& rhs)
{
	return !(lhs == rhs);
}
