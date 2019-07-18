#pragma once
#include "ServerNode.h"

namespace ravendb::client::http
{
	struct CurrentIndexAndNode
	{
		int32_t current_index{};
		std::shared_ptr<const ServerNode> current_node{};

		CurrentIndexAndNode(int32_t current_index_param, std::shared_ptr<const ServerNode> current_node_param);

		CurrentIndexAndNode(const CurrentIndexAndNode& other);

		CurrentIndexAndNode(CurrentIndexAndNode&& other) noexcept;

		CurrentIndexAndNode& operator=(const CurrentIndexAndNode& other);

		CurrentIndexAndNode& operator=(CurrentIndexAndNode&& other) noexcept;
	};
}
