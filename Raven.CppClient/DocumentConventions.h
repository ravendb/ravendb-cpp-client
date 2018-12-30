#pragma once

namespace ravendb::client::documents::conventions
{
	struct DocumentConventions
	{
		static const DocumentConventions& default_conventions()
		{
			static DocumentConventions dc{};
			return dc;
		}
		
		DocumentConventions() = default;

		//TODO do it !


	};
}
