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
		Lazy<int32_t> count_lazily();

		std::shared_ptr<T> first(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> first_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> single(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		std::shared_ptr<T> single_or_default(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		bool any();

		int32_t count();

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval = {});
	};

	template <typename T, class TThis>
	Lazy<int32_t> IDocumentQueryBaseSingle<T, TThis>::count_lazily()
	{
		return cast_down()->count_lazily;
	}

	template <typename T, class TThis>
	std::shared_ptr<T> IDocumentQueryBaseSingle<T, TThis>::first(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return cast_down()->first(from_json);
	}

	template <typename T, class TThis>
	std::shared_ptr<T> IDocumentQueryBaseSingle<T, TThis>::first_or_default(
		const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return cast_down()->first_or_default();
	}

	template <typename T, class TThis>
	std::shared_ptr<T> IDocumentQueryBaseSingle<T, TThis>::single(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return cast_down()->single(from_json);
	}

	template <typename T, class TThis>
	std::shared_ptr<T> IDocumentQueryBaseSingle<T, TThis>::single_or_default(
		const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		return cast_down()->single_or_default(from_json);
	}

	template <typename T, class TThis>
	bool IDocumentQueryBaseSingle<T, TThis>::any()
	{
		return cast_down()->any();
	}

	template <typename T, class TThis>
	int32_t IDocumentQueryBaseSingle<T, TThis>::count()
	{
		return cast_down()->count();
	}

	template <typename T, class TThis>
	Lazy<std::vector<std::shared_ptr<T>>> IDocumentQueryBaseSingle<T, TThis>::lazily(
		const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval)
	{
		return cast_down()->lazily(on_eval);
	}
}
