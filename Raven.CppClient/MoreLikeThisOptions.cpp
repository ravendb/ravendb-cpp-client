#include "stdafx.h"
#include "MoreLikeThisOptions.h"

namespace ravendb::client::documents::queries::more_like_this
{
	int32_t MoreLikeThisOptions::DEFAULT_MAXIMUM_NUMBER_OF_TOKENS_PARSED = 5000;
	int32_t MoreLikeThisOptions::DEFAULT_MINIMUM_TERM_FREQUENCY = 2;
	int32_t MoreLikeThisOptions::DEFAULT_MINIMUM_DOCUMENT_FREQUENCY = 5;
	int32_t MoreLikeThisOptions::DEFAULT_MAXIMUM_DOCUMENT_FREQUENCY = INT32_MAX;
	bool    MoreLikeThisOptions::DEFAULT_BOOST = false;
	double  MoreLikeThisOptions::DEFAULT_BOOST_FACTOR = 1;
	int32_t MoreLikeThisOptions::DEFAULT_MINIMUM_WORD_LENGTH = 0;
	int32_t MoreLikeThisOptions::DEFAULT_MAXIMUM_WORD_LENGTH = 0;
	int32_t MoreLikeThisOptions::DEFAULT_MAXIMUM_QUERY_TERMS = 25;

	void to_json(nlohmann::json& j, const MoreLikeThisOptions& mlto)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "MinimumTermFrequency", mlto.minimum_term_frequency);
		set_val_to_json(j, "MaximumQueryTerms", mlto.maximum_query_terms);
		set_val_to_json(j, "MaximumNumberOfTokensParsed", mlto.maximum_number_of_tokens_parsed);
		set_val_to_json(j, "MinimumWordLength", mlto.minimum_word_length);
		set_val_to_json(j, "MaximumWordLength", mlto.maximum_word_length);
		set_val_to_json(j, "MinimumDocumentFrequency", mlto.minimum_document_frequency);
		set_val_to_json(j, "MaximumDocumentFrequency", mlto.maximum_document_frequency);
		set_val_to_json(j, "MaximumDocumentFrequencyPercentage", mlto.maximum_document_frequency_percentage);
		set_val_to_json(j, "Boost", mlto.boost);
		set_val_to_json(j, "BoostFactor", mlto.boost_factor);
		set_val_to_json(j, "StopWordsDocumentId", mlto.stop_words_document_id);
		set_val_to_json(j, "Fields", mlto.fields);
	}
}
