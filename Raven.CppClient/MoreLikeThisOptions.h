#pragma once
#include "json.hpp"

namespace ravendb::client::documents::queries::more_like_this
{
	class MoreLikeThisOptions
	{
	public:
		static int32_t DEFAULT_MAXIMUM_NUMBER_OF_TOKENS_PARSED;
		static int32_t DEFAULT_MINIMUM_TERM_FREQUENCY;
		static int32_t DEFAULT_MINIMUM_DOCUMENT_FREQUENCY;
		static int32_t DEFAULT_MAXIMUM_DOCUMENT_FREQUENCY;
		static bool    DEFAULT_BOOST;
		static double  DEFAULT_BOOST_FACTOR;
		static int32_t DEFAULT_MINIMUM_WORD_LENGTH;
		static int32_t DEFAULT_MAXIMUM_WORD_LENGTH;
		static int32_t DEFAULT_MAXIMUM_QUERY_TERMS;

		MoreLikeThisOptions() = default;

		std::optional<int32_t> minimum_term_frequency{};
		std::optional<int32_t> maximum_query_terms{};
		std::optional<int32_t> maximum_number_of_tokens_parsed{};
		std::optional<int32_t> minimum_word_length{};
		std::optional<int32_t> maximum_word_length{};
		std::optional<int32_t> minimum_document_frequency{};
		std::optional<int32_t> maximum_document_frequency{};
		std::optional<int32_t> maximum_document_frequency_percentage{};
		std::optional<bool> boost{};
		std::optional<double> boost_factor{};
		std::string stop_words_document_id{};
		std::vector<std::string> fields{};
	};

	void to_json(nlohmann::json& j, const MoreLikeThisOptions& mlto);

}
