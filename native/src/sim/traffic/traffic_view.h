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
};

// A node as the traffic runs it; lamps and minimum greens are ranges of TrafficView::lamps / ::least.
struct JunctionView {
	bool known = false, signal = false;
	int control = 0, movements = 0, stage = 0, stages = 0;
	float cycle = 0;
	int lamp_from = 0, lamp_n = 0, least_from = 0; // lamp_n: one per stage's approaches; least: one per stage
};

struct TrafficView {
	int vehicles = 0, people = 0, arrived = 0, lost = 0; // the fleet (the snapshot)
	int near = 0, queued = 0; // driven by the near model, and of those standing
	double mean_speed_ms = 0;
	std::vector<TrafficTally> seg, node; // per segment and per node
	std::vector<uint32_t> volume; // per segment: vehicles that entered it since the city opened
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

namespace traffic {
void publish_view(CityState &s); // the only writer of CityState::traffic_views
} // namespace traffic

} // namespace city
