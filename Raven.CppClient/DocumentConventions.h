#pragma once

namespace ravendb::client::documents::conventions
{
	class DocumentConventions
	{
	public:
		static const DocumentConventions& default_conventions()
		{
			static DocumentConventions dc{};
			return dc;
		}
		
		DocumentConventions() = default;
		~DocumentConventions() = default;

		//TODO do it !


	};
}
