#pragma once
#include "ServerNode.h"

namespace ravendb::client::http
{
	struct CurrentIndexAndNode
	{
		int32_t current_index;
		std::shared_ptr<const ServerNode> current_node;

		CurrentIndexAndNode(int32_t current_index_param, std::shared_ptr<const ServerNode> current_node_param)
			: current_index(current_index_param)
			, current_node(current_node_param)
		{}


		CurrentIndexAndNode(const CurrentIndexAndNode& other)
			: current_index(other.current_index)
		{
			current_node = std::make_shared<ServerNode>(*other.current_node);
		}

		CurrentIndexAndNode(CurrentIndexAndNode&& other) noexcept
			: current_index(other.current_index),
			  current_node(std::move(other.current_node))
		{
			other.current_index = -1;
			other.current_node.reset();
		}

		CurrentIndexAndNode& operator=(const CurrentIndexAndNode& other)
		{
			if (this == &other)
				return *this;
			current_index = other.current_index;
			current_node = std::make_shared<ServerNode>(*other.current_node);
			return *this;
		}

		CurrentIndexAndNode& operator=(CurrentIndexAndNode&& other) noexcept
		{
			if (this == &other)
				return *this;
			current_index = other.current_index;
			current_node = std::move(other.current_node);
			other.current_index = -1;
			other.current_node.reset();
			return *this;
		}
	};
}
