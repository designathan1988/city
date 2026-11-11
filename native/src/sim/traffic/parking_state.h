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
// `entry`: arc of the body's centre where it stops to turn in; `foot`: arc of the gate's foot; (p, t): the lane's point at `at`
// and its direction.
struct LaneSpot {
	int ds = -1, lane = 0;
	float at = 0, entry = 0, foot = 0;
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

// The swath of a vehicle off the lanes (swaths.h): discs of its body where it stands (the first `body`) and, while it drives in or out,
// of the rest of its way; the order it started on that way (0: it has not) and whether the way leads out to the street.
struct SwathDisc {
	float x = 0, z = 0, r = 0;
};
struct Swath {
	float x0 = 0, z0 = 0, x1 = 0, z1 = 0; // the box of its discs
	int first = 0, count = 0, body = 0; // in ParkingState::swath_discs
	int vehicle = -1;
	uint32_t order = 0;
	bool outward = false;
};

enum class CarPhase : uint8_t { COMING, PARKING, PARKED, LEAVING };

// A car of the lots: on the road while it is COMING, off the lanes after that (Vehicle::off). Once PARKED it becomes its bay's
// row at the next look, its Vehicle let go: a standing car costs nothing a tick, like a scenery one. Several cars of a lot drive at
// once, kept apart by their swaths (swaths.h).
struct LotCar {
	int id = -1, bay = -1;
	CarPhase phase = CarPhase::COMING;
	Manoeuvre path;
	float t = 0, waited = 0; // seconds in the phase; seconds waited for walkers
	// When it started on its way (ParkingState::starts; 0: it has not): from then on the rest of its way is its swath, it waits for no
	// walker and gives way only to the swaths of those that started before it (swaths.h).
	uint32_t start_order = 0;
	std::vector<SwathDisc> way; // its body along `path`, laid once when the path is made (swaths.h lay_way)
	// Its lane is closed to the traffic behind while its body stands on it (lot_traffic.cpp, LaneClosure owned by its vehicle id):
	// going in, until its way has carried every corner past the lane's edge, `lane_clear_at` metres along it; coming out, from when
	// it goes at the kerb until it is on the lane's list.
	float lane_clear_at = 0;
	bool closing = false;
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
	Manoeuvre path;
	float t = 0, stay = 0, waited = 0;
	uint32_t start_order = 0; // when it started on the way it drives now (into the yard or out of it), as a car's (LotCar)
	std::vector<SwathDisc> way; // its body along `path`, laid once when the path is made (swaths.h lay_way)
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
	// The swaths of the cars and lorries off the lanes, filed before the lots move and before the walkers step (swaths.cpp), and the
	// orders handed to those starting on their way.
	std::vector<SwathDisc> swath_discs;
	std::vector<Swath> swaths;
	uint32_t starts = 0;
	std::unordered_map<int, std::shared_ptr<const traffic::YardPlan>> yards; // by building, the ones planned
	std::shared_ptr<traffic::YardPlanner> planner; // the yard being planned, a little each tick
	int planner_building = -1;
	double plan_ms = 0, plan_slowest_ms = 0, plan_tick_ms = 0; // probes, never read by the simulation: this plan so far, the worst plan, the worst tick
	float lorry_clock = 0;
	int lorries_sent = 0, lorries_docked = 0, lorries_left = 0; // probes
	int lorry_calls[LORRY_CALLS] = {};
};

} // namespace city
