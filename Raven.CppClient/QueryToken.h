#pragma once

namespace ravendb::client::documents::session::tokens
{
	class QueryToken
	{
	private:
		static const std::set<std::string> RQL_KEYWORDS;
		
	public:
		virtual ~QueryToken() = 0;

		virtual void write_to(std::ostringstream& oss) = 0;

	protected:
		static void write_field(std::ostringstream& oss, const std::string& field);
	};
}
