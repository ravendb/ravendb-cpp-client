#include "stdafx.h"
#include "AbstractIndexCreationTaskBase.h"
#include "GetCppClassName.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"

namespace ravendb::client::documents::indexes
{
	void AbstractIndexCreationTaskBase::throw_index_name_was_set()
	{
		throw std::runtime_error("The index name was already set");
	}

	void AbstractIndexCreationTaskBase::throw_index_name_was_not_set()
	{
		throw std::runtime_error("Index name was not set."
			"Did you forget to call set_index_name() or set_default_index_name() ?");
	}

	void AbstractIndexCreationTaskBase::set_index_name(std::string index_name)
	{
		if(_index_name)
		{
			throw_index_name_was_set();
		}
		_index_name = std::move(index_name);
	}

	void AbstractIndexCreationTaskBase::set_default_index_name(std::type_index type)
	{
		if (_index_name)
		{
			throw_index_name_was_set();
		}

		auto&& index_name = impl::utils::GetCppClassName::get_simple_class_name(type);
		for(auto& c : index_name)
		{
			if(c == '_')
				c = '/';
		}
		_index_name = std::move(index_name);
	}

	AbstractIndexCreationTaskBase::~AbstractIndexCreationTaskBase() = default;

	const std::optional<std::unordered_map<std::string, std::string>>& 
		AbstractIndexCreationTaskBase::get_additional_sources() const
	{
		return additional_sources;
	}

	void AbstractIndexCreationTaskBase::set_additional_sources(
		std::optional<std::unordered_map<std::string, std::string>> additional_sources_param)
	{
		additional_sources = std::move(additional_sources_param);
	}

	bool AbstractIndexCreationTaskBase::is_map_reduce() const
	{
		return false;
	}

	std::string AbstractIndexCreationTaskBase::get_index_name() const
	{
		if(!_index_name)
		{
			throw_index_name_was_not_set();
		}
		return *_index_name;
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
		priority = priority_param;
	}

	std::optional<IndexLockMode> AbstractIndexCreationTaskBase::get_lock_mode() const
	{
		return lock_mode;
	}

	void AbstractIndexCreationTaskBase::set_lock_mode(std::optional<IndexLockMode> lock_mode_param)
	{
		lock_mode = lock_mode_param;
	}

	void AbstractIndexCreationTaskBase::execute(std::shared_ptr<IDocumentStore> store,
		std::shared_ptr<conventions::DocumentConventions> conventions_param,
		std::optional<std::string> database)
	{
		struct OldConventionsRestore
		{
			AbstractIndexCreationTaskBase* outer_this;
			std::shared_ptr<conventions::DocumentConventions> old_conventions;

			~OldConventionsRestore(){ outer_this->set_conventions(old_conventions); }
		} old_conventions_restore{this, get_conventions()};

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

		store->maintenance()
			->for_database(database ? *database : store->get_database())
			->send(operations::indexes::PutIndexesOperation({ index_definition }));
	}
}
