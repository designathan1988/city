// The parked side of the traffic, held by TrafficState: the bays near the camera (along the kerbs of the streets that
// park, sim/roads/parking_layout.h, and in the lots, sim/buildings/lot_access.h), the cars that stand in them as scenery
// and the cars of the traffic that drive in and out of the lots. Pure C++; written by sim/traffic/parking_sources.cpp and
// lot_traffic.cpp, read by publish_poses.
#pragma once

#include "core/math.h"
#include "sim/catalog/traffic_params.h"
#include "sim/state/pose_buffer.h"
#include "sim/traffic/manoeuvre.h"
#include "sim/traffic/yard.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace city {

// The place on a lane a vehicle joins it at and stops before a gate at. `at`: arc of the body's centre where it joins;
// `entry`: arc of the body's centre where it stops to turn in; (p, t): the lane's point at `at` and its direction.
struct LaneSpot {
	int ds = -1, lane = 0;
	float at = 0, entry = 0;
	V2 p, t;
};

struct Bay {
	bool alive = false, kerb = false, dock = false;
	bool way_read = false, reached = false; // its way out asked of the lot (reached: it has one, to a lane)
	bool has_row = false; // a scenery car stands in it
	bool vacated = false; // its scenery car drove off: empty until the bay is laid out again
	PlaceKind kind = PlaceKind::HOME;
	int source = -1; // the building or the segment it belongs to
	int holder = -1; // the vehicle coming to it or standing in it
	uint8_t filed = 0; // the list of its kind it is on: 0 none, 1 free, 2 with a scenery car (gated lots' bays only)
	int slot = -1; // its place on that list
	V2 at, open; // centre, and the unit direction out of the stall (a kerb bay: the way the traffic beside it goes)
	float y = 0, room = 0; // ground; half its clear length
	uint8_t row_type = 0, row_paint = 0;
	uint32_t row_id = 0;
	int row_at = -1; // its place in ParkingState::rows
	double stay_until = 0; // a car that parked here stands at least until then
	LaneSpot lane;
	std::vector<V2> via; // from the aisle in front of the stall to just inside the gate
};

struct ParkSource {
	std::vector<int> bays;
};

enum class CarPhase : uint8_t { COMING, PARKING, PARKED, LEAVING };

// A car of the lots: on the road while it is COMING, off the lanes after that (Vehicle::off). Once PARKED it becomes its bay's
// row at the next look, its Vehicle let go: a standing car costs nothing a tick, like a scenery one.
struct LotCar {
	int id = -1, bay = -1;
	CarPhase phase = CarPhase::COMING;
	Manoeuvre path;
	float t = 0, waited = 0; // seconds in the phase; seconds waited for walkers
	bool reserved = false; // holding the ground of the rest of its way: it no longer stops for anybody
	bool closed = false; // it holds the stretches of lane its way sweeps (lane_closures.h take_lanes)
};

// A stretch of lane closed while something off the lanes stands on it. `key`: ds * MAX_LANES + lane.
struct LaneClosure {
	int key = -1;
	float from = 0, to = 0;
	int owner = -1;
};

enum class LorryPhase : uint8_t { COMING, WAITING, ARRIVING, DOCKED, LEAVING, JOINING };
// Where a call for a lorry stops, for the probes.
enum LorryCall { CALL_NO_DOCK, CALL_PLANNING, CALL_NO_WAY, CALL_NO_TRUCK, CALL_NO_ROUTE, CALL_SENT, LORRY_CALLS };

// A truck called to a dock (lorry_traffic.cpp); the same Vehicle all along, `off` once it leaves the lanes.
struct Lorry {
	int id = -1, building = -1, bay = -1;
	LorryPhase phase = LorryPhase::COMING;
	traffic::YardGate gate;
	std::vector<traffic::AxlePose> into, away;
	int into_street = 0, away_street = 0;
	float street_at = 0; // along `path`, where the part on the street ends (arriving) or begins (leaving)
	std::vector<LaneClosure> close_in, close_out;
	bool closed = false;
	bool in_lot = false; // DockWays::in_lot: the street is behind it once it is past street_at; else never while it is docked
	Manoeuvre path;
	float t = 0, stay = 0, waited = 0;
};

struct ParkingState {
	std::vector<Bay> bays;
	std::vector<int> free_bays;
	std::unordered_map<int, ParkSource> lots, kerbs; // by building, by segment
	std::vector<int> lots_wanted, kerbs_wanted; // to read, nearest last
	std::vector<int> rows; // the bays with a car standing in them
	bool rows_dirty = true; // `published` is to be made again
	std::shared_ptr<const std::vector<ParkedRecord>> published; // what the render reads: shared by the frames until a row changes
	std::vector<int> free_of[PLACE_KINDS], row_of[PLACE_KINDS]; // the bays free / with a scenery car, by kind, gated lots only
	std::vector<LotCar> cars; // Vehicle::car is its index here
	float win_x = 0, win_z = 0;
	bool looked = false, relook = false;
	uint32_t next_row = 0;
	float clock = 0, owed = 0; // seconds since the last look; trips owed to the lots
	int coming = 0, moving = 0; // cars on their way, and driving in or out of a bay now
	int called = 0, parked_in = 0, left_by = 0; // probes: cars sent in, parked, driven out
	std::vector<Lorry> lorries; // Vehicle::lorry is its index here
	std::unordered_map<int, std::shared_ptr<const traffic::YardPlan>> yards; // by building, the ones planned
	std::shared_ptr<traffic::YardPlanner> planner; // the yard being planned, a little each tick
	int planner_building = -1;
	double plan_ms = 0, plan_slowest_ms = 0, plan_tick_ms = 0; // probes, never read by the simulation: this plan so far, the worst plan, the worst tick
	float lorry_clock = 0;
	int lorries_sent = 0, lorries_docked = 0, lorries_left = 0; // probes
	int lorry_calls[LORRY_CALLS] = {};
	// By building: the vehicle using its lot's way between the gate and the bays now. That way is one body wide, so one mover holds it
	// at a time (a car driving in, a car driving out, a lorry for its whole visit, as the 4.0's lots wait for a lorry shunting):
	// two that took it together stood inside each other at the gate.
	std::unordered_map<int, int> access;
	// The stretches of kerb a mover goes over between its lane and a lot, each held by one mover: the gates of two lots side by side
	// share their stretch (two movers there, one coming in and one going out, would each stand in the other's way for good).
	struct Crossing {
		int key = -1; // the lane, ds * MAX_LANES + lane
		float from = 0, to = 0; // its arcs on that lane
		int holder = -1;
	};
	std::vector<Crossing> crossings;
	// The lot's way and its stretch of kerb, both or neither: whether `id` holds them now.
	bool take_access(int building, int id, int key, float from, float to) {
		const auto lot = access.find(building);
		if (lot != access.end() && lot->second != id) return false;
		bool mine = false;
		for (const Crossing &c : crossings) {
			if (c.holder == id) mine = true;
			else if (c.key == key && c.from < to && c.to > from) return false;
		}
		access[building] = id;
		if (!mine) crossings.push_back({ key, from, to, id });
		return true;
	}
	void let_access(int building, int id) {
		const auto it = access.find(building);
		if (it != access.end() && it->second == id) access.erase(it);
		for (size_t i = crossings.size(); i-- > 0;)
			if (crossings[i].holder == id) crossings.erase(crossings.begin() + std::ptrdiff_t(i));
	}
};

} // namespace city
