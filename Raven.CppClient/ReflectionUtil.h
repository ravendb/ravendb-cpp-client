#pragma once
#include <typeindex>

namespace ravendb::client::impl
{
	class ReflectionUtil
	{
	private:
		static std::unordered_map<std::type_index, std::string> full_name_cache;

	public:
		ReflectionUtil() = delete;
		~ReflectionUtil() = delete;

		static const std::string& get_full_name_without_version(std::type_index entity_type);
	};
}
