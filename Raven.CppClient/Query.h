#pragma once

namespace ravendb::client::documents::queries
{
	class Query
	{
	private:
		std::optional<std::string> _collection{};
		std::optional<std::string> _index_name{};

	public:
		const std::optional<std::string>& get_collection() const;
		const std::optional<std::string>& get_index_name() const;

		static Query index(std::string index_name);

		static Query collection(std::string collection_name);
	};
}
