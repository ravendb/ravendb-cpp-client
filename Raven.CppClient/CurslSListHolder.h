#pragma once
#include "stdafx.h"

namespace ravendb::client::impl
{
	class CurlSlistHolder
	{
	private:
		struct curl_slist* _list = nullptr;

	public:
		~CurlSlistHolder()
		{
			free_list();
		}

		CurlSlistHolder() = default;

		//Iterators  : *it = std::string
		template<typename InputIt>
		CurlSlistHolder(InputIt first, InputIt last)
		{
			std::for_each(first, last, [this](const std::string& str) {append(str); });
		}

		CurlSlistHolder(const CurlSlistHolder& other) = delete;

		CurlSlistHolder(CurlSlistHolder&& other) noexcept
			: _list(other._list)
		{
			other._list = nullptr;
		}

		CurlSlistHolder& operator=(const CurlSlistHolder& other) = delete;

		CurlSlistHolder& operator=(CurlSlistHolder&& other) noexcept
		{
			if (this != &other)
			{
				free_list();
				_list = other._list;
				other._list = nullptr;
			}
			return *this;
		}

		struct curl_slist* get() const noexcept
		{
			return _list;
		}
		//the string is copied
		void append(const std::string& str)
		{
			append(str.c_str());
		}
		//the string is copied
		void append(const char* str)
		{
			auto temp = curl_slist_append(_list, str);
			if (temp == nullptr)
			{
				throw std::runtime_error("can't append the string to the list : " + std::string(str));
			}
			_list = temp;
		}

		void free_list()
		{
			curl_slist_free_all(_list);
			_list = nullptr;
		}
	};
}
