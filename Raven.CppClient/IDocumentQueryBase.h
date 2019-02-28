#pragma once
#include "IQueryBase.h"
#include "OrderingType.h"

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IDocumentQueryBase : public IQueryBase<TThis>
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IDocumentQueryBase, TThis>, "'TThis' should be derived from IDocumentQueryBase");
			return static_cast<TThis*>(this);
		}

	protected:
		IDocumentQueryBase() = default;

	public:
		std::shared_ptr<TThis> add_order(const std::string& field_name, bool descending)
		{
			return cast_down()->add_order(field_name, descending);
		}

		std::shared_ptr<TThis> add_order(const std::string& field_name, bool descending, OrderingType ordering )
		{
			return cast_down()->add_order(field_name, descending, ordering);
		}

		std::shared_ptr<TThis> boost(double boost)
		{
			return cast_down()->boost(boost);
		}

		std::shared_ptr<TThis> distinct()
		{
			return cast_down()->distinct();
		}

		//TODO implement
		//std::shared_ptr<TThis> include_explanation(Explanations& explanations)
		//{
		//	return cast_down()->include_explanation(explanations);
		//}

		//std::shared_ptr<TThis> include_explanation(const ExplanationOptions& options, Explanations& explanations)
		//{
		//	return cast_down()->include_explanation(options, explanations);
		//}

		std::shared_ptr<TThis> fuzzy(double fuzzy)
		{
			return cast_down()->fuzzy(fuzzy);
		}

		std::shared_ptr<TThis> include(const std::string& path)
		{
			return cast_down()->include(path);
		}

		//TODO implement
		//std::shared_ptr<TThis> include(const std::function<void(IQueryIncludeBuilder&)>& includes)
		//{
		//	return cast_down()->include(includes);
		//}

		std::shared_ptr<TThis> intersect()
		{
			return cast_down()->intersect();
		}

		std::shared_ptr<TThis> order_by(const std::string& field)
		{
			return cast_down()->order_by(field);
		}

		std::shared_ptr<TThis> order_by(const std::string& field, OrderingType ordering)
		{
			return cast_down()->order_by(field, ordering);
		}


		std::shared_ptr<TThis> order_by_descending(const std::string& field)
		{
			return cast_down()->order_by_descending(field);
		}

		std::shared_ptr<TThis> order_by_descending(const std::string& field, OrderingType ordering)
		{
			return cast_down()->order_by_descending(field, ordering);
		}

		std::shared_ptr<TThis> order_by_score()
		{
			return cast_down()->order_by_score();
		}

		std::shared_ptr<TThis> order_by_score_descending()
		{
			return cast_down()->order_by_score_descending();
		}

		std::shared_ptr<TThis> proximity()
		{
			return cast_down()->proximity();
		}

		std::shared_ptr<TThis> random_ordering()
		{
			return cast_down()->random_ordering();
		}

		std::shared_ptr<TThis> random_ordering(const std::string& seed)
		{
			return cast_down()->random_ordering(seed);
		}

		//TODO implement all overloads
		//TSelf orderByDistance(DynamicSpatialField field, double latitude, double longitude);

	};
}
