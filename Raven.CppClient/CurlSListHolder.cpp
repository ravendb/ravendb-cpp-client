#include "stdafx.h"
#include "CurlSListHolder.h"

namespace ravendb::client::impl
{
	CurlSListHolder::~CurlSListHolder()
	{
		free_list();
	}

	CurlSListHolder::CurlSListHolder() = default;

	CurlSListHolder::CurlSListHolder(CurlSListHolder && other) noexcept
		: _list(other._list)
	{
		other._list = nullptr;
	}

	CurlSListHolder & CurlSListHolder::operator=(CurlSListHolder && other) noexcept
	{
		if (this != &other)
		{
			free_list();
			_list = other._list;
			other._list = nullptr;
		}
		return *this;
	}

	curl_slist * CurlSListHolder::get() const noexcept
	{
		return _list;
	}

	void CurlSListHolder::append(const std::string & str)
	{
		append(str.c_str());
	}

	void CurlSListHolder::append(const char * str)
	{
		auto temp = curl_slist_append(_list, str);
		if (temp == nullptr)
		{
			throw std::runtime_error("can't append the string to the list : " + std::string(str));
		}
		_list = temp;
	}

	void CurlSListHolder::free_list()
	{
		curl_slist_free_all(_list);
		_list = nullptr;
	}
}