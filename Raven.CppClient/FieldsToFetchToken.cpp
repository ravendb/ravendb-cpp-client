#include "stdafx.h"
#include "FieldsToFetchToken.h"

namespace ravendb::client::documents::session::tokens
{
	FieldsToFetchToken::~FieldsToFetchToken() = default;

	std::shared_ptr<FieldsToFetchToken> FieldsToFetchToken::create(
		std::vector<std::string> fields_to_fetch, std::optional<std::vector<std::string>> projections,
		bool custom_function, std::optional<std::string> source_alias)
	{
		return std::shared_ptr<FieldsToFetchToken>(new FieldsToFetchToken(std::move(fields_to_fetch),
			std::move(projections), custom_function, std::move(source_alias)));
	}

	FieldsToFetchToken::FieldsToFetchToken(std::vector<std::string> fields_to_fetch, std::optional<std::vector<std::string>> projections, bool custom_function, std::optional<std::string> source_alias)
		: _fields_to_fetch([fields_to_fetch = std::move(fields_to_fetch)]()
	{
		if (fields_to_fetch.empty())
		{
			throw std::runtime_error("field_to_fetch can't be empty");
		}
		return std::move(fields_to_fetch);
	}())
		, _projections([&]()
	{
		if (!custom_function && projections && projections->size() != _fields_to_fetch.size())
		{
			throw std::runtime_error("The length of projections must be the same as the length of fields_to_fetch");
		}

		return std::move(projections);
	}())
		, _custom_function(custom_function)
		, _source_alias(std::move(source_alias))
	{}

	void FieldsToFetchToken::write_to(std::ostringstream & writer) const
	{
		for (auto i = 0; i < _fields_to_fetch.size(); ++i)
		{
			auto&& field_to_fetch = _fields_to_fetch[i];
			if (i > 0)
			{
				writer << ", ";
			}
			//TODO check if relevant
			//if (!field_to_fetch)
			//{
			//	writer << "null";
			//}else
			{
				write_field(writer, field_to_fetch);
			}
			if (_custom_function)
			{
				continue;
			}

			const std::string* projection = _projections ? &(*_projections)[i] : nullptr;
			if (projection == nullptr || *projection == field_to_fetch)
			{
				continue;
			}

			writer << " as " << *projection;
		}
	}

}