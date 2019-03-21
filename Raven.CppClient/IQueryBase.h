#pragma once

namespace ravendb::client::documents::queries
{
	enum class QueryOperator;
}

namespace ravendb::client::documents::session
{
	template<class TThis>
	class IQueryBase
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IQueryBase, TThis>,
				"'TThis' should be derived from IQueryBase");
			return static_cast<TThis*>(this);
		}

	protected:
		IQueryBase() = default;

	public:
		std::shared_ptr<DocumentConventions> get_conventions() const
		{
			return cast_down()->get_conventions();
		}

		std::shared_ptr<TThis> skip(int32_t count)
		{
			return cast_down()->skip(count);
		}

		std::shared_ptr<TThis> take(int32_t count)
		{
			return cast_down()->take(count);
		}

		std::shared_ptr<TThis> no_tracking()
		{
			return cast_down()->no_tracking();
		}

		std::shared_ptr<TThis> no_caching()
		{
			return cast_down()->no_caching();
		}

		std::shared_ptr<TThis> using_default_operator(queries::QueryOperator query_operator)
		{
			return cast_down()->using_default_operator(query_operator);
		}

		std::shared_ptr<TThis> add_parameter(std::string name, nlohmann::json value)
		{
			return cast_down()->add_parameter(std::move(name), std::move(value));
		}
	};

}
