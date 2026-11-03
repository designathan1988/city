// Traffic behaviour on top of CityState (Roadcraft src/sim/**): lane graph, demand and spawning, A* routing, IDM car
// following, MOBIL lane changes, junction control with timed signals, pedestrians on the sidewalks. Each part lives
// in its own .cpp; Simulation::tick calls step() once per tick holding map_mutex shared and traffic_mutex.
#pragma once

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
double next_green(const CityState &s, int ds, double now); // when that approach next shows green (meso waits)
bool may_enter(const CityState &s, int vehicle, int conn, float dist_to_line);
int pick_connector(const CityState &s, const Vehicle &v);
Hold hold_reason(const CityState &s, int vehicle);
int lane_head(const CityState &s, int vehicle); // the first vehicle of its lane (itself when it leads), -1 off lanes

// junction_admission.cpp (anti-gridlock)
bool is_splice(const CityState &s, int node); // a road joint: no junction to block
bool has_exit(const CityState &s, const Conn &c, int vehicle); // room on the exit lane (exitRoom)
int pass_holder(const CityState &s, int conn); // vehicle committed to `conn` and not yet on it, or -1
bool box_clear(const CityState &s, int conn); // nobody inside (or committed earlier) on a crossing connector
void commit(CityState &s, int vehicle, int conn);
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
float desired_speed(const CityState &s, const Vehicle &v, int ds);
void step_vehicles(CityState &s, float dt);

// lane_change.cpp
void change_lanes(CityState &s);

// pedestrians.cpp
void step_pedestrians(CityState &s, float dt);
void reset_pedestrians(CityState &s);
void drop_pedestrians_on_seg(CityState &s, int seg); // a segment is going: its walkers leave the scene

// ped_routes.cpp
bool walkable(const CityState &s, int seg);
int seg_between(const CityState &s, int a, int b); // a walkable segment joining two nodes, or -1
bool find_walk_route(const CityState &s, int from, int goal, std::vector<int> &out); // nodes from..goal
// Waypoints round node `n` from the end of `seg_in` (sidewalk `side_in`) to the start of `seg_out`: kerb points of
// each crosswalk crossed (the far one carries the arm). Returns how many (-1: no way); `side_out` = exit sidewalk.
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, Waypoint *ways, int &side_out);

// ped_crossing.cpp
int green_stage(const CityState &s, int node); // the stage on green now, -1 on amber/all-red or no signal
bool walk_ok(CityState &s, int node, int arm_seg); // a walker may step onto the crosswalk over this arm
bool cars_on_crossing(CityState &s, int node, int arm_seg); // a vehicle on it or too close to stop
// Walkers on the crosswalks connector `conn` drives over (the arm it comes from and the one it leaves by) who are
// not clear of its lane when its front gets there (`front_in`/`front_out` seconds from now).
bool peds_in_way(const CityState &s, int conn, float front_in = 0, float front_out = 0);
float front_time(const Vehicle &v, float metres); // seconds for the front to cover `metres` (at least walking speed)

} // namespace traffic
} // namespace city
