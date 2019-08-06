#include "stdafx.h"
#include "WhereToken.h"
#include "Constants.h"
#include <cstdlib>

namespace ravendb::client::documents::session::tokens
{
	WhereToken::WhereOptions::WhereOptions() = default;

	WhereToken::WhereOptions WhereToken::WhereOptions::default_options()
	{
		return {};
	}

	WhereToken::WhereOptions::WhereOptions(bool exact_param)
		: exact(exact_param)
	{}

	WhereToken::WhereOptions::WhereOptions(bool exact_param, std::string from, std::string to)
		: exact(exact_param)
		, from_parameter_name(std::move(from))
		, to_parameter_name(std::move(to))
	{}

	WhereToken::WhereOptions::WhereOptions(queries::SearchOperator search)
		: search_operator(search)
	{}

	WhereToken::WhereOptions::WhereOptions(std::shared_ptr<ShapeToken> shape, double distance)
		: where_shape(shape)
		, distance_error_pct(distance)
	{}

	WhereToken::WhereOptions::WhereOptions(MethodsType method_type, std::vector<std::string> parameters,
		std::string property, bool exact_param)
		: exact(exact_param)
		, method({method_type, std::move(parameters), std::move(property)})
	{}

	bool WhereToken::write_method(std::ostringstream& writer) const
	{
		if(options.method)
		{
			switch (options.method->method_type)
			{
			case MethodsType::CMP_X_CHG:
				writer << "cmpxchg(";
				break;
			default:
				throw std::invalid_argument("Unsupported method");
			}

			bool first = true;
			for(const auto& parameter : options.method->parameters)
			{
				if(!first)
				{
					writer << ",";
				}
				first = false;
				writer << "$" << parameter;
			}
			writer << ")";
			if(options.method->property)
			{
				writer << "." << *options.method->property;
			}
			return true;
		}
		return false;
	}

	void WhereToken::write_inner_where(std::ostringstream& writer) const
	{
		write_field(writer, field_name);

		switch (where_operator)
		{
		case WhereOperator::EQUALS:
			writer << " = ";
			break;
		case WhereOperator::NOT_EQUALS:
			writer << " != ";
			break;
		case WhereOperator::GREATER_THAN:
			writer << " > ";
			break;
		case WhereOperator::GREATER_THAN_OR_EQUAL:
			writer << " >= ";
			break;
		case WhereOperator::LESS_THAN:
			writer << " < ";
			break;
		case WhereOperator::LESS_THAN_OR_EQUAL:
			writer << " <= ";
			break;
		default:
			special_operator(writer);
			return;
		}

		if(!write_method(writer))
		{
			writer << "$" << parameter_name;
		}
	}

	void WhereToken::special_operator(std::ostringstream& writer) const
	{
		switch (where_operator)
		{
		case WhereOperator::_IN:
			writer << " in ($" << parameter_name << ")";
			break;
		case WhereOperator::ALL_IN:
			writer << " all in ($" << parameter_name << ")";
			break;
		case WhereOperator::BETWEEN:
			writer << " between $" << options.from_parameter_name << " and $" << options.to_parameter_name;
			break;
		case WhereOperator::SEARCH:
			writer << ", $" << parameter_name;
			if(options.search_operator == queries::SearchOperator::AND)
			{
				writer << ", and";
			}
			writer << ")";
			break;
		case WhereOperator::LUCENE:
		case WhereOperator::STARTS_WITH:
		case WhereOperator::ENDS_WITH:
		case WhereOperator::REGEX:
			writer << ", $" << parameter_name << ")";
			break;
		case WhereOperator::EXISTS:
			writer << ")";
			break;
		case WhereOperator::SPATIAL_WITHIN:
		case WhereOperator::SPATIAL_CONTAINS:
		case WhereOperator::SPATIAL_DISJOINT:
		case WhereOperator::SPATIAL_INTERSECTS:
			writer << ", ";
			options.where_shape->write_to(writer);

			if(std::abs(options.distance_error_pct - constants::documents::indexing::spatial::DEFAULT_DISTANCE_ERROR_PCT) >
				1e-40)
			{
				writer << ", " << options.distance_error_pct;
			}
			writer << ")";
			break;
		default:
			throw std::invalid_argument("'where_operator' is of unknown type");
		}
	}

	WhereToken::WhereToken() = default;

	std::shared_ptr<WhereToken> WhereToken::create(WhereOperator op, std::string field_name,
		std::string parameter_name,	std::optional<WhereOptions> options)
	{
		auto token = std::shared_ptr<WhereToken>(new WhereToken());

		token->_weak_this = token;
		token->field_name = std::move(field_name);
		token->parameter_name = std::move(parameter_name);
		token->where_operator = op;
		if(options)
		{
			token->options = *options;
		}

		return token;
	}

	std::shared_ptr<WhereToken> WhereToken::add_alias(const std::string& alias)
	{
		if(field_name == "id()")
		{
			return _weak_this.lock();
		}

		auto token = std::shared_ptr<WhereToken>(new WhereToken());

		token->_weak_this = token;
		std::ostringstream field{};
		field << alias << "." << field_name;
		token->field_name = field.str();
		token->parameter_name = parameter_name;
		token->where_operator = where_operator;
		token->options = options;

		return token;
	}

	void WhereToken::write_to(std::ostringstream& writer) const
	{
		if(options.boost)
		{
			writer << "boost(";
		}
		if(options.fuzzy)
		{
			writer << "fuzzy(";
		}
		if(options.proximity)
		{
			writer << "proximity(";
		}
		if(options.exact)
		{
			writer << "exact(";
		}

		switch (where_operator)
		{
		case WhereOperator::SEARCH:
			writer << "search(";
			break;
		case WhereOperator::LUCENE:
			writer << "lucene(";
			break;
		case WhereOperator::STARTS_WITH:
			writer << "startsWith(";
			break;
		case WhereOperator::ENDS_WITH:
			writer << "endsWith(";
			break;
		case WhereOperator::EXISTS:
			writer << "exists(";
			break;
		case WhereOperator::SPATIAL_WITHIN:
			writer << "spatial.within(";
			break;
		case WhereOperator::SPATIAL_CONTAINS:
			writer << "spatial.contains(";
			break;
		case WhereOperator::SPATIAL_DISJOINT:
			writer << "spatial.disjoint(";
			break;
		case WhereOperator::SPATIAL_INTERSECTS:
			writer << "spatial.intersects(";
			break;
		case WhereOperator::REGEX:
			writer << "regex(";
			break;
		default:
			break;
		}

		write_inner_where(writer);

		if(options.exact)
		{
			writer << ")";
		}
		if(options.proximity)
		{
			writer << ", " << *options.proximity << ")";
		}
		if(options.fuzzy)
		{
			writer << ", " << *options.fuzzy << ")";
		}
		if(options.boost)
		{
			writer << ", " << *options.boost << ")";
		}
	}
}
