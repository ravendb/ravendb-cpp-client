#include "stdafx.h"
#include "AbstractJavaScriptIndexCreationTask.h"

namespace ravendb::client::documents::indexes
{
	AbstractJavaScriptIndexCreationTask::AbstractJavaScriptIndexCreationTask()
	{
		_definition.lock_mode = IndexLockMode::UNLOCK;
		_definition.priority = IndexPriority::NORMAL;
	}

	const std::optional<std::string>& AbstractJavaScriptIndexCreationTask::get_reduce() const
	{
		return _definition.reduce;
	}

	void AbstractJavaScriptIndexCreationTask::set_reduce(std::optional<std::string> reduce)
	{
		_definition.reduce = std::move(reduce);
	}

	const std::optional<std::string>& AbstractJavaScriptIndexCreationTask::get_output_reduce_to_collection() const
	{
		return _definition.output_reduce_to_collection;
	}

	void AbstractJavaScriptIndexCreationTask::set_output_reduce_to_collection(
		std::optional<std::string> output_reduce_to_collection)
	{
		_definition.output_reduce_to_collection = std::move(output_reduce_to_collection);
	}

	const std::unordered_set<std::string>& AbstractJavaScriptIndexCreationTask::get_maps() const
	{
		return _definition.maps;
	}

	void AbstractJavaScriptIndexCreationTask::set_maps(std::unordered_set<std::string> maps)
	{
		_definition.maps = std::move(maps);
	}

	const std::unordered_map<std::string, IndexFieldOptions>& AbstractJavaScriptIndexCreationTask::get_field() const
	{
		return _definition.fields;
	}

	void AbstractJavaScriptIndexCreationTask::set_fields(std::unordered_map<std::string, IndexFieldOptions> fields)
	{
		_definition.fields = std::move(fields);
	}

	const IndexConfiguration& AbstractJavaScriptIndexCreationTask::get_configuration() const
	{
		return _definition.configuration;
	}

	void AbstractJavaScriptIndexCreationTask::set_configuration(IndexConfiguration configuration)
	{
		_definition.configuration = std::move(configuration);
	}

	bool AbstractJavaScriptIndexCreationTask::is_map_reduce() const
	{
		return get_reduce().has_value();
	}

	IndexDefinition AbstractJavaScriptIndexCreationTask::create_index_definition()
	{
		_definition.index_type = is_map_reduce() ? IndexType::JAVASCRIPT_MAP_REDUCE :
			IndexType::JAVASCRIPT_MAP;
		if (get_additional_sources())
		{
			_definition.additional_sources = *get_additional_sources();
		}else
		{
			_definition.additional_sources.emplace();
		}

		return _definition;
	}

	AbstractJavaScriptIndexCreationTask::~AbstractJavaScriptIndexCreationTask() = default;


}
