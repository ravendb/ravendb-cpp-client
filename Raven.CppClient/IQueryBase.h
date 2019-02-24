#pragma once

namespace ravendb::client::documents::queries
{
	enum class QueryOperator;
}

namespace ravendb::client::documents::session
{
	template<typename TThis>
	class IQueryBase
	{
	private:
		TThis* cast_to_derived()
		{
			return static_cast<TThis*>(this);
		}

	public:
		std::shared_ptr<TThis> skip(int32_t count)
		{
			return cast_to_derived()->skip(count);
		}

		std::shared_ptr<TThis> take(int32_t count)
		{
			return cast_to_derived()->take(count);
		}

		std::shared_ptr<TThis> no_tracking()
		{
			return cast_to_derived()->no_tracking();
		}

		std::shared_ptr<TThis> no_caching()
		{
			return cast_to_derived()->no_caching();
		}

		std::shared_ptr<TThis> using_default_operator(queries::QueryOperator query_operator)
		{
			return cast_to_derived()->using_default_operator(query_operator);
		}

		std::shared_ptr<TThis> add_parameter(std::string name, nlohmann::json value)
		{
			return cast_to_derived()->add_parameter(std::move(name), std::move(value));
		}
	};

}
