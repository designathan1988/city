// What the traffic publishes for the main thread after each tick and on a reset (publish_view, traffic_view.cpp): the fleet's
// counts and metrics, each segment's and node's tally, and each junction as the inspector shows it. A lock-free triple
// buffer, the PoseBuffer pattern: the traffic fills the back view in place (its vectors keep their room) and swaps it, the
// main thread takes the newest without waiting. So the snapshot, the HUD's metrics and the inspector never wait for a
// traffic step under traffic_mutex. Only the main thread acquires (one consumer).
#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace city {

struct CityState;

// Vehicles on a segment (both ways) or on the approaches that end at a node: far ones have no speed of their own.
struct TrafficTally {
	int vehicles = 0, queued = 0, seen = 0;
	float speed = 0, capacity = 0;
	float mean_speed() const { return seen > 0 ? speed / float(seen) : -1.f; }
	void clear_fleet() { vehicles = queued = seen = 0; speed = 0; } // capacity is the graph's
};

// A node as the traffic runs it; lamps and minimum greens are ranges of TrafficView::lamps / ::least.
struct JunctionView {
	bool known = false, signal = false;
	int control = 0, movements = 0, stage = 0, stages = 0;
	float cycle = 0;
	int lamp_from = 0, lamp_n = 0, least_from = 0; // lamp_n: one per stage's approaches; least: one per stage
};

struct TrafficView {
	// What only the lane graph decides (capacities, known, control, movements, the signalled nodes) is written when this
	// differs from CityState::lane_graph_serial: each of the three views holds its own copy.
	uint64_t graph_serial = 0;
	std::vector<int> signals; // the signalled nodes
	int vehicles = 0, people = 0, arrived = 0, lost = 0; // the fleet (the snapshot)
	int near = 0, queued = 0; // driven by the near model, and of those standing
	double mean_speed_ms = 0;
	double near_speed = 0; // the near model's speeds summed while the view is filled
	std::vector<TrafficTally> seg, node; // per segment and per node
	// The segments and nodes whose tallies hold vehicles in this view: the only ones its next filling clears (every other tally
	// holds none), so a filling costs the fleet, not the network.
	std::vector<int> seg_held, node_held;
	std::vector<uint32_t> volume; // per segment: vehicles that entered it since the city opened
	uint64_t volume_upto = 0; // the entries of TrafficState::volume_log it has taken (counted from the first ever)
	std::vector<JunctionView> junction; // per node
	std::vector<int> lamps; // each signalled stage's light, by its first approach
	std::vector<float> least; // each signalled stage's minimum green
};

template <typename T>
class TripleBuffer {
public:
	T &back() { return frames_[back_]; }
	void publish() { back_ = state_.exchange(back_ | NEW) & IDX; }
	const T &acquire() {
		if (state_.load() & NEW) front_ = state_.exchange(front_) & IDX;
		return frames_[front_];
	}

private:
	static constexpr int NEW = 4, IDX = 3;
	T frames_[3];
	std::atomic<int> state_{ 1 }; // middle = 1
	int back_ = 0, front_ = 2;
};

// The panels' choices for the simulation (Roadcraft SimWorld trafficIntensity, pedestrianIntensity, demandMultiplier):
// shares of the city's own density, 1 = as the catalog says, and the Effects panel's switches. The main thread writes
// them, the traffic reads them: atomics, so neither waits for the other.
struct TrafficLevels {
	std::atomic<float> traffic{ 1 }, people{ 1 }, demand{ 1 }, population{ 1 }; // population: the Optimisations tab's cap on both
	std::atomic<bool> vehicles_on{ true }, people_on{ true }, growth_on{ true }, crowd_on{ true };
};

struct Vehicle;

namespace traffic {
// The only writer of CityState::traffic_views. A view is filled in one pass over the fleet: begun (the back view, the tallies it
// held cleared), each live vehicle counted in id order, then ended (the fleet's totals, the volumes, the lights) and published.
// publish_poses counts the vehicles in its own pass over them; publish_view does the three for a fleet nobody else walks.
TrafficView &view_begin(CityState &s);
void view_vehicle(const CityState &s, TrafficView &view, const Vehicle &v);
void view_end(CityState &s, TrafficView &view);
void publish_view(CityState &s);
} // namespace traffic

} // namespace city
