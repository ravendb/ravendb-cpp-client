#include "stdafx.h"
#include "QueryFieldUtil.h"
#include "Constants.h"
#include <cctype>

namespace ravendb::client::documents::queries
{
	std::string QueryFieldUtil::escape_if_necessary(std::string name)
	{
		if(name.empty() ||
			name == constants::documents::indexing::fields::DOCUMENT_ID_FIELD_NAME ||
			name == constants::documents::indexing::fields::REDUCE_KEY_HASH_FIELD_NAME ||
			name == constants::documents::indexing::fields::REDUCE_KEY_KEY_VALUE_FIELD_NAME ||
			name == constants::documents::indexing::fields::SPATIAL_SHAPE_FIELD_NAME)
		{
			return name;
		}

		bool escape = false;
		bool inside_escaped = false;

		char c;
		for(auto i = 0; i < name.length(); ++i)
		{
			c = name[i];

			if(c == '\'' || c == '"')
			{
				inside_escaped = !inside_escaped;
				continue;
			}

			if(i == 0)
			{
				if(!std::isalpha(static_cast<unsigned char>(c)) && c != '_' && c != '@' && !inside_escaped)
				{
					escape = true;
					break;
				}
			}
			else
			{
				if(!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-' &&
					c != '@' && c != '.' && c != '[' && c != ']' && !inside_escaped)
				{
					escape = true;
					break;
				}
			}
		}

		if(escape ||inside_escaped)
		{
			std::ostringstream res{};
			res << "'" << name << "'";
			return res.str();
		}

		return name;
	}
}
