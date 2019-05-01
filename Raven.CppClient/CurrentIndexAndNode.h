#pragma once
#include "ServerNode.h"

namespace ravendb::client::http
{
	struct CurrentIndexAndNode
	{
		const int32_t current_index;
		const std::shared_ptr<const ServerNode> current_node;

		CurrentIndexAndNode(int32_t current_index_param, std::shared_ptr<ServerNode> current_node_param)
			: current_index(current_index_param)
			, current_node(std::const_pointer_cast<const ServerNode>(current_node_param))
		{}
	};
}
