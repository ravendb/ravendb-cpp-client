#pragma once
#include "QueryOperator.h"

namespace ravendb::client::documents::session
{
	template<typename TThis>
	class QueryBase
	{
	protected:
		class AbstractInner
		{
			friend QueryBase;

		protected:
			std::shared_ptr<TThis> _query_base_ptr{};

		public:
			virtual ~AbstractInner() = 0;

			virtual void inner_no_caching() = 0;

			virtual void inner_no_tracking() = 0;

			//TODO
			//virtual void timings

			virtual void inner_skip(int32_t count) = 0;

			//TODO
			//virtual void statistics

			virtual void inner_take(int32_t count) = 0;

			virtual void inner_using_default_operator(queries::QueryOperator query_operator) = 0;

			//TODO
			//virtual void wait_for_non_stale_results

			virtual void inner_add_parameter(std::string name, nlohmann::json object) = 0;
		};

		std::unique_ptr<AbstractInner> _inner{};

		explicit QueryBase(std::unique_ptr<AbstractInner> inner)
			: _inner(std::move(inner))
		{}

		void set_this_ptr(std::shared_ptr<TThis> shared_this)
		{
			_inner->_query_base_ptr = shared_this;
		}

	public:
		std::shared_ptr<TThis> skip(int32_t count)
		{
			_inner->inner_skip(count);
			return _inner->_query_base_ptr;
		}

		std::shared_ptr<TThis> take(int32_t count)
		{
			_inner->inner_take(count);
			return _inner->_query_base_ptr;
		}

		std::shared_ptr<TThis> no_tracking()
		{
			_inner->inner_no_tracking();
			return _inner->_query_base_ptr;
		}

		std::shared_ptr<TThis> no_caching()
		{
			_inner->inner_no_caching();
			return _inner->_query_base_ptr;
		}

		std::shared_ptr<TThis> using_default_operator(queries::QueryOperator query_operator)
		{
			_inner->inner_using_default_operator(query_operator);
			return _inner->_query_base_ptr;
		}

		std::shared_ptr<TThis> add_parameter(std::string name, nlohmann::json object)
		{
			_inner->inner_add_parameter(std::move(name), std::move(object));
			return _inner->_query_base_ptr;
		}
	};

	template <typename TSelf>
	QueryBase<TSelf>::AbstractInner::~AbstractInner() = default;

}
