#pragma once


//TODO dispose of it in favor of std::function<T(void)>
namespace ravendb::client::impl
{
	template<typename T>
	class Supplier
	{
		virtual ~Supplier() = 0;

		virtual T get() const = 0;
	};

	template <typename T>
	Supplier<T>::~Supplier() = default;

}
