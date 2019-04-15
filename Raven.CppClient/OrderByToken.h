#pragma once
#include "QueryToken.h"
#include "OrderingType.h"

namespace ravendb::client::documents::session::tokens
{
	class OrderByToken : public QueryToken
	{
	private:
		const std::string _field_name;
		const bool _descending;
		const OrderingType _ordering;

		OrderByToken(std::string field_name, bool descending, OrderingType ordering);

	public:
		~OrderByToken() override = default;

		static const std::shared_ptr<OrderByToken> random;
		static const std::shared_ptr<OrderByToken> score_ascending;
		static const std::shared_ptr<OrderByToken> score_descending;

		static std::shared_ptr<OrderByToken> create_distance_ascending(const std::string& field_name,
			const std::string& latitude_parameter_name,
			const std::string& longitude_parameter_name);

		static std::shared_ptr<OrderByToken> create_distance_ascending(const std::string& field_name,
			const std::string& shape_wkt_parameter_name);

		static std::shared_ptr<OrderByToken> create_distance_descending(const std::string& field_name,
			const std::string& latitude_parameter_name,
			const std::string& longitude_parameter_name);

		static std::shared_ptr<OrderByToken> create_distance_descending(const std::string& field_name,
			const std::string& shape_wkt_parameter_name);

		static std::shared_ptr<OrderByToken> create_random(const std::string& seed);

		static std::shared_ptr<OrderByToken> create_ascending(std::string field_name, OrderingType ordering);

		static std::shared_ptr<OrderByToken> create_descending(std::string field_name, OrderingType ordering);

		void write_to(std::ostringstream& writer) const override;
	};
};
