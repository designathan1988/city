// The parked side of the traffic, held by TrafficState: the bays near the camera (along the kerbs of the streets that
// park, sim/roads/parking_layout.h, and in the lots, sim/buildings/lot_access.h), the cars that stand in them as scenery
// and the cars of the traffic that drive in and out of the lots. Pure C++; written by sim/traffic/parking_sources.cpp and
// lot_traffic.cpp, read by publish_poses.
#pragma once

#include "core/math.h"
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

// What kind of place a bay's building is: what its cars' trips follow over the day (traffic.json hours_working, hours_purchasing).
enum class PlaceKind : uint8_t { HOME = 0, SHOP, WORK, CIVIC, COUNT };
constexpr int PLACE_KINDS = int(PlaceKind::COUNT);

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
};

// A yard's plan on its way: a worker fills `job`, and it is read from `ready_tick` on.
struct YardEntry {
	std::shared_ptr<traffic::YardJob> job;
	uint64_t ready_tick = 0;
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
	std::unordered_map<int, YardEntry> yards; // by building
	float lorry_clock = 0;
	int lorries_sent = 0, lorries_docked = 0, lorries_left = 0; // probes
	int lorry_calls[LORRY_CALLS] = {};
};

} // namespace city
