#pragma once

namespace ravendb::client::documents::session::loaders
{
	template<class TThis>
	class IQueryIncludeBuilder
	{
	private:
		constexpr TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IQueryIncludeBuilder, TThis>,
				"'TThis' should be derived from IQueryIncludeBuilder");
			return static_cast<TThis*>(this);
		}

	protected:
		IQueryIncludeBuilder() = default;

	public:
		auto include_documents(std::string path);

	};

	template <class TThis>
	auto IQueryIncludeBuilder<TThis>::include_documents(std::string path)
	{
		return cast_down()->include_documents(std::move(path));
	}
}
