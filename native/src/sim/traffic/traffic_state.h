// Traffic data owned by CityState: the lane graph derived from the road network (directed segments, lane connectors
// through junctions, junction control), the vehicles and pedestrians, per-lane ordering and connector occupancy.
// Pure C++ (no Godot types); written only by sim/traffic, read by render/agents.
#pragma once

#include "core/random.h"
#include "sim/catalog/traffic_params.h"
#include "sim/state/pose_buffer.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

constexpr int MAX_LANES = 4; // lanes per direction (the widest one-way profile has 4)
constexpr int CONN_STEPS = 8; // arc-length table of a connector curve
constexpr int NODE_ARMS_MAX = 8; // arms a node may have in the crosswalk answer cache
constexpr int CROSS_CACHE_KINDS = 2; // walk_ok and cars_on_crossing are cached apart
constexpr uint32_t STAMP_NONE = 0xFFFFFFFFu; // crosswalk cache: no step answered yet
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
	std::vector<float> green, amber; // per stage, seconds
	float cycle = 0, offset = 0;
	int degree = 0; // arms with lanes
};

// A route is a slice of the traffic/people pool: no heap allocation per vehicle or walker.
struct RouteRef {
	int off = -1, len = 0, cap = 0;
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
	bool stood = false; // stood at the stop line of the lane it is on (traffic_probe: a red entry from a standstill)
	float goal_s = 0;
	RouteRef route; // directed segments, in TrafficState::route_pool
	int ri = 0;
	Pose cur; // at the last publication
	PoseTrail trail;
};

// A point on a walker's way round a corner: `cross` is the arm segment crossed on the way to it (-1: along the kerb).
struct Waypoint {
	float x = 0, y = 0, z = 0;
	int cross = -1;
};
constexpr int MAX_WAYS = 12; // two per arm crossed (at most 3 at a 6-arm node) and the exit sidewalk start

enum class PedMode : uint8_t { SIDEWALK = 0, WAY = 1, WAIT = 2 };

// A piece of street furniture as a body standing on a sidewalk: its footprint along the segment's arc (s0..s1) and
// across from the sidewalk's middle (u0..u1), and in the world (centre, the arc's direction there, half sizes).
struct StreetObstacle {
	float s0 = 0, s1 = 0, u0 = 0, u1 = 0;
	float x = 0, z = 0, fx = 1, fz = 0, half_along = 0, half_across = 0;
};
struct SidewalkFurniture {
	std::vector<StreetObstacle> side[2]; // right (0) and left (1) sidewalk, by s0
	float longest = 0; // the longest footprint along the arc (a search by s0 starts this far back)
};

// A walker with a destination (Roadcraft peds.ts): a route of nodes over streets with sidewalks; along a sidewalk on
// its own track, round each corner and over the crosswalks (waiting at the kerb for the walk phase or a gap).
struct Pedestrian {
	bool alive = false;
	PedMode mode = PedMode::SIDEWALK;
	uint32_t look = 0; // render hash: clothes, skin, hair
	int seg = -1, dir = 0, side = 0; // walking a -> b (dir 0) on the right (side 0, -u) or left sidewalk
	float s = 0, speed = 0, track = 0, walked = 0, wait = 0, scale = 1;
	float cross_cd = 0; // seconds until a WAIT walker tries the crosswalk again (5 Hz, not every step)
	RouteRef route; // nodes; route[ri] is the node it walks toward, in TrafficState::ped_pool
	int ri = 0;
	Waypoint ways[MAX_WAYS];
	int wn = 0, wi = 0, way_node = -1; // waypoints round node `way_node`, the next one
	float lx = 0, ly = 0, lz = 0, llen = 0, ls = 0; // current straight line: start, length, metres walked on it
	int cross_seg = -1; // the arm it is crossing now (-1 when not on a crosswalk)
	// on a sidewalk (ped_crowd.cpp): across it, metres from its middle (u), the stripe aimed at, how long that has
	// held and the pace the walkers ahead leave it
	float u = 0, u_goal = 0, stripe_held = 0, pace = 0;
	int64_t slot_key = -1; // the kerb a place to wait is held at (TrafficState::kerb_slots), and which place
	int slot = -1;
	Pose cur;
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
	std::vector<std::vector<int>> lane_vehicles; // ds * MAX_LANES + lane, sorted by s (back first)
	std::vector<std::vector<int>> conn_vehicles; // in entry order
	std::vector<std::vector<int>> node_crossers; // per node: walkers on one of its crosswalks
	std::unordered_map<int64_t, uint32_t> kerb_slots; // per kerb end of a crosswalk: the places to wait taken (bits)
	std::vector<std::vector<int>> crowd; // per sidewalk (seg * 2 + side): its walkers by arc, rebuilt every step
	std::vector<int> crowd_keys; // the sidewalks of `crowd` with walkers this step
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
	int red_entries = 0, red_from_stop = 0; // over the stop line on red (probe): all, and after standing at it (a violation)
	int vehicle_count = 0, ped_count = 0, arrived = 0, lost = 0, ped_arrived = 0, ped_waiting = 0, stuck_lost = 0;
	double time = 0; // simulated seconds
	uint64_t tick = 0;
	float spawn_credit = 0;
	Rng rng{ TRAFFIC_SEED };
};

inline int route_at(const std::vector<int> &pool, const RouteRef &r, int k) {
	return k >= 0 && k < r.len ? pool[size_t(r.off + k)] : -1;
}
void route_set(std::vector<int> &pool, RouteRef &r, size_t &waste, const int *src, int n);
void route_compact(TrafficState &t);

} // namespace city
