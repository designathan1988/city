// Traffic data owned by CityState: the lane graph derived from the road network (directed segments, lane connectors
// through junctions, junction control), the vehicles and pedestrians, per-lane ordering and connector occupancy.
// Pure C++ (no Godot types); written only by sim/traffic, read by render/agents.
#pragma once

#include "core/math.h"
#include "core/random.h"
#include "sim/traffic/flow_control.h"
#include "sim/catalog/traffic_params.h"
#include "sim/state/pose_buffer.h"
#include "sim/traffic/lane_graph_types.h"
#include "sim/traffic/parking_state.h"
#include "sim/traffic/ped_state.h"
#include "sim/traffic/ring_meter.h"
#include "sim/traffic/route_reach.h"
#include "sim/traffic/traffic_view.h"

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

namespace city {

constexpr int NODE_ARMS_MAX = 8; // arms a node may have in the crosswalk answer cache
constexpr int CROSS_CACHE_KINDS = 2; // walk_ok and cars_on_crossing are cached apart
constexpr uint32_t STAMP_NONE = 0xFFFFFFFFu; // crosswalk cache: no step answered yet
enum StepStage { STAGE_BAYS = 0, STAGE_FLEET, STAGE_LOTS, STAGE_WALKERS, STAGE_LOT_OUT, STAGE_LOT_IN, STAGE_PUBLISH, TIMED_STAGES };
constexpr uint64_t TRAFFIC_SEED = 12345; // Roadcraft traffic.ts: Rng(12345)

// A vehicle's paint from here up is the colour of a transport line, not one of the paints (agent_renderer.cpp).
constexpr int LINE_PAINT = 64;
// An errand at the kerb (kerb_stops.cpp): somebody set down, somebody picked up, a truck's mate away with a delivery.
enum class Errand : uint8_t { NONE = 0, DROP, PICK, DELIVER };
enum class KerbPhase : uint8_t { APPROACH, HALT, FETCH, OPEN, TRANSFER, AWAY, CLOSE };
// Vehicle::aboard, what the render does to its passengers: as drawn, at least one to set down, one less, one more.
constexpr uint8_t ABOARD_AS_DRAWN = 0, ABOARD_TO_SET_DOWN = 1, ABOARD_SET_DOWN = 2, ABOARD_PICKED_UP = 3;
struct KerbStop {
	int vehicle = -1, ds = -1, lane = 0, ped = -1;
	Errand errand = Errand::NONE;
	KerbPhase phase = KerbPhase::APPROACH;
	bool back = false; // the mate is on the way back
	float at = 0, t = 0, elapsed = 0, fetch = 0, walked = 0, transfer = 0; // where its front stops; seconds in the phase and in all; the moves
	V2 from; // where the person to pick up set off from
};


// A tick's route searches (route_admission.cpp): the tick they were counted in and how many started.
struct SearchBudget {
	uint64_t tick = ~0ull;
	int used = 0;
};

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
	// A lane change under way (lane_shift.h): the sideways offset still to cover, as it started, the road driven since and the change's length;
	// the lane it still stands in (its shadow, -1 none) and the offset under which it has left it; the lane it signals for and since when.
	float lat = 0, lat_start = 0, lat_done = 0, lat_len = 0, lc_cool = 0;
	int shadow_key = -1, lc_intent = -1;
	float shadow_clear = 0, lc_intent_t = 0;
	float wait = 0, stuck = 0;
	float held = 0; // stopped since it last reconsidered the route (Roadcraft micro.ts c.held)
	int pass = -1; // connector it committed to at its line with the way open (Roadcraft c.pass + exitC)
	int amber_hold = -1; // approach whose amber it chose to stop for, held until green
	int stood_at = -1; // approach whose Stop it has come to a stand at: it may go on from there
	int bus = -1; // its place in TransitRun::buses
	float bus_doors = 0; // how far the bus's doors stand open, left here by the transit step
	float bus_stop_at = -1; // a bus: on its last lane it stops with its front here
	uint8_t riders = 0; // a bus: who sits behind the driver, drawn at its birth and counted by who gets off and on (bus_calls.cpp)
	bool off = false; // off the lanes (in or out of a lot): on no lane list, not in the fleet count; `cur` is its pose
	int car = -1; // its index in ParkingState::cars
	int lorry = -1; // its index in ParkingState::lorries
	float stop_at = -1; // called to a gate: on its last lane it stops with its front here, on lane stop_lane
	int stop_lane = 0;
	Errand errand = Errand::NONE; // still to do on this trip
	int kerb = -1; // its stop in TrafficState::kerb_stops
	uint8_t aboard = ABOARD_AS_DRAWN;
	uint8_t shots = 0; // rounds taken on the bodywork (the pistol): enough of them and it burns
	bool stood = false; // stood at the stop line of the lane it is on (traffic_probe: a red entry from a standstill)
	float goal_s = 0;
	bool ended = false; // its trip ended where it may be seen (sight.h): it stands short of its lane's end until it is out of sight
	RouteRef route; // directed segments, in TrafficState::route_pool
	int ri = 0;
	Pose cur; // at the last publication
	PoseTrail trail;
};

struct TrafficState {
	int64_t stamp_ns[POSE_TRAIL] = {}; // scheduled times of the last pose publications (publish_poses)
	LaneGraph g;
	FlowControl flow_control;
	std::vector<Vehicle> vehicles;
	std::vector<int> free_vehicles;
	std::vector<int> route_pool; // vehicle routes, kept in one block
	std::vector<int> ped_pool; // pedestrian routes
	size_t route_waste = 0, ped_waste = 0;
	std::vector<PedShock> shocks; // what walkers run from (ped_fright.cpp)
	float shock_look = 0; // t.time they are looked at next
	std::string graph_fault; // the debug DLL: the first broken rule of the lane graph (lane_graph_check.cpp)
	std::vector<Pedestrian> peds;
	std::vector<int> free_peds;
	// The light each approach shows, refreshed once per step: light_for() is asked many times per vehicle and conflict.
	std::vector<uint8_t> light_now;
	std::vector<int> lit_ds; // the approaches update_lights set last: green again before it sets this step's
	std::vector<uint32_t> seg_volume; // per segment: vehicles that entered it since the city opened (the Inspect tool)
	// The segments entered, in order, since entry `volume_base` (counted from the first ever): each published view copies only
	// the volumes of those entered since it last did (traffic_view.cpp).
	std::vector<int> volume_log;
	uint64_t volume_base = 0;
	std::vector<double> green_wanted; // per approach: until when the vehicles over its detector want green
	std::vector<std::vector<int>> lane_vehicles; // ds * MAX_LANES + lane, sorted by s (back first)
	std::vector<std::vector<int>> lane_shadows; // the vehicles changing out of each lane that still stand in it (lane_shift.h)
	// Per lane key: its list was in order of s when the step last moved it (vehicle_motion.cpp). Only that step changes the s of a
	// vehicle on a list; an insertion keeps a list in order and a removal keeps it so.
	std::vector<uint8_t> lane_sorted;
	std::vector<std::vector<int>> conn_vehicles; // in entry order
	std::vector<std::vector<int>> node_crossers; // per node: walkers on one of its crosswalks
	std::unordered_map<int64_t, uint32_t> kerb_slots; // per kerb end of a crosswalk: the places to wait taken (bits)
	// Who walks near whom, rebuilt every step (ped_crowd.cpp): each sidewalk's walkers by arc (seg * 2 + side) and the
	// sidewalks with walkers; each junction's walkers (round its corners, at its kerbs, or on a sidewalk within reach of
	// it: SUMO's walking area) and the junctions with walkers.
	std::vector<std::vector<CrowdEntry>> crowd;
	std::vector<int> crowd_keys;
	std::vector<size_t> crowd_first; // per crowd key: where its walkers' choices start in the step's list
	std::vector<std::vector<CrowdEntry>> node_walkers;
	std::vector<int> node_keys;
	std::vector<CrowdChoice> crowd_next; // the step's choices, kept between steps for their room
	std::vector<const StreetObstacle *> crowd_around; // the furniture round the junction being looked at
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
	std::vector<int> lane_active, conn_active; // per lane key / connector: its place in the active list plus one, 0 when off it
	std::vector<int> step_lanes, step_conns; // per-step snapshot of the active lists
	// Micro window (Roadcraft traffic.ts checkWindow/materialize): last camera position and time, sidewalk metres and
	// the walkable segments inside ped_window_m.
	float win_x = 0, win_z = 0;
	double win_t = -double(BIG);
	double ped_kind_m[PLACE_KINDS] = {}; // metres of street of each kind of place inside the window
	std::vector<int> ped_segs;
	// The doors the walkers use (ped_doors.cpp): by sidewalk (seg * 2 + side), the lots read (seed + 1 by building id) and
	// those to read, a few a step.
	std::vector<std::vector<PedDoor>> doors;
	std::vector<int> door_sides; // the sidewalks with doors (a sidewalk that lost them is dropped when drawn)
	std::vector<uint32_t> door_seen;
	std::vector<int> door_todo;
	int doors_out = 0, doors_in = 0; // walkers who came out of a door and went in at one
	// Crosswalk answers per (node, arm) for the current step (ped_crossing.cpp).
	std::vector<uint32_t> cross_stamp;
	std::vector<uint8_t> cross_walk, cross_cars;
	// Per (node, arm) of the cache: when the traffic last held a walker at that crosswalk's kerb, and since when it has (-1: nobody
	// held now); the walkers' impatience and the vehicles that give way to those waiting read them (ped_crossing.cpp).
	std::vector<double> cross_held, cross_held_from;
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
	int stop_entries = 0, stop_skipped = 0; // vehicles let in past a Stop, and those that had not stood at it (tests)
	SearchBudget search_budget; // the route searches of this tick, every one counted (route_admission.cpp)
	// Per directed segment: one connector leaving it for each segment its connectors reach, the first each_conn_from walks to it, made
	// again when CityState::lane_graph_serial moves (stamped serial + 1): the router relaxes each segment it can reach once (routing.cpp).
	std::vector<std::vector<int>> route_next;
	std::vector<uint64_t> route_next_stamp;
	RouteReach reach; // which segments lead to which, cars and bicycles (route_reach.cpp)
	RingIndex rings; // the small closed loops of road and what they hold (ring_meter.cpp)
	float gov = 1, gov_clock = 0; // fleet governor (traffic/regions.ts)
	int reroutes = 0, released = 0, jams = 0;
	float red_last[TIMED_STAGES] = {}; // the last red entry: type, speed, ds, time (traffic_probe)
	int red_entries = 0, red_from_stop = 0; // over the stop line on red (probe): all, and after standing at it (a violation)
	int vehicle_count = 0, ped_count = 0, arrived = 0, lost = 0, ped_arrived = 0, ped_waiting = 0;
	// near vehicles and walkers taken away where the player may see them (sight.h: none, but by the player's own hand)
	int gone_in_sight = 0, ped_gone_in_sight = 0;
	double time = 0; // simulated seconds
	uint64_t tick = 0;
	float spawn_credit = 0;
	// Where a step goes (traffic_probe step_ms, summed since the start): the bays read, the fleet (demand, far model, lane
	// changes, motion), the lots, the walkers; then, of the lots, the bays' ways and the routes of a look, and the publication.
	double step_ms[TIMED_STAGES] = {};
	std::vector<KerbStop> kerb_stops; // few: the vehicles pulling up or standing at the kerb
	std::vector<int> free_kerb_stops;
	int kerb_planned = 0, kerb_drops = 0, kerb_picks = 0, kerb_deliveries = 0, kerb_abandoned = 0;
	int bus_boarded = 0, bus_alighted = 0; // who has got on and off the buses
	std::vector<LaneClosure> closures; // few, and only while a lorry swings across a street
	std::vector<float> pub_sum, pub_lim; // publish_poses: per segment, kept between ticks (zero but during a pass)
	std::vector<int> pub_count;
	std::vector<int> pub_segs; // the segments the last pass counted vehicles on
	// Per frame of CityState::poses (by its address, they stand as long as the city): the segments whose speed share it shows
	// other than 1, so its next filling puts back only those.
	struct ShareFrame {
		const PoseFrame *frame = nullptr;
		std::vector<int> segs;
	};
	ShareFrame share_frames[3];
	ParkingState park;
	WalkerCells walker_cells; // who stands where, filed when something off the lanes asks (walker_cells.cpp)
	WalkerCells spawn_cells; // who stands where while walkers are born
	Rng rng{ TRAFFIC_SEED };
};

inline int route_at(const std::vector<int> &pool, const RouteRef &r, int k) {
	return k >= 0 && k < r.len ? pool[size_t(r.off + k)] : -1;
}
void route_set(std::vector<int> &pool, RouteRef &r, size_t &waste, const int *src, int n);
void route_compact(TrafficState &t);

// A vehicle enters the directed segment: it counts on that road's volume.
inline void count_entry(TrafficState &t, int ds) {
	const size_t seg = size_t(ds) / 2;
	if (seg >= t.seg_volume.size()) t.seg_volume.resize(seg + 1, 0);
	t.seg_volume[seg]++;
	// a log longer than the counts costs more to read than a copy of them: the views behind it take them whole
	if (t.volume_log.size() >= t.seg_volume.size()) {
		t.volume_base += t.volume_log.size();
		t.volume_log.clear();
	}
	t.volume_log.push_back(int(seg));
}

} // namespace city
