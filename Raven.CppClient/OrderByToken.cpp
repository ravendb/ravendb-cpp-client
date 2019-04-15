#include "stdafx.h"
#include "OrderByToken.h"
#include <iterator>

namespace ravendb::client::documents::session::tokens
{
	OrderByToken::OrderByToken(std::string field_name, bool descending, OrderingType ordering)
		: _field_name(std::move(field_name))
		, _descending(descending)
		, _ordering(ordering)
	{}

	std::shared_ptr<OrderByToken> OrderByToken::create_distance_ascending(const std::string& field_name,
		const std::string& latitude_parameter_name, const std::string& longitude_parameter_name)
	{
		std::ostringstream field{};
		field << "spatial.distance(" << field_name <<
			", spatial.point($" << latitude_parameter_name <<
			", $" << longitude_parameter_name << "))";
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field).str(), false, OrderingType::STRING));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_distance_ascending(const std::string& field_name,
		const std::string& shape_wkt_parameter_name)
	{
		std::ostringstream field{};
		field << "spatial.distance(" << field_name <<
			", spatial.wkt($" << shape_wkt_parameter_name << "))";
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field).str(), false, OrderingType::STRING));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_distance_descending(const std::string& field_name,
		const std::string& latitude_parameter_name, const std::string& longitude_parameter_name)
	{
		std::ostringstream field{};
		field << "spatial.distance(" << field_name <<
			", spatial.point($" << latitude_parameter_name <<
			", $" << longitude_parameter_name << "))";
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field).str(), true, OrderingType::STRING));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_distance_descending(const std::string& field_name,
		const std::string& shape_wkt_parameter_name)
	{
		std::ostringstream field{};
		field << "spatial.distance(" << field_name <<
			", spatial.wkt($" << shape_wkt_parameter_name << "))";
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field).str(), true, OrderingType::STRING));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_random(const std::string& seed)
	{
		std::ostringstream field{};
		field << "random('";

		std::transform(seed.cbegin(), seed.cend(), std::ostream_iterator<const char*>(field),
			[](auto c)->const char*
		{
			char char_as_str[2] = { c, '\0' };
			return c == '\'' ? "''" : char_as_str;
		});

		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field).str(), false, OrderingType::STRING));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_ascending(std::string field_name, OrderingType ordering)
	{
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field_name), false, ordering));
	}

	std::shared_ptr<OrderByToken> OrderByToken::create_descending(std::string field_name, OrderingType ordering)
	{
		return std::shared_ptr<OrderByToken>(new OrderByToken(std::move(field_name), true, ordering));
	}

	void OrderByToken::write_to(std::ostringstream& writer) const
	{
		write_field(writer, _field_name);

		switch (_ordering)
		{
		case OrderingType::LONG:
			writer << " as long";
			break;
		case OrderingType::DOUBLE:
			writer << " as double";
			break;
		case OrderingType::ALPHA_NUMERIC:
			writer << " as alphaNumeric";
			break;
		default:
			break;
		}

		if(_descending) // we only add this if we have to, ASC is the default and reads nicer
		{
			writer << " desc";
		}
	}

	const std::shared_ptr<OrderByToken> OrderByToken::random = 
		std::shared_ptr<OrderByToken>(new OrderByToken("random()", false, OrderingType::STRING));

	const std::shared_ptr<OrderByToken> OrderByToken::score_ascending =
		std::shared_ptr<OrderByToken>(new OrderByToken("score()", false, OrderingType::STRING));

	const std::shared_ptr<OrderByToken> OrderByToken::score_descending =
		std::shared_ptr<OrderByToken>(new OrderByToken("score()", true, OrderingType::STRING));

}
