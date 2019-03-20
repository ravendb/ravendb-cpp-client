#pragma once
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::documents::indexes
{
	class AbstractMultiMapIndexCreationTask : public AbstractIndexCreationTask
	{
	private:
		std::vector<std::string> _maps{};

	protected:
		void add_map(std::string map);

	public:
		~AbstractMultiMapIndexCreationTask() override;

		IndexDefinition create_index_definition() override;
	};
}
