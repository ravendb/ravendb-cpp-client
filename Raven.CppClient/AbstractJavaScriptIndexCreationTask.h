#pragma once
#include "AbstractIndexCreationTaskBase.h"

namespace ravendb::client::documents::indexes
{
	class AbstractJavaScriptIndexCreationTask : public AbstractIndexCreationTaskBase
	{
	private:
		IndexDefinition _definition{};

	protected:
		AbstractJavaScriptIndexCreationTask();

		const std::optional<std::string>& get_reduce() const;
		void set_reduce(std::optional<std::string> reduce);

		const std::optional<std::string>& get_output_reduce_to_collection() const;
		void set_output_reduce_to_collection(std::optional<std::string> output_reduce_to_collection);

	public:
		~AbstractJavaScriptIndexCreationTask() override;

		const std::unordered_set<std::string>& get_maps() const;
		void set_maps(std::unordered_set<std::string> maps);

		const std::unordered_map<std::string, IndexFieldOptions>& get_field() const;
		void set_fields(std::unordered_map<std::string, IndexFieldOptions> fields);

		const IndexConfiguration& get_configuration() const;
		void set_configuration(IndexConfiguration configuration);

		bool is_map_reduce() const override;

		IndexDefinition create_index_definition() override;
	};
}
