#pragma once
#include <mutex>

namespace ravendb::client::documents
{
	template<typename T>
	class Lazy
	{
	private:
		class LazyInner
		{
		private:
			const std::function<T()> _value_factory{};
			std::once_flag _flag;
			std::optional<T> _value{};
			std::atomic_bool _is_created = false;

		public:
			~LazyInner() = default;

			explicit LazyInner(const std::function<T()>& value_factory);

			bool is_value_created() const;

			const T& get_value();
		};

		std::shared_ptr<LazyInner> _inner;

	public:
		~Lazy() = default;

		explicit Lazy(const std::function<T()>& value_factory);

		bool is_value_created() const;

		const T& get_value();

		operator const T&();
	};

	template <typename T>
	Lazy<T>::LazyInner::LazyInner(const std::function<T()>& value_factory)
		: _value_factory([&]()
	{
		static_assert(std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>,
			"T should be copy or move constructible");
		if (!value_factory)
		{
			throw std::invalid_argument("value_factory must have a target");
		}
		return value_factory;
	}())
	{}

	template <typename T>
	bool Lazy<T>::LazyInner::is_value_created() const
	{
		return _is_created;
	}

	template <typename T>
	const T& Lazy<T>::LazyInner::get_value()
	{
		std::call_once(_flag, [this]()
		{
			_value = _value_factory();
			_is_created = true;
		});
		return *_value;
	}

	template <typename T>
	Lazy<T>::Lazy(const std::function<T()>& value_factory)
		: _inner(std::make_shared<LazyInner>(value_factory))
	{}

	template <typename T>
	bool Lazy<T>::is_value_created() const
	{
		return _inner->is_value_created();
	}

	template <typename T>
	const T& Lazy<T>::get_value()
	{
		return _inner->get_value();
	}

	template <typename T>
	Lazy<T>::operator const T&()
	{
		return _inner->get_value();
	}
}
