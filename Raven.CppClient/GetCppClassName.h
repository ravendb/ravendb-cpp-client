#pragma once
#include <typeindex>
#include <unordered_map>

namespace ravendb::client::impl::utils
{
	class GetCppClassName
	{
	private:
		static std::unordered_map<std::type_index, std::string> _classes_names;

		static std::string _get_class_name_impl(std::type_index type);

	public:
		static std::string get_class_name(std::type_index type);

		static std::string get_simple_class_name(std::type_index type);

		std::string operator()(std::type_index type) const;
	};
}
