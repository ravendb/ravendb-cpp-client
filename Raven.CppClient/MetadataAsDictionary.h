#pragma once
#include "nlohmann/json.hpp"
#include <any>

namespace ravendb::client::json
{
	class MetadataDictionaryArray;

	class MetadataAsDictionary
	{
	private:
		using InnerMap = std::unordered_map<std::string, std::any>;

	public:
		using const_iterator = InnerMap::const_iterator;
		using iterator = InnerMap::iterator;

	private:
		InnerMap _metadata{};

		bool _dirty{ false };

	private:
		void update_from_source(const nlohmann::json& source);

		static std::any convert_value(const nlohmann::json& value);

		void set_dirty();
		
	public:
		MetadataAsDictionary(const nlohmann::json& metadata = {});

		bool is_dirty() const;

		template<typename T>
		bool insert_or_assign(const std::string& key, T&& value);

		//all 'get_as' methods throw if the element doesn't exists -> use contains() first.
		template<typename T>
		const T& get_as(const std::string& key) const;
		template<typename T>
		T& get_as(const std::string& key);

		const MetadataAsDictionary& get_as_dict(const std::string& key) const;
		MetadataAsDictionary& get_as_dict(const std::string& key);

		const MetadataDictionaryArray& get_as_array(const std::string& key) const;
		MetadataDictionaryArray& get_as_array(const std::string& key);

		bool empty() const;

		size_t size() const;

		void clear();

		bool contains(const std::string& key) const;

		bool erase(const std::string& key);

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;

		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

		static const MetadataAsDictionary& get_dict(const std::any& value);
		static MetadataAsDictionary& get_dict(std::any& value);

		static const MetadataAsDictionary* get_dict(const std::any* value);
		static MetadataAsDictionary* get_dict(std::any* value);
	};

	template <typename T>
	bool MetadataAsDictionary::insert_or_assign(const std::string& key, T&& value)
	{
		set_dirty();

		bool res = false;
		std::tie(std::ignore, res) = _metadata.insert_or_assign(key, std::forward<T>(value));
		return res;
	}

	template <typename T>
	const T& MetadataAsDictionary::get_as(const std::string& key) const
	{
		const auto& res = _metadata.at(key);

		return std::any_cast<const T&>(res);
	}

	template <typename T>
	T& MetadataAsDictionary::get_as(const std::string& key)
	{
		auto& res = _metadata.at(key);

		return std::any_cast<T&>(res);
	}
}

