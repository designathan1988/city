// Traffic behaviour on top of CityState (Roadcraft src/sim/**): lane graph, demand and spawning, A* routing, IDM car
// following, MOBIL lane changes, junction control with timed signals, pedestrians on the sidewalks. Each part lives
// in its own .cpp; Simulation::tick calls step() once per tick holding map_mutex shared and traffic_mutex.
#pragma once

#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"
#include "sim/traffic/ped_system.h"
#include "sim/traffic/records_changed.h"
#include "sim/street/street_spots.h"

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
// vehicle_pose.cpp: where a vehicle on the lanes is drawn (its middle, heading and pitch into `out`), from its path.
void vehicle_pose(const CityState &s, const Vehicle &v, Pose &out);
// The indicator a vehicle on the lanes shows (pose_buffer.h SIGNAL_*): the side it slides or means to change to, the turn it is in
// or about to make.
int signal_side(const CityState &s, const Vehicle &v);
float day_factor(const TrafficParams &p, float hour);
// Gone from the scene: its trip `arrived` or lost; not `counted` as either when it only drove out of the window.
void despawn_vehicle(CityState &s, int id, bool arrived, bool counted = true);
int new_vehicle(CityState &s, VehicleType type, uint8_t paint); // of that type, on no lane yet, counted in the fleet
void reset_traffic(CityState &s); // full graph build and an empty fleet: a new map or a load

// lane_graph.cpp / lane_geometry.cpp
void build_lane_graph(CityState &s); // into s.traffic.g
void lane_pose(const CityState &s, int ds, int lane, float at, Pose &out);
// links_built, and what the parking lets go of a changed street or lot: sim/traffic/records_changed.h.
void conn_pose(const Conn &c, float at, Pose &out);
void sidewalk_pose(const CityState &s, int seg, int side, float arc, float track_u, Pose &out);

// parking_sources.cpp: the bays near the camera and the scenery cars in them (parking_state.h)
void step_parking_sources(CityState &s);
void refile_bay(CityState &s, int b, bool away); // the bay on the free or scenery list its state says; `away`: on none
void remove_row(ParkingState &p, int b); // the scenery car of bay `b` is gone
bool read_bay_way(CityState &s, int b); // the bay's way out through its gate and the lane it joins; false: it has none
// The lots of every building with a dock read now, wherever the camera is (the yard planner's bench, CityProbes::plan_all_yards):
// the game reads a lot when the view comes near it.
void read_lots_with_docks(CityState &s);
void add_row(ParkingState &p, int b); // a car stands in bay `b` now
void publish_parked(CityState &s, PoseFrame &f); // the standing cars, made again only when a row changed

// parking_lanes.cpp
void seg_point(const CityState &s, int seg, float arc, float u, Pose &out); // the point (arc, u) of a segment, its a -> b heading
// The lane a lot's gate opens onto, on the lot's kerb; `reads` (a yard's) gets the box and segments looked at.
bool gate_lane(const CityState &s, const V2 &gate, LaneSpot &out, YardReads *reads = nullptr);

// lot_traffic.cpp
void step_lots(CityState &s, float dt);
bool lot_car_at_rest(const CityState &s, int id); // standing in its bay and not sent out
void lot_car_gone(CityState &s, int id); // its bay went: off the road it goes too, on the road it drives on
void lot_car_forget(CityState &s, int id); // the vehicle left the scene: its bay is free
void settle_parked(CityState &s); // the cars that parked become rows of their bays, their Vehicles let go
void set_off_pose(CityState &s, Vehicle &v, const OffPose &q); // where a vehicle off the lanes stands
// The car moves to phase `next` (enter_car_phase); on a move the lots' counts of cars coming and moving follow it and the phase's clocks
// start again. False, nothing changed, for a move its visit never makes.
bool lot_car_moves(ParkingState &p, LotCar &car, CarPhase next);

// car_phase.cpp: a lot car's one setter (CAR_PHASES) and its one maker (CAR_BIRTHS: called, or out of the row it stood in as).
bool enter_car_phase(LotCar &car, CarPhase next);
LotCar new_lot_car(int id, int bay, CarPhase born);

// lot_demand.cpp
void lot_depart(CityState &s, double target); // cars sent out of the bays while fewer drive than the demand asks
bool lot_call(CityState &s); // a trip ending in a lot: a free bay drawn, the nearest passing car sent to it

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
double window_lane_metres(const CityState &s); // the lane metres of the directed segments whose middle is in the window
void check_window(CityState &s);
void materialize(CityState &s, int key);

// junction_control.cpp
void build_junctions(CityState &s);
void build_junction(CityState &s, int n); // one node's control and stages (incremental rebuild)
// signal_timing.cpp: node `n` on LaneGraph::signal_nodes when its junction runs a signal plan, off it otherwise; called after every
// write of a junction.
void file_signal(LaneGraph &g, int n);
void replan_signal(CityState &s, int n); // its signal settings changed: the plan again, the stage running ending as it began
// The fixed plans of a green wave along the main road through node `n` (green_wave.cpp): empty with under two signals on it.
std::vector<std::pair<int, SignalSettings>> green_wave_plans(const CityState &s, int n);
// The node's control (signals, priority or none), from the road network alone: the only owner of that decision,
// read by build_junction and by the render's signal posts (which hold just the map lock, not the lane graph).
Control control_of(const CityState &s, int n);
std::vector<int> main_legs(const CityState &s, int n); // the two legs a junction's main road comes and goes by
// The plate the player put on arm `seg` of node `n` (SegRule), and the rule its traffic meets there, by plate or by rank:
// both from the road network alone, like control_of (the render's plates read them under the map lock only).
int plate_rule(const RoadNetwork &net, int seg, int n);
int leg_rule(const RoadNode &node);
int approach_rule(const CityState &s, int n, int seg);
// A plate a rule stands on an arm (street_signs.cpp): the arc and the sidewalk its rule wants it at, the node whose plate it is,
// what it is and says, and which way along the segment its face looks (+1: as a runs to b).
struct RulePlate {
	double v = 0;
	int side = 0, node = -1;
	SignType type = SignType::STOP;
	std::string text;
	float dir = 1;
};
// The plates the rules stand on arm `seg` of node `n`, and on a segment at both its nodes (a's, then b's): which, and where each
// rule wants its plate. Where each stands is the segment's placement (street_furniture.h PlateWanted).
void arm_plates(const CityState &s, int n, int seg, std::vector<RulePlate> &out);
void segment_plates(const CityState &s, int seg, std::vector<RulePlate> &out);
// The plates the rules put up at node `n`, each where its post stands in its arm's placement.
void junction_signs(const CityState &s, int n, std::vector<SignStand> &out);
void update_lights(CityState &s); // fills TrafficState::light_now once per step
Light light_for(const CityState &s, int ds); // the light the approach `ds` shows now
float amber_left(const CityState &s, int ds); // seconds of amber left on approach `ds`, which shows amber
double next_green(const CityState &s, int ds, double now); // when that approach next shows green (meso waits)
bool may_enter(const CityState &s, int vehicle, int conn, float dist_to_line);
// From lane `lane` of `ds` onto `next_ds` (from the nearest lane that has one), one a body of class `cls` fits, or -1.
int connector_to(const CityState &s, int ds, int lane, int next_ds, int cls = 0);
int pick_connector(const CityState &s, const Vehicle &v);
Hold hold_reason(const CityState &s, int vehicle);
int lane_head(const CityState &s, int vehicle); // the first vehicle of its lane (itself when it leads), -1 off lanes
// The lights' rule for vehicles has its own header: sim/traffic/light_rule.h.

// junction_sweeps.cpp: the body class (BODY_CLASSES) a vehicle type is swept as at the junctions, the smallest envelope holding it.
int body_class(const TrafficParams &p, VehicleType type);

// junction_admission.cpp (anti-gridlock)
bool is_splice(const CityState &s, int node); // a road joint: no junction to block
bool has_exit(const CityState &s, const Conn &c, int vehicle); // room on the exit lane (exitRoom)
int pass_holder(const CityState &s, int conn); // vehicle committed to `conn` and not yet on it, or -1
// Whether a body on conflicting connector `k` of `c` (on it, or come off it with its body still reaching back) is not yet clear of
// a body of class `mine` on `c`: by the sweeps' clear arcs (Conn::conflict_clear).
bool conflict_busy(const CityState &s, const Conn &c, size_t k, int mine);
// No body in the way of this vehicle's: on a crossing connector, on another branch of its lane's fork short of where the ways part
// (fork_bodies.h), nor committed earlier to a crossing one. A commitment is granted only on it.
bool box_clear(const CityState &s, int conn, int vehicle);
void commit(CityState &s, int vehicle, int conn);
void release(CityState &s, int vehicle, int conn); // takes a commitment back before the vehicle used it
void claim_exit(CityState &s, const Conn &c, int vehicle);
bool jam_release(CityState &s, int conn, int vehicle);
void admit(CityState &s, int node, bool released);
void reset_admission(CityState &s);
void step_governor(CityState &s, float dt);

// route_admission.cpp: the tick's route searches. A search that can wait asks first and starts only on true; every search, of
// either kind, is counted (find_route counts its own).
bool may_request_search(CityState &s);
void note_search(CityState &s);

// routing.cpp: directed segments from `from_ds` to `goal_ds` (inclusive) for a vehicle of `type`, edge times raised by the load
// (BPR): a bicycle only where bicycles ride, and only through the turns its body fits (Conn::max_class); false when unreachable.
// `max_expanded` < 0 uses route_max_expanded; `banned_ds` is never entered. Counted on the tick's searches.
bool find_route(CityState &s, int from_ds, int goal_ds, std::vector<int> &out, VehicleType type, int max_expanded = -1, int banned_ds = -1);
float junction_cost(const CityState &s, const Conn &c); // turn + signal time the router adds for a connector
// The route's next turn is not reachable from this lane: another lane of the segment, else a new route, else false.
bool retry_route(CityState &s, int id);
// Another connector of the vehicle's lane (not `blocked`; with room at its exit when `need_room`), the route
// re-planned from it, rejoining the old route a few segments on. False when there is none.
bool detour(CityState &s, int vehicle, int blocked, bool need_room);
// Stopped long in a full queue: a local route avoiding the full lane ahead, rejoining the old one (reconsiderRoute).
bool reconsider(CityState &s, int vehicle);

// demand.cpp
double fleet_target(const CityState &s); // how many vehicles the demand asks for now
double demand_for(const CityState &s, double lane_metres); // the vehicles these lane metres carry at this hour and these levels
void free_vehicle(TrafficState &t, int id); // the slot of a vehicle off the lanes, out of the fleet count
bool new_trip(CityState &s, int id, int from_ds); // a route from `from_ds` to a destination the demand draws
// A vehicle of `type` put on lane `lane` of `ds`, its nose at arc `at`, going `speed`, on the route `route` (directed segments, the
// first `ds`): a trip given, not drawn (a test's scene). -1 where the lane has no room for its body there.
int vehicle_on_lane(CityState &s, VehicleType type, int ds, int lane, float at, float speed, const std::vector<int> &route);

// vehicle_motion.cpp
float idm_accel(const VehicleClass &c, float v, float v0, float gap, float vl);
// The following law that anticipates, the bends a vehicle slows for and its braking curve: sim/traffic/motion_law.h.
// What a vehicle standing on a connector stands for (traffic_probe), from the very obstacles it drives against: the one
// ahead, walkers on its exit crosswalk, a light down the street, or none of them holds it back (it is moving off).
enum class BoxHold : uint8_t { LEADER = 0, WALKERS, LIGHT, STARTING, COUNT };
BoxHold box_hold(const CityState &s, int id);
float desired_speed(const CityState &s, const Vehicle &v, int ds);
void step_vehicles(CityState &s, float dt);

// cycling.cpp
float cycle_shift(const CityState &s, const Vehicle &v, int ds, float front); // how far across a bicycle's body rides at `front`
bool out_of_the_way(const CityState &s, const Vehicle &follower, const Vehicle &lead, int ds); // `lead` rides the band beside `follower`
float top_speed(const CityState &s, const Vehicle &v, float limit); // the most `v` goes where the limit is `limit`

// lane_change.cpp
void change_lanes(CityState &s);
// Lane `target` of its segment has room for the vehicle's body beside it now (the physical gaps of a forced change).
bool lane_room(const CityState &s, int id, int target);

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

} // namespace traffic
} // namespace city
