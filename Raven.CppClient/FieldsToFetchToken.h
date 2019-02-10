#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class FieldsToFetchToken : public QueryToken
	{
	public:
		const std::vector<std::optional<std::string>> _fields_to_fetch;
		const std::optional<std::vector<std::string>> _projections;
		const bool _custom_function;
		const std::optional<std::string> _source_alias;


		~FieldsToFetchToken() override = default;
		
		FieldsToFetchToken(std::vector<std::optional<std::string>> fields_to_fetch, 
			std::optional<std::vector<std::string>> projections,
			bool custom_function, std::optional<std::string> source_alias)
			: _fields_to_fetch([&]()
		{
			if (fields_to_fetch.empty())
			{
				throw std::runtime_error("field_to_fetch can't be empty");
			}
			return std::move(fields_to_fetch);
		}())
			, _projections([&]()
		{
			if (!custom_function && projections && projections->size() != fields_to_fetch.size())
			{
				throw std::runtime_error("The length of projections must be the same as the length of fields_to_fetch");
			}

			return std::move(projections);
		}())
			, _custom_function(custom_function)
			, _source_alias(std::move(source_alias))
		{}

		void write_to(std::ostringstream& oss) override
		{
			for(auto i=0; i< _fields_to_fetch.size(); ++i)
			{
				auto&& field_to_fetch = _fields_to_fetch[i];
				if(i>0)
				{
					oss << ", ";
				}
				if(!field_to_fetch)
				{
					oss << "null";
				}else
				{
					write_field(oss, *field_to_fetch);
				}
				if (_custom_function)
				{
					continue;
				}
				const std::string* projection = _projections ? &(*_projections)[i] : nullptr;
				if(projection == nullptr || *projection == field_to_fetch)
				{
					continue;
				}

				oss << " as " << *projection;
			}
		}
	};
}
