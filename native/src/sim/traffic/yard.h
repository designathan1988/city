// A works' loading yard as a lorry drives it: what is read from the world under the locks (YardInput), and the ways worked out
// from that copy alone on a worker (plan_yard): for each dock, from a stop on the lane before a gate, in reverse onto the dock, and
// from the dock out to the lane past the gate.
#pragma once

#include "sim/traffic/way_search.h"

#include <atomic>
#include <memory>
#include <vector>

namespace city {

struct CityState;

namespace traffic {

struct YardGate {
	V2 inner, outer; // the gate's middle just inside, and its foot on the lane
	AxlePose stop, join; // the rear axle standing on the lane before the gate and past it
	int ds = -1, lane = 0;
	float stop_at = 0, join_at = 0; // the same along the lane, the body's middle
};

struct YardDock {
	int bay = -1; // ParkingState::bays
	V2 door, out; // the middle of its closed end and the unit direction out of it
};

struct YardInput {
	Steered body;
	double x0 = 0, z0 = 0, cell = 0;
	int nx = 0, nz = 0;
	float street_window = 0; // the street counts only this near a gate: the search does not wander the whole frontage
	float dock_gap = 0, pre_dock = 0, pre_step = 0, reach = 0, turn = 0, settle_step = 0, fit_margin = 0;
	std::vector<V2> lot;
	std::vector<std::vector<V2>> open, shut; // carriageways and gate mouths; other lots and this lot's solid things
	std::vector<YardGate> gates;
	std::vector<YardDock> docks;
};

// A dock's ways, rear-axle poses; `into_street` / `away_street`: where along them the part on the street ends and begins.
struct DockWays {
	int bay = -1, gate = -1; // -1: no way
	std::vector<AxlePose> into, away;
	int into_street = 0, away_street = 0;
};

struct YardPlan {
	std::vector<YardGate> gates;
	std::vector<DockWays> docks;
};

// What a worker fills: `done` is set once `plan` may be read.
struct YardJob {
	YardPlan plan;
	std::atomic<bool> done{ false };
};

bool yard_input(const CityState &s, int building, YardInput &out);
void plan_yard(const YardInput &in, YardPlan &out);

} // namespace traffic
} // namespace city
