#pragma once
#include <any>

namespace ravendb::client::documents::session
{
	struct IMetadataDictionary
	{
		virtual ~IMetadataDictionary() = 0;

		virtual const std::unordered_map<std::string, std::any>& get_dictionary() const = 0;

		virtual bool is_dirty() const = 0;

		virtual bool insert(std::string key, std::any value) = 0;
	};
	inline IMetadataDictionary::~IMetadataDictionary() = default;

}
