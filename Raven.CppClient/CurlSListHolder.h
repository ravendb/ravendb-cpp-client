#pragma once

namespace ravendb::client::impl
{
	class CurlSListHolder
	{
	private:
		struct curl_slist* _list = nullptr;

	public:
		~CurlSListHolder();

		CurlSListHolder();

		//Iterators  : *it = std::string
		template<typename InputIt>
		CurlSListHolder(InputIt first, InputIt last);

		CurlSListHolder(const CurlSListHolder& other) = delete;

		CurlSListHolder(CurlSListHolder&& other) noexcept;

		CurlSListHolder& operator=(const CurlSListHolder& other) = delete;

		CurlSListHolder& operator=(CurlSListHolder&& other) noexcept;

		struct curl_slist* get() const noexcept;

		//the string is copied
		void append(const std::string& str);

		//the string is copied
		void append(const char* str);

		void free_list();
	};

	template<typename InputIt>
	CurlSListHolder::CurlSListHolder(InputIt first, InputIt last)
	{
		static_assert(std::is_same_v<std::remove_cv_t<typename std::iterator_traits<InputIt>::value_type>, std::string>, "invalid iterator type");

		std::for_each(first, last, [this](const std::string& str) {this->append(str); });
	}
}
