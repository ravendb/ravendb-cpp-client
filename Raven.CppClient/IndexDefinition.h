#pragma once
#include <unordered_set>
#include "IndexPriority.h"
#include "IndexLockMode.h"
#include "IndexType.h"
#include "IndexConfiguration.h"
#include "IndexFieldOptions.h"
#include "json.hpp"

namespace ravendb::client::documents::indexes
{
	struct IndexDefinition
	{
	public:
		 std::string name{};
		 std::optional<IndexPriority> priority{};
		 std::optional<IndexLockMode> lock_mode{};
		 std::optional<std::unordered_map<std::string, std::string>> additional_sources{};
		 std::unordered_set<std::string> maps{};
		 std::optional<std::string> reduce{};
		 std::unordered_map<std::string, IndexFieldOptions> fields{};
		 IndexConfiguration configuration{};
		 std::optional<IndexType> index_type{};
		 std::optional<std::string> output_reduce_to_collection{};

	private:
		IndexType detect_static_index_type() const;

	public:
		operator std::string() const;

		IndexType get_type();
	};

	bool operator==(const IndexDefinition& lhs, const IndexDefinition& rhs);

	void to_json(nlohmann::json& j, const IndexDefinition& id);

	void from_json(const nlohmann::json& j, IndexDefinition& id);
}
