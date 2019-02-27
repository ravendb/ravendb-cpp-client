#pragma once

namespace ravendb::client::documents
{
	template<typename T>
	class Lazy;
}

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IDocumentQueryBaseSingle
	{
	private:	
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IDocumentQueryBaseSingle, TThis>,
				"'TThis' should be derived from IDocumentQueryBaseSingle");
			return static_cast<TThis*>(this);
		}

	protected:
		IDocumentQueryBaseSingle() = default;

	public:

		//TODO implement
		//Lazy<Integer> countLazily();

		std::shared_ptr<T> first()
		{
			return cast_down()->first();
		}

		std::shared_ptr<T> first_or_default()
		{
			return cast_down()->first_or_default();
		}

		std::shared_ptr<T> single()
		{
			return cast_down()->single();
		}

		bool any() const 
		{
			return cast_down()->any();
		}

		int32_t count()
		{
			return cast_down()->count();
		}

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval = {})
		{
			return cast_down()->lazily(on_eval);
		}
	};
}
