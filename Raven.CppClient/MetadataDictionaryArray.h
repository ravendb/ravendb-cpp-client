#pragma once
#include "nlohmann/json.hpp"
#include <any>

namespace ravendb::client::json
{
	class MetadataAsDictionary;

	class MetadataDictionaryArray
	{
	private:
		using InnerArrayType = std::vector<std::any>;

	public:
		using const_iterator = InnerArrayType::const_iterator;
		using iterator = InnerArrayType::iterator;

	private:
		InnerArrayType _metadata{};

	public:
		MetadataDictionaryArray() = default;

		template<typename T>
		void push_back(T&& value);

		//all 'get_as' methods throw if the index is out of range -> use size() first.
		template<typename T>
		const T& get_as(size_t index) const;

		template<typename T>
		T& get_as(size_t index);

		const MetadataAsDictionary& get_as_dict(size_t index) const;
		MetadataAsDictionary& get_as_dict(size_t index);

		const MetadataDictionaryArray& get_as_array(size_t index) const;
		MetadataDictionaryArray& get_as_array(size_t index);

		bool empty() const;

		size_t size() const;

		void reserve(size_t new_capacity);

		void clear();

		void erase(size_t index);

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;

		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

		static const MetadataDictionaryArray& get_array(const std::any& value);
		static MetadataDictionaryArray& get_array(std::any& value);

		static const MetadataDictionaryArray* get_array(const std::any* value);
		static MetadataDictionaryArray* get_array(std::any* value);
	};

	template <typename T>
	void MetadataDictionaryArray::push_back(T&& value)
	{
		_metadata.push_back(std::forward<T>(value));
	}

	template <typename T>
	const T& MetadataDictionaryArray::get_as(size_t index) const
	{
		const auto& res = _metadata.at(index);

		return std::any_cast<const T&>(res);
	}

	template <typename T>
	T& MetadataDictionaryArray::get_as(size_t index)
	{
		auto& res = _metadata.at(index);

		return std::any_cast<T&>(res);
	}
}

