// Traffic behaviour on top of CityState (Roadcraft src/sim/**): lane graph, demand and spawning, A* routing, IDM car
// following, MOBIL lane changes, junction control with timed signals, pedestrians on the sidewalks. Each part lives
// in its own .cpp; Simulation::tick calls step() once per tick holding map_mutex shared and traffic_mutex.
#pragma once

#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

namespace city {

constexpr float NO_LEADER = BIG; // gap meaning a free road

namespace traffic {

// The directed segment of `seg` arriving at / leaving `node` (even: a to b).
inline int in_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].b == node ? 0 : 1); }
inline int out_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].a == node ? 0 : 1); }

// traffic_system.cpp
void step(CityState &s, float dt); // applies the network changes, then advances everything near and far
// After each tick: poses near the camera into CityState::poses, stamped with the tick's scheduled time.
void publish_poses(CityState &s, int64_t stamp_ns);
float day_factor(const TrafficParams &p, float hour);
void despawn_vehicle(CityState &s, int id, bool arrived);
void reset_traffic(CityState &s); // full graph build and an empty fleet: a new map or a load

// lane_graph.cpp / lane_geometry.cpp
void build_lane_graph(CityState &s); // into s.traffic.g
void lane_pose(const CityState &s, int ds, int lane, float at, Pose &out);
void conn_pose(const Conn &c, float at, Pose &out);
void sidewalk_pose(const CityState &s, int seg, int side, float arc, float track_u, Pose &out);

// lane_list.cpp: the per-lane/connector lists with a near vehicle, and the lane queries.
void lane_add(CityState &s, int key, int id);
void lane_del(CityState &s, int key, int id);
void conn_push(CityState &s, int ci, int id);
void conn_pop_front(CityState &s, int ci);
void conn_del(CityState &s, int ci, int id);
void conn_clear(CityState &s, int ci);
void micro_add(TrafficState &t, int id);
void micro_remove(TrafficState &t, int id);
int lane_load(const TrafficState &t, int key); // near + far vehicles on the lane
bool lane_reaches(const CityState &s, int ds, int lane, int next);
int conn_toward(const CityState &s, int ds, int lane, int to_ds);
int lane_toward(const CityState &s, int ds, int to_ds, int near_lane);
void move_vehicle_lane(CityState &s, int id, int target);

// meso.cpp / window.cpp: the far model and the micro window around the camera.
void meso_clear(TrafficState &t);
void meso_link(CityState &s, int id, int key);
void meso_unlink(CityState &s, int id);
void meso_enter(CityState &s, int id, int key, bool spawned); // the only way into the far model (leaves lane + micro lists)
void meso_process(CityState &s);
bool in_window(const CityState &s, int ds);
void check_window(CityState &s);
void materialize(CityState &s, int key);

// junction_control.cpp
void build_junctions(CityState &s);
void build_junction(CityState &s, int n); // one node's control and stages (incremental rebuild)
// The node's control (signals, priority or none), from the road network alone: the only owner of that decision,
// read by build_junction and by the render's signal posts (which hold just the map lock, not the lane graph).
Control control_of(const CityState &s, int n);
void update_lights(CityState &s); // fills TrafficState::light_now once per step
Light light_for(const CityState &s, int ds); // the light the approach `ds` shows now
float amber_left(const CityState &s, int ds); // seconds of amber left on approach `ds`, which shows amber
double next_green(const CityState &s, int ds, double now); // when that approach next shows green (meso waits)
bool may_enter(const CityState &s, int vehicle, int conn, float dist_to_line);
int connector_to(const CityState &s, int ds, int lane, int next_ds); // from lane `lane` of `ds` onto `next_ds`, or -1
int pick_connector(const CityState &s, const Vehicle &v);
Hold hold_reason(const CityState &s, int vehicle);
int lane_head(const CityState &s, int vehicle); // the first vehicle of its lane (itself when it leads), -1 off lanes

// light_rule.cpp: the one rule of the lights, for admission, commitments and the stops ahead. `v` can stop
// comfortably (its class's b) within `stop_m`; how far it needs to stop as the intergreen is timed (CONTRAN MBST vol. V
// 5.7.1: reaction, then signal_decel); whether it stops at the amber of approach `ds`, `stop_m` ahead, with connector
// `ci` past the line (-1: none from its lane); the light of `ds` holds it (red always; amber by that rule, a decision
// held until green).
bool stop_is_comfortable(const CityState &s, const Vehicle &v, float stop_m);
float amber_reach(const TrafficParams &p, float v);
struct AmberApproach {
	float v = 0, stop_m = 0; // its speed and the metres to where it stops
	float top = 0; // the speed it may take the line at
	float brake = 0, brake_hard = 0; // m/s²: its comfortable and its emergency braking
	float left = 0; // seconds of amber left
};
bool amber_stops(const TrafficParams &p, const AmberApproach &a);
bool stops_at_amber(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci);
bool light_stops(const CityState &s, const Vehicle &v, int ds, float stop_m, int ci);
// Where a vehicle stops for the lights down its route: the first stop line within its horizon whose light holds it.
struct LightStop {
	float gap = NO_LEADER; // metres to where it stops
	bool here = false; // that stop is on the lane it is on: it does not cross its junction now
	int amber_hold = -1; // its amber decision from now on (Vehicle::amber_hold)
};
// The walk starts at route step `ri` on lane `lane`, whose start is `start` metres ahead (negative: it is on it); the
// last step's junction is never crossed.
LightStop lights_ahead(const CityState &s, const Vehicle &v, int ri, int lane, float start);

// junction_admission.cpp (anti-gridlock)
bool is_splice(const CityState &s, int node); // a road joint: no junction to block
bool has_exit(const CityState &s, const Conn &c, int vehicle); // room on the exit lane (exitRoom)
int pass_holder(const CityState &s, int conn); // vehicle committed to `conn` and not yet on it, or -1
bool box_clear(const CityState &s, int conn); // nobody inside (or committed earlier) on a crossing connector
void commit(CityState &s, int vehicle, int conn);
void release(CityState &s, int vehicle, int conn); // takes a commitment back before the vehicle used it
void claim_exit(CityState &s, const Conn &c, int vehicle);
bool jam_release(CityState &s, int conn, int vehicle);
void admit(CityState &s, int node, bool released);
void reset_admission(CityState &s);
void step_governor(CityState &s, float dt);

// routing.cpp: directed segments from `from_ds` to `goal_ds` (inclusive), edge times raised by the load (BPR);
// false when unreachable. `max_expanded` < 0 uses route_max_expanded; `banned_ds` is never entered.
bool find_route(const CityState &s, int from_ds, int goal_ds, std::vector<int> &out, int max_expanded = -1, int banned_ds = -1);
float junction_cost(const CityState &s, const Conn &c); // turn + signal time the router adds for a connector
// The route's next turn is not reachable from this lane: another lane of the segment, else a new route, else false.
bool retry_route(CityState &s, int id);
// Another connector of the vehicle's lane (not `blocked`; with room at its exit when `need_room`), the route
// re-planned from it, rejoining the old route a few segments on. False when there is none.
bool detour(CityState &s, int vehicle, int blocked, bool need_room);
// Stopped long in a full queue: a local route avoiding the full lane ahead, rejoining the old one (reconsiderRoute).
bool reconsider(CityState &s, int vehicle);

// demand.cpp
void spawn_vehicles(CityState &s, float dt);

// vehicle_motion.cpp
float idm_accel(const VehicleClass &c, float v, float v0, float gap, float vl);
// What a vehicle standing on a connector stands for (traffic_probe), from the very obstacles it drives against: the one
// ahead, walkers on its exit crosswalk, a light down the street, or none of them holds it back (it is moving off).
enum class BoxHold : uint8_t { LEADER = 0, WALKERS, LIGHT, STARTING, COUNT };
BoxHold box_hold(const CityState &s, int id);
float desired_speed(const CityState &s, const Vehicle &v, int ds);
void step_vehicles(CityState &s, float dt);

// lane_change.cpp
void change_lanes(CityState &s);

// pedestrians.cpp
void step_pedestrians(CityState &s, float dt);
void reset_pedestrians(CityState &s);
// A blow or a shot reached a walker (sim/commands/strike_people.cpp): gone for good, knocked to the ground for a while
// (it walks on after), or frightened into running for a while.
void remove_pedestrian(CityState &s, int id);
void down_pedestrian(CityState &s, int id, float seconds);
void frighten_pedestrian(CityState &s, int id, float seconds);
// ped_wounds.cpp: the share of its own pace a hurt walker keeps (1 unhurt).
float wounded_pace(const BlastParams &b, const Pedestrian &w);

// ped_crowd.cpp / ped_stripes.cpp: walkers among walkers and street furniture, on every way they walk
void crowd_step(CityState &s, float dt); // every walking walker's stripe and pace, from the bodies around it
float walk_half(const CityState &s, int seg, int side); // stripe centres lie within this of the sidewalk's middle
void on_sidewalk(Pedestrian &w); // onto a new sidewalk: its own side of the way, its own pace
void onto_line(Pedestrian &w); // onto a new line round a corner or over a zebra: from where it stands
void drop_pedestrians_on_seg(CityState &s, int seg); // a segment is going: its walkers leave the scene

// ped_kerb.cpp: places to wait at the kerb of a crosswalk
void take_kerb_place(CityState &s, int id); // heading for the kerb before a crosswalk: a place to wait there
void free_kerb_place(TrafficState &t, int id);
// A waiting walker with a free place nearer the kerb: turned back toward its kerb (ways[wi]) to take it; true.
bool step_to_nearer_place(CityState &s, int id);

// ped_furniture.cpp: the street furniture (sim/roads/street_furniture.h) as the walkers and the render meet it
// The pieces of segment `sid` of the kinds in `kinds`, the signal posts where control_of signals its ends.
void street_furniture_of(const CityState &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out);
// Where a piece on a sidewalk stands in the world; false for a piece with no sidewalk (it cannot be knocked out).
bool furniture_xz(const CityState &s, int seg, const FurnitureSpot &f, float &x, float &z);
const SidewalkFurniture &sidewalk_furniture(CityState &s, int seg); // its footprints as bodies, built once per edit
bool on_furniture(CityState &s, int seg, int side, float x, float z, float reach); // within `reach` of a footprint
// The first arc from `at` on where a walker `u` across the sidewalk keeps its body off every footprint on it.
float clear_arc(CityState &s, int seg, int side, float at, float u);
void forget_furniture(CityState &s, int seg); // an edit touched the segment or one of its nodes
void prune_furniture(CityState &s, float cx, float cz, float radius); // segments the walkers are far from
int walkers_in_furniture(const CityState &s); // walkers whose body stands in a footprint (traffic_probe)

// ped_routes.cpp
bool walkable(const CityState &s, int seg);
int seg_between(const CityState &s, int a, int b); // a walkable segment joining two nodes, or -1
bool find_walk_route(const CityState &s, int from, int goal, std::vector<int> &out); // nodes from..goal
// Waypoints round node `n` from the end of `seg_in` (sidewalk `side_in`) to the start of `seg_out`: kerb points of
// each crosswalk crossed (the far one carries the arm). Returns how many (-1: no way); `side_out` = exit sidewalk.
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, Waypoint *ways, int &side_out);

// ped_crossing.cpp
float amber_wanted(const TrafficParams &tp, float v); // the amber a regulated speed `v` asks (m/s), before amber_max_s
// The intergreen after the green of approach `ds` into node `n` (amber, all-red), `walkers` when it has crosswalks.
void intergreen(const CityState &s, int n, int ds, bool walkers, float &amber, float &all_red);
// Places the detection section of signalled approach `ds` and its green extension (MBST vol. V 7.3.2, optimum cut:
// table 8.1 by its lanes, at its speed) and returns the minimum green that clears the queue before it (7.3.1, Eq. 8.2).
float detection(CityState &s, int ds);
// A vehicle on approach `ds` moved from arc `from` to `to`: over the detection section it wants the green until it
// reaches the line. A far (meso) vehicle asks as it leaves at the line, one saturation headway on.
void signal_call(CityState &s, int ds, float from, float to);
void signal_call_far(CityState &s, int ds);
// Walkers at signalled node `node` may step onto the crosswalk over arm `arm_seg` (its traffic in `arm_stage`) now: while
// another stage runs and their flashing red and all-red still fit before it ends (CONTRAN MBST vol. V 5.7.2).
bool walk_signal(const CityState &s, int node, int arm_seg, int arm_stage);
// The flashing red a crosswalk over arm `arm_seg` needs: reaction and its length at the walkers' design speed.
float ped_clearance(const CityState &s, int arm_seg);
bool walk_ok(CityState &s, int node, int arm_seg); // a walker may step onto the crosswalk over this arm
bool cars_on_crossing(CityState &s, int node, int arm_seg); // a vehicle on it or too close to stop
// Walkers on the crosswalks connector `conn` drives over (the arm it comes from and the one it leaves by) who are
// not clear of its lane when its front gets there (`front_in`/`front_out` seconds from now).
bool peds_in_way(const CityState &s, int conn, float front_in = 0, float front_out = 0);
constexpr float FRONT_PAST = BIG; // peds_in_way front time of a crosswalk the vehicle has already passed

// Seconds for the front of `v` to cover `metres` accelerating at its class's rate from its speed up to `top`: a vehicle
// starting at its line reaches a crosswalk sooner than its speed now says. Measured on 155 000 commitments (soak, 7
// seeds): to the exit crosswalk it errs 0.2 s early on average, where metres / max(2 m/s, v) was 0.9 s late and let
// vehicles in that then stood in the box for walkers (4x the box stops, 6x the hard brakes for walkers, the same for
// walkers). What it leaves out (the jerk ramp from rest, slowing to the turn speed) only makes it early, the safe side.
float front_time(const CityState &s, const Vehicle &v, float metres, float top);
float conn_speed(const CityState &s, const Conn &c); // driven over a connector: the exit's limit straight on, else the turn speed
// The fastest `v` goes `dist` before its lane's end on the way onto `c`: braking down to the turn speed (no limit straight on).
float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist);

} // namespace traffic
} // namespace city
