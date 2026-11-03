// Traffic behaviour on top of CityState (Roadcraft src/sim/**): lane graph, demand and spawning, A* routing, IDM car
// following, MOBIL lane changes, junction control with timed signals, pedestrians on the sidewalks. Each part lives
// in its own .cpp; Simulation::step calls step() once per tick under world_mutex.
#pragma once

#include "sim/state/city_state.h"

namespace city {

constexpr float NO_LEADER = BIG; // gap meaning a free road

namespace traffic {

// traffic_system.cpp
void step(CityState &s, float dt); // rebuilds the lane graph when the roads changed, then advances everything
void publish_poses(CityState &s); // after each tick: poses near the camera into CityState::poses
float day_factor(const TrafficParams &p, float hour);
void despawn_vehicle(CityState &s, int id, bool arrived);

// lane_graph.cpp / lane_geometry.cpp
void build_lane_graph(CityState &s); // into s.traffic.g
void lane_pose(const CityState &s, int ds, int lane, float at, Pose &out);
void conn_pose(const Conn &c, float at, Pose &out);
void sidewalk_pose(const CityState &s, int seg, int side, float arc, float track_u, Pose &out);

// junction_control.cpp
void build_junctions(CityState &s);
void update_lights(CityState &s); // fills TrafficState::light_now once per step
Light light_for(const CityState &s, int ds); // the light the approach `ds` shows now
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

// ped_routes.cpp
bool walkable(const CityState &s, int seg);
int seg_between(const CityState &s, int a, int b); // a walkable segment joining two nodes, or -1
bool find_walk_route(const CityState &s, int from, int goal, std::vector<int> &out); // nodes from..goal
// Waypoints round node `n` from the end of `seg_in` (sidewalk `side_in`) to the start of `seg_out`: kerb points of
// each crosswalk crossed (the far one carries the arm). Returns how many (-1: no way); `side_out` = exit sidewalk.
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, Waypoint *ways, int &side_out);

// ped_crossing.cpp
int green_stage(const CityState &s, int node); // the stage on green now, -1 on amber/all-red or no signal
bool walk_ok(const CityState &s, int node, int arm_seg); // a walker may step onto the crosswalk over this arm
bool cars_on_crossing(const CityState &s, int node, int arm_seg); // a vehicle on it or too close to stop
// Walkers on the crosswalks connector `conn` drives over (the arm it comes from and the one it leaves by) who are
// not clear of its lane when its front gets there (`front_in`/`front_out` seconds from now).
bool peds_in_way(const CityState &s, int conn, float front_in = 0, float front_out = 0);
float front_time(const Vehicle &v, float metres); // seconds for the front to cover `metres` (at least walking speed)

} // namespace traffic
} // namespace city
