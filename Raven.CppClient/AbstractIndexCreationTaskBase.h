#pragma once
#include "IndexDefinition.h"
#include "DocumentConventions.h"
#include "IDocumentStore.h"

namespace ravendb::client::documents::indexes
{
	class AbstractIndexCreationTaskBase
	{
	protected:
		std::shared_ptr<conventions::DocumentConventions> conventions{};
		std::unordered_map<std::string, std::string> additional_sources{};
		std::optional<IndexPriority> priority{};
		std::optional<IndexLockMode> lock_mode{};

		std::optional<std::type_index> my_real_type{};

		//set the real type of IndexCreationTask to generate the proper index name(from the class name)
		void set_my_type(std::type_index type);
		
	public:
		virtual ~AbstractIndexCreationTaskBase() = 0;

		virtual IndexDefinition create_index_definition() = 0;

		const std::unordered_map<std::string, std::string>& get_additional_sources() const;
		void set_additional_sources(std::unordered_map<std::string, std::string> additional_sources_param);

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
