#include "stdafx.h"
#include "NodeSelector.h"
#include "utils.h"
#include <algorithm>
#include "AllTopologyNodesDownException.h"

namespace ravendb::client::http
{
	NodeSelector::NodeSelectorState::NodeSelectorState(std::shared_ptr<Topology> topology_param)
		: topology(std::const_pointer_cast<const Topology>(topology_param))
		, nodes(std::const_pointer_cast<const std::vector<std::shared_ptr<ServerNode>>>(topology->nodes))
		, failures(topology->nodes->size())
		, fastest_records(topology->nodes->size(), 0)
	{
		
		for (size_t i = 0; i < failures.size(); ++i)
		{
			failures[i].store(0);
		}
	}

	CurrentIndexAndNode NodeSelector::unlikely_everyone_faulted_choice(const NodeSelectorState& state)
	{
		// if all nodes are marked as failed, we'll chose the first
		// one so the user will get an error (or recover :-) );

		if(state.nodes->empty())
		{
			throw exceptions::AllTopologyNodesDownException("There are no nodes in the topology at all");
		}
		return CurrentIndexAndNode(0, state.nodes->at(0));
	}

	void NodeSelector::switch_to_speed_test_phase()
	{
		auto& state = *_state;

		if(state.speed_test_mode.exchange(1) != 0)
		{
			return;
		}

		for(auto& record : state.fastest_records)
		{
			record = 0;
		}

		++state.speed_test_mode;
	}

	int32_t NodeSelector::find_name_index(const NodeSelectorState& state)
	{
		auto& state_fastest = state.fastest_records;
		int32_t max_index{ 0 };
		int32_t max_value{ 0 };

		for(int32_t i = 0; i < state_fastest.size(); ++i)
		{
			if(max_value >= state_fastest[i])
			{
				continue;
			}
			max_index = i;
			max_value = state_fastest[i];
		}
		return max_index;
	}

	void NodeSelector::select_fastest(NodeSelectorState& state, int32_t index)
	{
		state.fastest = index;
		state.speed_test_mode.store(0);

		if(_update_fastest_node_timer)
		{
			_update_fastest_node_timer->change(std::chrono::minutes(1));
		}
		else
		{
			_update_fastest_node_timer = primitives::Timer::create(
				_scheduler,
				std::bind(&NodeSelector::switch_to_speed_test_phase, this),
				std::chrono::minutes(1));
		}
	}

	void NodeSelector::throw_empty_topology()
	{
		throw std::runtime_error("Empty database topology, this shouldn't happen.");
	}

	std::shared_ptr<const Topology> NodeSelector::get_topology() const
	{
		return _state->topology;
	}

	NodeSelector::NodeSelector(std::shared_ptr<Topology> topology, std::shared_ptr<impl::TasksScheduler> scheduler)
		: _scheduler(scheduler)
		, _state(topology)
	{}

	void NodeSelector::on_failed_request(int32_t node_index)
	{
		auto& state = *_state;
		if(node_index < 0 || node_index >= state.failures.size())
		{
			return;// probably already changed
		}

		++state.failures[node_index];
	}

	bool NodeSelector::on_update_topology(std::shared_ptr<Topology> topology, bool force_update)
	{
		if(!topology)
		{
			return false;
		}

		const auto state_etag = _state->topology->etag.value_or(0);
		const auto topology_etag = topology->etag.value_or(0);

		if(state_etag >= topology_etag && !force_update)
		{
			return false;
		}

		_state.emplace(topology);
		return true;
	}

	CurrentIndexAndNode NodeSelector::get_preferred_node() const
	{
		auto& state = *_state;
		auto& state_failures = state.failures;
		auto& server_nodes = state.nodes;

#ifdef min
#undef min
#endif
		int32_t len =static_cast<int32_t>(std::min(server_nodes->size(), state_failures.size()));

		for(int32_t i = 0; i < len; ++i)
		{
			if(state_failures[i].load() == 0 && !impl::utils::is_blank(server_nodes->at(i)->url))
			{
				return CurrentIndexAndNode(i, server_nodes->at(i));
			}
		}
		return unlikely_everyone_faulted_choice(state);
	}

	CurrentIndexAndNode NodeSelector::get_node_by_session_id(int32_t session_id) const
	{
		auto& state = *_state;
		int32_t index = session_id % state.topology->nodes->size();

		for(int32_t i = index; i < state.failures.size(); ++i)
		{
			if(state.failures[i].load() == 0 && state.nodes->at(i)->server_role == Role::MEMBER)
			{
				return CurrentIndexAndNode(i, state.nodes->at(i));
			}
		}

		for (int32_t i = 0; i < index; ++i)
		{
			if (state.failures[i].load() == 0 && state.nodes->at(i)->server_role == Role::MEMBER)
			{
				return CurrentIndexAndNode(i, state.nodes->at(i));
			}
		}

		return get_preferred_node();
	}

	CurrentIndexAndNode NodeSelector::get_fastest_node()
	{
		auto& state = *_state;

		if(state.failures[state.fastest].load() == 0 && state.nodes->at(state.fastest)->server_role == Role::MEMBER)
		{
			return CurrentIndexAndNode(state.fastest, state.nodes->at(state.fastest));
		}

		// if the fastest node has failures, we'll immediately schedule
		// another run of finding who the fastest node is, in the meantime
		// we'll just use the server preferred node or failover as usual

		switch_to_speed_test_phase();
		return get_preferred_node();
	}

	void NodeSelector::restore_node_index(int32_t node_index)
	{
		auto& state = *_state;
		if(state.failures.size() <= node_index)
		{
			return; // the state was changed and we no longer have it?
		}

		state.failures.at(node_index).store(0);
	}

	bool NodeSelector::in_speed_test_phase() const
	{
		return _state->speed_test_mode.load() > 1;
	}

	void NodeSelector::record_fastest(int32_t index, std::shared_ptr<const ServerNode> node)
	{
		auto& state = *_state;
		auto& state_fastest = state.fastest_records;

		// the following two checks are to verify that things didn't move
		// while we were computing the fastest node, we verify that the index
		// of the fastest node and the identity of the node didn't change during
		// our check

		if(index < 0 || index >= state_fastest.size())
		{
			return;
		}
		if(*node != *state.nodes->at(index))
		{
			return;
		}

		if(++state_fastest[index] >= 10)
		{
			select_fastest(state, index);
		}
		if(++state.speed_test_mode <= state.nodes->size() * 10)
		{
			return;
		}

		//too many concurrent speed tests are happening
		int32_t max_index = find_name_index(state);
		select_fastest(state, max_index);
	}

	void NodeSelector::schedule_speed_test()
	{
		switch_to_speed_test_phase();
	}
}
