#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	//TODO complete
	class FacetToken : public QueryToken
	{
	private:
		std::optional<std::string> _facet_setup_document_id{};
		std::optional<std::string> _aggregate_by_field_name{};
		std::optional<std::string> _alias{};
		std::optional<std::vector<std::string>> _ranges{};
		std::optional<std::string> _options_parameter_name{};

		class FacetAggregationToken;
		std::vector<std::shared_ptr<FacetAggregationToken>> _aggregations{};

		explicit FacetToken(std::optional<std::string> facet_setup_document_id);

		FacetToken(std::optional<std::string> aggregate_by_field_name, std::optional<std::string> alias,
			std::optional<std::vector<std::string>> ranges, std::optional<std::string> options_parameter_name);

	public:
		~FacetToken() override = default;

		const std::optional<std::string>& get_name() const;
	};
}
