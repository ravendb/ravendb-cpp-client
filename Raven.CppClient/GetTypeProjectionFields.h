#pragma once
#include <typeindex>
#include <unordered_map>
#include <shared_mutex>
#include <string>
#include <vector>

namespace ravendb::client::impl::utils
{
	class GetTypeProjectionFields
	{
	private:
		static std::shared_mutex _projection_fields_mutex;
		static std::unordered_map<std::type_index, std::vector<std::string>> _projection_fields;

		template <typename TProjection>
		static const std::vector<std::string>& _get_projection_fields_impl();

	public:
		template <typename TProjection>
		static const std::vector<std::string>& get_projection_fields();

		template <typename TProjection>
		const std::vector<std::string>& operator()() const;
	};

	template <typename TProjection>
	const std::vector<std::string>& GetTypeProjectionFields::_get_projection_fields_impl()
	{
		static_assert(std::is_default_constructible_v<TProjection>, "'TProjection' must be default constructible.");

		auto lock = std::unique_lock(_projection_fields_mutex);
		if (auto it = _projection_fields.find(typeid(TProjection));
			it != _projection_fields.end())
		{
			return it->second;
		}

		nlohmann::json sample = TProjection();

		std::vector<std::string> fields{};
		if (sample.is_object())
		{
			fields.reserve(sample.size());
			for (const auto& item : sample.items())
			{
				fields.push_back(item.key());
			}
		}

		auto res = _projection_fields.emplace(typeid(TProjection), std::move(fields));
		return res.first->second;
	}

	template <typename TProjection>
	const std::vector<std::string>& GetTypeProjectionFields::get_projection_fields()
	{
		{
			auto lock = std::shared_lock(_projection_fields_mutex);
			if(auto it = _projection_fields.find(typeid(TProjection));
				it != _projection_fields.end())
			{
				return it->second;
			}
		}
		return _get_projection_fields_impl<TProjection>();
	}

	template <typename TProjection>
	const std::vector<std::string>& GetTypeProjectionFields::operator()() const
	{
		return get_projection_fields<TProjection>();
	}
}
