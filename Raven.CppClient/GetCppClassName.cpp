#include "stdafx.h"
#include "GetCppClassName.h"

namespace ravendb::client::impl::utils
{
	std::unordered_map<std::type_index, std::string> GetCppClassName::_classes_names{};

#ifdef _WIN32
	std::string GetCppClassName::_get_class_name_impl(std::type_index type)
	{
		std::string_view win_class_name = type.name();
		auto first_space_pos = win_class_name.find_first_of(' ');

		std::string res = first_space_pos != std::string_view::npos ? win_class_name.data() + first_space_pos + 1 : win_class_name.data();
		_classes_names.insert_or_assign(type, res);

		return std::move(res);
	}
#endif

#ifdef  __unix__
#define _GNU_SOURCE
#include <cxxabi.h>

	std::string GetCppClassName::_get_class_name_impl(std::type_index type)
	{
		const char* mangled_class_name = type.name();

		int status{};
		char* demangle_res = abi::__cxa_demangle(mangled_class_name, nullptr, 0, &status);
		std::string demangled_class_name = demangle_res;
		free(demangle_res);

		if (status != 0)
		{
			std::ostringstream msg{};
			msg << "demangling of " << mangled_class_name << " failed";
			throw std::invalid_argument(msg.str());
		}
		_classes_names.insert_or_assign(type, demangled_class_name);
		return std::move(demangled_class_name);
	}
#endif

	std::string GetCppClassName::get_class_name(std::type_index type)
	{
		if (auto it = _classes_names.find(type);
			it != _classes_names.end())
		{
			return it->second;
		}

		return _get_class_name_impl(type);
	}

	std::string GetCppClassName::operator()(std::type_index type) const
	{
		return get_class_name(type);
	}
}
