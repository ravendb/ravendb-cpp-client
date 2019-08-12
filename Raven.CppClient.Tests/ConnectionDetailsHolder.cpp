#include "pch.h"
#include "ConnectionDetailsHolder.h"
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>


#include <finally.hpp>

namespace ravendb::client::tests::infrastructure
{
	static inline void rtrim(std::string &s, char trimDelimiter) 
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [trimDelimiter](int ch) { return ch != trimDelimiter; }).base(), s.end());
	}

	ConnectionDetailsHolder::~ConnectionDetailsHolder() = default;

	ConnectionDetailsHolder::ConnectionDetailsHolder(const std::string& def_file_name, bool has_certificate = true)
	{
		//open definitions file
		std::ifstream def_file;
		
		try
		{
			auto def_path = absolute(def_file_name).string();

#ifdef _DEBUG
			std::cerr << "Look for config file at: " + def_path << std::endl;
#endif

			def_file = std::ifstream(def_file_name, std::ios::in);
		}
		catch(const std::exception& e) //in Linux this would throw exception if the file doesn't exist
		{
			std::cerr << "Failed to open " + def_file_name + ". Most likely, it was not found... (reason: " + e.what() + ")" <<std::endl;
		}

		if (!def_file) //if we didn't find the file at it's path, first look at the same folder as executing assembly
		{
			auto filename = path(def_file_name).filename();

			def_file = std::ifstream(filename.string()); //try to get at the same folder as the executable
			if(!def_file) //if we didn't find the config at the same file as executing assembly, try environment variables
			{
				std::cerr << "Failed to find the config file at " + filename.string() + ", so now will try to fetch the path from environment variables" << std::endl;

				//try to find the path from environment variable
				char* value = nullptr;
				auto _ = finally([value] { if(value != nullptr) free(value); });				

				value = getenv(path(def_file_name).stem().string().c_str());
				if (value == nullptr)
				{
					throw std::runtime_error(std::string("Failed to get the value for environment variable ") + filename.string());
				}

				if(value != nullptr)
				{
					def_file = std::ifstream(value);
				}
				else
				{
					throw std::runtime_error(std::string("Failed to get the value for environment variable ") + filename.string());
				}
			}
		}

		if(!def_file)
		{
			throw std::runtime_error(std::string("Failed to open the file ") + def_file_name);
		}

		if (!std::getline(def_file, url))
		{
			throw std::runtime_error(std::string("Can't read url from ") + def_file_name);
		}

		rtrim(url, '\r');

		if (!has_certificate)
		{
			return;
		}

		//get certificate
		std::string cert_file_name;
		if (!std::getline(def_file, cert_file_name) || cert_file_name.empty())
		{
			throw std::runtime_error(std::string("Can't read certificate file name from ") + def_file_name);
		}

		rtrim(cert_file_name, '\r');
		std::ifstream cert_file(cert_file_name);
		if (!cert_file)
		{			
			throw std::runtime_error(std::string("Failed to open certificate file at: ") + cert_file_name);
		}
		{
			std::stringstream stream;
			stream << cert_file.rdbuf();
			cert_details.certificate = stream.str();
		}
		//get key
		std::string key_file_name;
		if (!std::getline(def_file, key_file_name) || key_file_name.empty())
		{
			throw std::runtime_error(std::string("Can't read key file name from ") + def_file_name);
		}

		rtrim(key_file_name, '\r');
		
		std::ifstream key_file(key_file_name);
		if (!key_file)
		{
			throw std::runtime_error(std::string("Failed to open key file at: ") + key_file_name);
		}
		{
			std::stringstream stream;
			stream << key_file.rdbuf();
			cert_details.key = stream.str();
		}
		//get CA file path(optional)
		std::string CA_path_file_name;
		if (!std::getline(def_file, CA_path_file_name))
		{
			return;//defaults will be used
		}
		//no problem if CA_path_file_name.empty() == true
		cert_details.ca_path = CA_path_file_name;

		//get password(optional)
		std::string key_password;
		if (!std::getline(def_file, key_password))
		{
			return;//no password
		}
		//no problem if  key_password.empty() == true
		cert_details.key_password = key_password;
	}

	const impl::CertificateDetails& ConnectionDetailsHolder::get_certificate_details() const
	{
		return cert_details;
	}

	const std::string& ConnectionDetailsHolder::get_url() const
	{
		return url;
	}
}
