// Traffic data owned by CityState: the lane graph derived from the road network (directed segments, lane connectors
// through junctions, junction control), the vehicles and pedestrians, per-lane ordering and connector occupancy.
// Pure C++ (no Godot types); written only by sim/traffic, read by render/agents.
#pragma once

#include "core/random.h"
#include "sim/catalog/traffic_params.h"
#include "sim/state/pose_buffer.h"
#include "sim/traffic/parking_state.h"
#include "sim/traffic/ped_state.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

constexpr int MAX_LANES = 4; // lanes per direction (the widest one-way profile has 4)
constexpr int CONN_STEPS = 8; // arc-length table of a connector curve
constexpr int NODE_ARMS_MAX = 8; // arms a node may have in the crosswalk answer cache
constexpr int CROSS_CACHE_KINDS = 2; // walk_ok and cars_on_crossing are cached apart
constexpr uint32_t STAMP_NONE = 0xFFFFFFFFu; // crosswalk cache: no step answered yet
enum StepStage { STAGE_BAYS = 0, STAGE_FLEET, STAGE_LOTS, STAGE_WALKERS, STAGE_LOT_OUT, STAGE_LOT_IN, STAGE_PUBLISH, TIMED_STAGES };
constexpr uint64_t TRAFFIC_SEED = 12345; // Roadcraft traffic.ts: Rng(12345)

enum class Turn : uint8_t { STRAIGHT = 0, RIGHT = 1, LEFT = 2, UTURN = 3 };
enum class Control : uint8_t { NONE = 0, PRIORITY = 1, SIGNAL = 2 };
enum class Light : uint8_t { GREEN = 0, AMBER = 1, RED = 2 };
// Why a vehicle is not moving (traffic_probe, tests).
enum class Hold : uint8_t { QUEUED = 0, WRONG_LANE, RED, EXIT_ROOM, CONFLICT_BUSY, YIELD, ON_CONNECTOR, FREE, CROSSING, COUNT };

// One direction of a segment: ds = seg * 2 + dir; dir 0 runs a -> b on the profile's fwd lanes, dir 1 b -> a on back.
// s = 0 at the entry mouth, s = len at the exit mouth (stop line).
struct DirSeg {
	bool ok = false;
	int seg = -1, from = -1, to = -1, lanes = 0;
	float u[MAX_LANES] = {}; // profile offsets (a -> b frame) of lane 0 (rightmost) ... lanes-1
	float len = 0, speed = 0; // metres, m/s
	float at0 = 0, sign = 1; // arc length along the segment at s = 0 and its direction
	float ex = 0, ez = 0; // endpoint (s = len) in world XZ, computed once at graph build (A* heuristic)
	int stage = -1; // signal stage serving this approach at node `to`
	int rank = 2; // right of way at node `to`: 3 main, 2 free, 1 yield
	float stop_back = 0; // the stop line is this far before s = len (behind the crosswalk at node `to`)
	float cycle_shift = 0; // a bike strip beside lane 0: how far across (u, a -> b frame), 0 for none
	bool bikes = true; // bicycles may ride it (the street has a footway)
	float detector = 0, extension = 0; // a signalled approach: its detection section before the line, green per call
};

// A lane connector through a node: quadratic Bézier from the end of an in-lane to the start of an out-lane.
// Connections live in one pool with a free list; each node and each lane of origin keeps a linked list of them
// (Roadcraft lanes.ts nodeConn/laneConn), so a node rebuild touches only its own connections.
struct Conn {
	int node = -1, from_ds = -1, from_lane = 0, to_ds = -1, to_lane = 0;
	Turn turn = Turn::STRAIGHT;
	float x0 = 0, z0 = 0, cx = 0, cz = 0, x1 = 0, z1 = 0, y0 = 0, y1 = 0;
	float table[CONN_STEPS + 1] = {};
	float len = 0;
	int next_at_node = -1, next_from = -1; // next connection of the same node / of the same lane of origin
	bool alive = false;
	std::vector<int> conflicts;
	std::vector<float> conflict_at; // per conflict: metres along THAT connector after which its vehicles are clear
};

struct Junction {
	Control ctl = Control::NONE;
	// per stage, seconds: minimum and maximum green, then the intergreen (amber, all-red); its approaches
	std::vector<float> green, green_max, amber, all_red;
	std::vector<std::vector<int>> approaches;
	float cycle = 0; // with the minimum greens
	int run = 0; // the stage running, since `run_start` (its green); its green once ended (-1 while the green lasts)
	double run_start = 0;
	float run_green = -1;
	int degree = 0; // arms with lanes
};

struct LaneGraph {
	int version = -1; // RoadNetwork::version it was built from
	std::vector<DirSeg> ds;
	std::vector<Conn> conns; // pool: dead entries are on free_conns
	std::vector<int> free_conns;
	std::vector<int> node_conn; // per node: first connection of its list (-1)
	std::vector<int> lane_conn; // per ds * MAX_LANES + lane: first connection leaving it (-1)
	std::vector<Junction> junctions; // per node
	double lane_metres = 0;
};

// Walks the connections leaving `ds` (all its lanes), skipping dead pool entries.
template <typename F>
void each_conn_from(const LaneGraph &g, int ds, F &&fn) {
	for (int lane = 0; lane < MAX_LANES; lane++) {
		for (int c = g.lane_conn[size_t(ds) * MAX_LANES + lane]; c >= 0; c = g.conns[c].next_from)
			if (g.conns[c].alive) fn(c);
	}
}

struct Vehicle {
	bool alive = false, brake = false;
	bool micro = true; // drawn and driven by the near model; far vehicles are in their lane's meso queue
	int micro_idx = -1; // index in TrafficState::micro_list
	int ml = -1, mnext = -1, mprev = -1; // meso queue of its lane (doubly linked, in order of entry)
	double t_in = 0, t_exit = 0; // when it joined its lane in meso and when its event is due
	float s0 = 0; // lane position when it joined meso (materialize interpolates from it)
	VehicleType type = VehicleType::SEDAN;
	uint8_t paint = 0;
	int ds = -1, lane = 0, conn = -1;
	float s = 0, v = 0, acc = 0, cs = 0; // lane position, speed, acceleration, metres along the connector
	float vf = 1, drift = 0; // share of the speed limit, phase of the speed drift
	float lc_off = 0, lc_cool = 0; // lateral offset left after a lane change (decays), cooldown
	float wait = 0, stuck = 0;
	float held = 0; // stopped since it last reconsidered the route (Roadcraft micro.ts c.held)
	int pass = -1; // connector it committed to at its line with the way open (Roadcraft c.pass + exitC)
	int amber_hold = -1; // approach whose amber it chose to stop for, held until green
	bool off = false; // off the lanes (in or out of a lot): on no lane list, not in the fleet count; `cur` is its pose
	int car = -1; // its index in ParkingState::cars
	float stop_at = -1; // called to a gate: on its last lane it stops with its front here, on lane stop_lane
	int stop_lane = 0;
	bool stood = false; // stood at the stop line of the lane it is on (traffic_probe: a red entry from a standstill)
	float goal_s = 0;
	RouteRef route; // directed segments, in TrafficState::route_pool
	int ri = 0;
	Pose cur; // at the last publication
	PoseTrail trail;
};

struct TrafficState {
	int64_t stamp_ns[POSE_TRAIL] = {}; // scheduled times of the last pose publications (publish_poses)
	LaneGraph g;
	std::vector<Vehicle> vehicles;
	std::vector<int> free_vehicles;
	std::vector<int> route_pool; // vehicle routes, kept in one block
	std::vector<int> ped_pool; // pedestrian routes
	size_t route_waste = 0, ped_waste = 0;
	std::vector<Pedestrian> peds;
	std::vector<int> free_peds;
	// The light each approach shows, refreshed once per step: light_for() is asked many times per vehicle and conflict.
	std::vector<uint8_t> light_now;
	std::vector<double> green_wanted; // per approach: until when the vehicles over its detector want green
	std::vector<std::vector<int>> lane_vehicles; // ds * MAX_LANES + lane, sorted by s (back first)
	std::vector<std::vector<int>> conn_vehicles; // in entry order
	std::vector<std::vector<int>> node_crossers; // per node: walkers on one of its crosswalks
	std::unordered_map<int64_t, uint32_t> kerb_slots; // per kerb end of a crosswalk: the places to wait taken (bits)
	// Who walks near whom, rebuilt every step (ped_crowd.cpp): each sidewalk's walkers by arc (seg * 2 + side) and the
	// sidewalks with walkers; each junction's walkers (round its corners, at its kerbs, or on a sidewalk within reach of
	// it: SUMO's walking area) and the junctions with walkers.
	std::vector<std::vector<CrowdEntry>> crowd;
	std::vector<int> crowd_keys;
	std::vector<std::vector<CrowdEntry>> node_walkers;
	std::vector<int> node_keys;
	// The street furniture of the segments the walkers met, as bodies standing on its sidewalks (ped_furniture.cpp);
	// dropped when an edit touches the segment or a node of it, and far from the camera.
	std::unordered_map<int, SidewalkFurniture> furniture;
	// Far (meso) model, Roadcraft traffic/meso.ts: one queue per lane in order of entry and one event per vehicle at
	// the end of its lane; lane_last_exit spaces the queues; micro_list holds the near vehicles only.
	std::vector<int> meso_head, meso_tail;
	std::vector<uint16_t> meso_count;
	std::vector<double> lane_last_exit;
	std::vector<double> ev_t; // heap of events (time, vehicle)
	std::vector<int> ev_v;
	size_t ev_n = 0;
	std::vector<int> micro_list;
	// Lanes and connectors that hold a near vehicle (step_vehicles walks these, never the whole map).
	std::vector<int> active_lanes, active_conns;
	std::vector<uint8_t> lane_active, conn_active;
	std::vector<int> step_lanes, step_conns; // per-step snapshot of the active lists
	// Micro window (Roadcraft traffic.ts checkWindow/materialize): last camera position and time, sidewalk metres and
	// the walkable segments inside ped_window_m.
	float win_x = 0, win_z = 0;
	double win_t = -double(BIG);
	double ped_metres = 0;
	std::vector<int> ped_segs;
	// Crosswalk answers per (node, arm) for the current step (ped_crossing.cpp).
	std::vector<uint32_t> cross_stamp;
	std::vector<uint8_t> cross_walk, cross_cars;
	// Anti-gridlock bookkeeping (Roadcraft traffic/junctions.ts, micro.ts): who committed to each connector and when,
	// who holds the next free spot of a full lane (and when it last asked), when each node last let someone in and
	// when it may release the next vehicle of a jam.
	std::vector<int> conn_pass;
	std::vector<double> conn_pass_t;
	std::vector<double> conn_claim_t; // last time a vehicle that waited its share at its line claimed the connector
	std::vector<int> exit_claim; // per lane key
	std::vector<double> exit_claim_t;
	std::vector<double> last_admit, release_at; // per node
	std::vector<int> pass_count; // per node: vehicles let in (pedestrian courtesy, tests)
	int route_budget = 0; // route searches left this step (spawns, detours, reconsiderations)
	float gov = 1, gov_clock = 0; // fleet governor (traffic/regions.ts)
	int reroutes = 0, released = 0, jams = 0;
	float red_last[TIMED_STAGES] = {}; // the last red entry: type, speed, ds, time (traffic_probe)
	int red_entries = 0, red_from_stop = 0; // over the stop line on red (probe): all, and after standing at it (a violation)
	int vehicle_count = 0, ped_count = 0, arrived = 0, lost = 0, ped_arrived = 0, ped_waiting = 0, stuck_lost = 0;
	double time = 0; // simulated seconds
	uint64_t tick = 0;
	float spawn_credit = 0;
	// Where a step goes (traffic_probe step_ms, summed since the start): the bays read, the fleet (demand, far model, lane
	// changes, motion), the lots, the walkers; then, of the lots, the bays' ways and the routes of a look, and the publication.
	double step_ms[TIMED_STAGES] = {};
	std::vector<float> pub_sum, pub_lim; // publish_poses: per segment, kept between ticks
	std::vector<int> pub_count;
	ParkingState park;
	std::unordered_map<int64_t, std::vector<int>> walker_cells; // who stands where, filed when something off the lanes asks
	uint64_t walkers_filed = ~0ull; // the tick they were filed in
	Rng rng{ TRAFFIC_SEED };
};

inline int route_at(const std::vector<int> &pool, const RouteRef &r, int k) {
	return k >= 0 && k < r.len ? pool[size_t(r.off + k)] : -1;
}
void route_set(std::vector<int> &pool, RouteRef &r, size_t &waste, const int *src, int n);
void route_compact(TrafficState &t);

} // namespace city
