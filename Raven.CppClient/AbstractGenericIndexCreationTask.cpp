#include "stdafx.h"
#include "AbstractGenericIndexCreationTask.h"
#include "SpatialOptionsFactory.h"

namespace ravendb::client::documents::indexes
{
	AbstractGenericIndexCreationTask::~AbstractGenericIndexCreationTask() = default;

	bool AbstractGenericIndexCreationTask::is_map_reduce() const
	{
		return reduce.has_value();
	}

	void AbstractGenericIndexCreationTask::index(std::string field, FieldIndexing indexing)
	{
		indexes_strings.insert_or_assign(std::move(field), indexing);
	}

	void AbstractGenericIndexCreationTask::spacial(std::string field,
		std::function<spatial::SpatialOptions(const spatial::SpatialOptionsFactory&)> indexing)
	{
		spacial_options_strings.insert_or_assign(std::move(field), indexing(spatial::SpatialOptionsFactory()));
	}

	void AbstractGenericIndexCreationTask::store_all_fields(FieldStorage storage)
	{
		stores_strings.insert_or_assign(constants::documents::indexing::fields::ALL_FIELDS, storage);
	}

	void AbstractGenericIndexCreationTask::store(std::string field, FieldStorage storage)
	{
		stores_strings.insert_or_assign(std::move(field), storage);
	}

	void AbstractGenericIndexCreationTask::analyze(std::string field, std::string analyzer)
	{
		analyzers_strings.insert_or_assign(std::move(field), std::move(analyzer));
	}

	void AbstractGenericIndexCreationTask::term_vector(std::string field, FieldTermVector term_vector)
	{
		term_vectors_strings.insert_or_assign(std::move(field), term_vector);
	}

	void AbstractGenericIndexCreationTask::suggestion(std::string field)
	{
		index_suggestions.insert(std::move(field));
	}
}
