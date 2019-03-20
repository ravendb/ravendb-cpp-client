#include "stdafx.h"
#include "AbstractIndexCreationTaskBase.h"
#include "GetCppClassName.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"

namespace ravendb::client::documents::indexes
{
	void AbstractIndexCreationTaskBase::set_my_type(std::type_index type)
	{
		my_real_type = type;
	}

	AbstractIndexCreationTaskBase::~AbstractIndexCreationTaskBase() = default;

	const std::unordered_map<std::string, std::string>& 
		AbstractIndexCreationTaskBase::get_additional_sources() const
	{
		return additional_sources;
	}

	void AbstractIndexCreationTaskBase::set_additional_sources(
		std::unordered_map<std::string, std::string> additional_sources_param)
	{
		additional_sources = std::move(additional_sources_param);
	}

	bool AbstractIndexCreationTaskBase::is_map_reduce() const
	{
		return false;
	}

	std::string AbstractIndexCreationTaskBase::get_index_name() const
	{
		auto type = my_real_type ? *my_real_type : typeid(decltype(*this));

		auto&& class_name = impl::utils::GetCppClassName::get_simple_class_name(type);

		for(auto& c : class_name)
		{
			if (c == '_')
				c = '/';
		}

		return std::move(class_name);
	}

	std::shared_ptr<conventions::DocumentConventions> AbstractIndexCreationTaskBase::get_conventions() const
	{
		return conventions;
	}

	void AbstractIndexCreationTaskBase::set_conventions(
		std::shared_ptr<conventions::DocumentConventions> conventions_param)
	{
		conventions = conventions_param;
	}

	std::optional<IndexPriority> AbstractIndexCreationTaskBase::get_priority() const
	{
		return priority;
	}

	void AbstractIndexCreationTaskBase::set_priority(std::optional<IndexPriority> priority_param)
	{
		priority = std::move(priority_param);
	}

	std::optional<IndexLockMode> AbstractIndexCreationTaskBase::get_lock_mode() const
	{
		return lock_mode;
	}

	void AbstractIndexCreationTaskBase::set_lock_mode(std::optional<IndexLockMode> lock_mode_param)
	{
		lock_mode = std::move(lock_mode_param);
	}

	void AbstractIndexCreationTaskBase::execute(std::shared_ptr<IDocumentStore> store,
		std::shared_ptr<conventions::DocumentConventions> conventions_param,
		std::optional<std::string> database)
	{
		auto old_conventions = get_conventions();

		try
		{
			if (conventions_param)
			{
				set_conventions(conventions_param);
			}
			else if(get_conventions())
			{}
			else
			{
				set_conventions(store->get_conventions());
			}

			auto index_definition = create_index_definition();
			index_definition.name = get_index_name();

			if(lock_mode)
			{
				index_definition.lock_mode = *lock_mode;
			}
			if(priority)
			{
				index_definition.priority = *priority;
			}

			store->get_maintenance()
				->for_database(database ? *database : store->get_database())
				->send(operations::indexes::PutIndexesOperation({ index_definition }));
		}
		catch (...)
		{
			set_conventions(old_conventions);
			throw;
		}

		set_conventions(old_conventions);
	}
}
