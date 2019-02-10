#pragma once
#include "Supplier.h"
#include <shared_mutex>

namespace ravendb::client::documents
{
	template<typename T>
	class Lazy
	{
	private:
		const std::shared_ptr<impl::Supplier<T>> _value_factory{};
		mutable std::shared_mutex _m{};
		mutable std::shared_ptr<T> _value{};

	public:
		explicit Lazy(std::shared_ptr<impl::Supplier<T>> value_factory)
			: _value_factory([&]()
		{
			if(!value_factory)
			{
				throw std::invalid_argument("value_factory can't be empty");
			}
			return value_factory;
		}())
		{}

		bool is_value_created() const
		{
			std::shared_lock<std::shared_mutex> lock(_m);
			return _value;
		}

		std::shared_ptr<T> get_value() const
		{
			{
				std::shared_lock<std::shared_mutex> lock(_m);
				if(_value)
				{
					return _value;
				}
			}
			{
				std::unique_lock<std::shared_mutex> lock(_m);
				if (!_value)
				{
					_value = _value_factory.get();
				}
				return _value;
			}
		}
	};
}
