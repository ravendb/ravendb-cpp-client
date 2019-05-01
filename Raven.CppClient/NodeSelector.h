#pragma once
#include "TasksScheduler.h"
#include "Timer.h"
#include "Topology.h"
#include "CurrentIndexAndNode.h"

namespace ravendb::client::http
{
	class NodeSelector
	{
	private:
		class NodeSelectorState
		{
		public:
			const std::shared_ptr<const Topology> topology;
			const std::shared_ptr<const std::vector<std::shared_ptr<ServerNode>>> nodes;
			std::vector<std::atomic_int32_t> failures;
			std::vector<int32_t> fastest_records;
			int32_t fastest{};
			std::atomic_int32_t speed_test_mode{ 0 };

			NodeSelectorState(std::shared_ptr<Topology> topology_param);
		};

	private:
		const std::shared_ptr<impl::TasksScheduler> _scheduler;
		std::shared_ptr<primitives::Timer> _update_fastest_node_timer;
		std::optional<NodeSelectorState> _state{};

	private:
		static CurrentIndexAndNode unlikely_everyone_faulted_choice(const NodeSelectorState& state);

		void switch_to_speed_test_phase();

		static int32_t find_name_index(const NodeSelectorState& state);

		void select_fastest(NodeSelectorState& state, int32_t index);

	protected:
		static void throw_empty_topology();

	public:
		std::shared_ptr<const Topology> get_topology() const;

		NodeSelector(std::shared_ptr<Topology> topology, std::shared_ptr<impl::TasksScheduler> scheduler);

		void on_failed_request(int32_t node_index);

		bool on_update_topology(std::shared_ptr<Topology> topology, bool force_update = false);

		CurrentIndexAndNode get_preferred_node() const;

		CurrentIndexAndNode get_node_by_session_id(int32_t session_id) const;

		CurrentIndexAndNode get_fastest_node();

		void restore_node_index(int32_t node_index);

		bool in_speed_test_phase() const;

		void record_fastest(int32_t index, std::shared_ptr<const ServerNode> node);

		void schedule_speed_test();
	};
}
