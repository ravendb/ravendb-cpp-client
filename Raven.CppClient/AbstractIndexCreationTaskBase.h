#pragma once
#include "IndexDefinition.h"
#include "DocumentConventions.h"
#include "IDocumentStore.h"
#include "GetCppClassName.h"


// Set the index name from the IndexCreationTask name
// Call from the ctor;
#define SET_DEFAULT_INDEX_NAME\
	do\
	{\
		set_default_index_name(typeid(decltype(*this)));\
	}while(false)

namespace ravendb::client::documents::indexes
{
	class AbstractIndexCreationTaskBase
	{
	private:
		std::optional<std::string> _index_name{};

		void throw_index_name_was_set() const;
		void throw_index_name_was_not_set() const;

	protected:
		std::shared_ptr<conventions::DocumentConventions> conventions{};
		std::optional<std::unordered_map<std::string, std::string>> additional_sources{};
		std::optional<IndexPriority> priority{};
		std::optional<IndexLockMode> lock_mode{};

		void set_index_name(std::string index_name);
		void set_default_index_name(std::type_index type);

	public:
		virtual ~AbstractIndexCreationTaskBase() = 0;

		virtual IndexDefinition create_index_definition() = 0;

		const std::optional<std::unordered_map<std::string, std::string>>& get_additional_sources() const;
		void set_additional_sources(std::optional<std::unordered_map<std::string, std::string>> additional_sources_param);

		virtual bool is_map_reduce() const;

		virtual std::string get_index_name() const;

		std::shared_ptr<conventions::DocumentConventions> get_conventions() const;
		void set_conventions(std::shared_ptr<conventions::DocumentConventions> conventions_param);

		std::optional<IndexPriority> get_priority() const;
		void set_priority(std::optional<IndexPriority> priority_param);

		std::optional<IndexLockMode> get_lock_mode() const;
		void set_lock_mode(std::optional<IndexLockMode> lock_mode_param);

		virtual void execute(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions_param = {},
			std::optional<std::string> database = {});
	};
}
