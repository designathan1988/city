// The pedestrians' part of the traffic (Roadcraft src/sim/peds.ts): who walks (ped_demand), where from (ped_doors, ped_spawn,
// ped_party), how they move among each other and the street furniture (ped_crowd, ped_kerb, ped_furniture) and by which ways
// (ped_routes). traffic_system.h includes this; the crosswalk and signal rules the walkers meet are declared there.
#pragma once

#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

#include <limits>

namespace city {
namespace traffic {

// Two walkers stand in one another when their centres are nearer than two bodies by more than the rounding of their places: a place is
// float metres, and FARTHEST_PLACE_M from the origin (past the edge of the largest map) two of them read their distance off by up to two
// parts in 2^24 of that (a float's epsilon of it): PLACE_ROUNDING_M. Nearer than two bodies by less, they stand side by side, touching
// (a jam's squeeze closes to it).
constexpr float FARTHEST_PLACE_M = 16384.f;
constexpr float PLACE_ROUNDING_M = std::numeric_limits<float>::epsilon() * FARTHEST_PLACE_M;
inline float inside_another_m(const TrafficParams &p) { return 2 * p.ped_body_m - PLACE_ROUNDING_M; }

// pedestrians.cpp: the walk's own steps, shared with the door walk
inline float arc_lo(const RoadSegment &g) { return float(g.retreat_a); } // the sidewalk's first and last arc
inline float arc_hi(const RoadSegment &g) { return float(g.len - g.retreat_b); }
void aim_line(Pedestrian &w); // a straight line toward ways[wi] from where the walker stands
// Its arc (the street curve's nearest point to its body) and its place across (from the sidewalk's middle there), from where it stands.
void where_on_sidewalk(const CityState &s, Pedestrian &w);
void onto_sidewalk(const CityState &s, Pedestrian &w); // onto the sidewalk at the same place across as where it stands
void turn_round(Pedestrian &w); // it walks the other way along its sidewalk, on its own side of the way
void stand_on_sidewalk(const CityState &s, Pedestrian &w); // its pose at (seg, side, s, u), body and way heading the way it walks
void despawn(TrafficState &t, int id, bool arrived);
void despawn_party(CityState &s, int id, bool arrived); // with the one it walks with, when that one is within reach

// ped_ways.cpp: the ways off the sidewalk
void at_node(CityState &s, int id); // at the end of a sidewalk: arrived, or the way round the node onto the next street's sidewalk
void end_line(CityState &s, int id); // off the end of a line (its place across kept): onto the next line, or onto the sidewalk
void begin_line(CityState &s, int id); // the next straight line toward ways[wi] from where it stands; a crosswalk waits for its leave
void off_crosswalk(TrafficState &t, int id); // off the crosswalk it was on (TrafficState::node_crossers)

// ped_door_walk.cpp: the first and last steps at a lot's door
void aim_at_door(CityState &s, Pedestrian &w); // on the last street of its route: a door ahead to go in at
void go_in(CityState &s, int id); // at its door's place on the sidewalk: across to the foot of the way in, its bends, then the door
// Standing at door `d` (w.seg and w.side its sidewalk): its way out, through the way's bends to the foot on the sidewalk, the first line begun.
void door_way_out(const CityState &s, Pedestrian &w, const PedDoor &d);
// From the door it came out of (out_x, out_y, out_z), onto the first line of its way out (ways[0..wn)).
void leave_door(Pedestrian &w);
void walk_lot(CityState &s, int id, float dt); // between a door and the sidewalk

// pedestrians.cpp
void step_pedestrians(CityState &s, float dt);
void reset_pedestrians(CityState &s);

// A blow or a shot reached a walker (sim/commands/strike_people.cpp): gone for good, knocked to the ground for a while
// (it walks on after), or frightened into running for a while.
void remove_pedestrian(CityState &s, int id);
void down_pedestrian(CityState &s, int id, float seconds);
// ped_fright.cpp: a bang at (x, z) heard: a shot, or a blast's stampede. Frightened for about `seconds`; what it does first, and then away.
void frighten_pedestrian(CityState &s, int id, float seconds, float x, float z, bool stampede);
void startle_pedestrians(CityState &s, float x, float z, float reach, float seconds); // a shot heard by those within `reach`
void add_shock(CityState &s, float x, float z, float reach); // something those who come near run from a good while after: a body, a blast's ruin
void look_at_shocks(CityState &s); // who has come near one
void mourn_pedestrian(CityState &s, int id); // shot dead: those walking with it go down beside it a while
bool held_by_fright(const TrafficState &t, Pedestrian &w); // stopped by what it heard this step
void turn_from_bang(CityState &s, Pedestrian &w); // done with what it heard: if it was heading for the bang, back the other way
void flee_on(CityState &s, Pedestrian &w, int n); // frightened, at node n: by the street that leads furthest off
void ease_facing(const TrafficState &t, const TrafficParams &p, Pedestrian &w, float dt); // round to the bang, and back to its way, no faster than a body turns
// ped_wounds.cpp: the share of its own pace a hurt walker keeps (1 unhurt).
float wounded_pace(const BlastParams &b, const Pedestrian &w);

// ped_demand.cpp: how many people the land use puts on its street now, by kind of place
struct PeopleNow {
	float per_m[PLACE_KINDS] = {}; // people per metre of street
	float out[PLACE_KINDS] = {}; // of those at a place's door, the share coming out
	float peak = 0;
};
PeopleNow people_now(const TrafficParams &p, float hour);
double people_wanted(const CityState &s, const PeopleNow &now); // how many the walking window has on foot now
float zone_people(const TrafficParams &p, const PeopleNow &now, int zone); // people per metre beside a base zone

// ped_doors.cpp: the front doors of the walking window
void list_doors(CityState &s, float cx, float cz, float reach); // the lots not read yet round the camera
void read_doors(CityState &s); // a few of them a step
void forget_doors(CityState &s, int seg); // an edit touched the segment: its lots are read again
int door_sidewalk(CityState &s); // a sidewalk with doors, any of them (seg * 2 + side), or -1
// A door on the sidewalk between arcs `from` and `to`, drawn by those coming out of (`out`) or going in at each now.
bool pick_door(CityState &s, const PeopleNow &now, int seg, int side, bool out, float from, float to, PedDoor &door);

// ped_spawn.cpp
PedAge roll_age(TrafficState &t, const TrafficParams &p);
PedAge roll_age_alone(TrafficState &t, const TrafficParams &p);
void dress_walker(TrafficState &t, const TrafficParams &p, Pedestrian &w, PedAge age, float hp); // a body of that age and its own pace
int alloc_walker(TrafficState &t);
bool spawn_walker(CityState &s, const PeopleNow &now, bool from_door);

// walker_cells.cpp: who stands near a point (TrafficState::walker_cells, spawn_cells)
bool walker_within(CityState &s, float x, float z, float r); // somebody on foot within `r` of the point, as filed this tick
// The births of a step: the walkers filed where they stand (nobody moves until the births are over, then `spawn_cells.live` goes off),
// each one born filed too, and whether anybody stands within `r` of `at`; with no such filing every walker is looked at.
void file_spawn_cells(TrafficState &t);
void walker_born(TrafficState &t, int id);
bool walker_near(const TrafficState &t, const Pose &at, float r);

// ped_party.cpp
// Somebody born with the leader's trip, beside it on its sidewalk or out of its door after it; false with no room there.
bool add_company(CityState &s, int leader);
// The share of its own pace a walker goes at to stay level with the one it walks with (1 alone or far from it).
float company_pace(const TrafficState &t, const TrafficParams &p, const Pedestrian &w);
// About to go over the crosswalk it waits for (ways[wi]): whether the one it walks with is ready too (at that kerb, or in its queue with
// nobody between the two; or over already). A party starts over together.
bool company_ready(const TrafficState &t, const TrafficParams &p, const Pedestrian &w);
// The one it walks with is on the crosswalk over `arm` at its node, however far along: it goes over with it, on the pair's leave.
bool crossing_with(const TrafficState &t, const Pedestrian &w, int arm);
// Just stepped onto a crosswalk: the one it walks with that steps off with it on the same leave (waiting at the same kerb, or in the
// queue right behind it with nobody between), or -1.
int company_steps_off(const TrafficState &t, const TrafficParams &p, const Pedestrian &w);
int company_near(const TrafficState &t, const TrafficParams &p, const Pedestrian &w); // the one it walks with, when within reach; else -1
uint32_t party_look(const TrafficState &t, const Pedestrian &w); // the hash the party's choices are drawn by: its first walker's
bool company_door(const TrafficState &t, Pedestrian &w); // the door the one it walks with goes in at, ahead on its sidewalk: its own too
void step_act(const TrafficState &t, const TrafficParams &p, Pedestrian &w, float dt); // what it does while it stands

// ped_swaths.cpp: the swaths of the vehicles off the lanes (swaths.h), solid to the walkers
bool on_a_swath(const TrafficState &t, float x, float z, float body); // a body of that radius there would stand on one
void step_off_swath(const CityState &s, Pedestrian &w, float dt); // standing on one: a step away, at its pace aside

// ped_crowd.cpp / ped_stripes.cpp: walkers among walkers and street furniture, on every way they walk
void crowd_step(CityState &s, float dt); // every walking walker's stripe and pace, from the bodies around it
float walk_half(const CityState &s, int seg, int side); // stripe centres lie within this of the sidewalk's middle
void on_sidewalk(Pedestrian &w); // onto a new sidewalk: its own side of the way, its own pace
void onto_line(Pedestrian &w); // onto a new line round a corner or over a zebra: from where it stands
void drop_pedestrians_on_seg(CityState &s, int seg); // a segment is going: its walkers leave the scene

// ped_kerb.cpp: waiting at the kerb of a crosswalk
// Where those about to cross wait along the kerb, right of the zebra's middle line: the span's middle and its room either side.
struct KerbSpan {
	float middle = 0, room = 0;
};
KerbSpan kerb_span(const CityState &s);
bool toward_kerb(const Pedestrian &w); // on the queue: its line ends at the kerb of a crosswalk it is to cross next
// A kerb point and its far point were just written at ways[n - 2] and ways[n - 1]: both are moved to the span's middle and the
// approach point, back from the kerb, is put before them. Returns the new count.
int queue_before_kerb(const CityState &s, Waypoint *ways, int n);
bool toward_queue(const Pedestrian &w); // its line ends at the approach point of a kerb's queue
// Starting for a queue: the approach point and the crossing's points are moved along the kerb to where the walker comes from,
// inside the waiting span; line_off is that place left of the zebra's middle line.
void onto_approach(const CityState &s, Pedestrian &w);
void onto_queue(const CityState &s, Pedestrian &w); // starting the queue: its leg runs along the way across from where it stands
void cross_from_here(Pedestrian &w); // about to cross to ways[wi]: on a line parallel to the zebra's from where it stands

// ped_furniture.cpp: the street furniture (sim/roads/street_furniture.h) as the walkers and the render meet it
// The pieces of segment `sid` of the kinds in `kinds`, the signal posts where control_of signals its ends, appended to `out`.
void street_furniture_of(const CityState &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out, FurnitureRead read = FurnitureRead::KEPT);
// Where a piece on a sidewalk stands in the world; false for a piece with no sidewalk (it cannot be knocked out).
bool furniture_xz(const CityState &s, int seg, const FurnitureSpot &f, float &x, float &z);
const SidewalkFurniture &sidewalk_furniture(CityState &s, int seg); // its footprints as bodies, built once per edit
bool on_furniture(CityState &s, int seg, int side, float x, float z, float reach); // within `reach` of a footprint
// The straight way from `from` to `to` keeps `reach` off every footprint on that sidewalk (the segment against each one, exactly).
bool way_clear(CityState &s, int seg, int side, const V2 &from, const V2 &to, float reach);
// The place on that sidewalk nearest (arc, u) where a body stands clear of the furniture and its straight way from `from` (and, with
// `also`, from that point too) passes clear, written back; `along`: it may move along the sidewalk as well as across it. False when there
// is none.
bool clear_foot(CityState &s, int seg, int side, float &arc, float &u, const V2 &from, bool along, const V2 *also = nullptr);
// The first arc from `at` on where a walker `u` across the sidewalk keeps its body off every footprint on it.
float clear_arc(CityState &s, int seg, int side, float at, float u);
void forget_furniture(CityState &s, int seg); // an edit touched the segment or one of its nodes
void prune_furniture(CityState &s, float cx, float cz, float radius); // segments the walkers are far from
// Walkers whose body stands in a footprint (traffic_probe), and who with the piece.
int walkers_in_furniture(const CityState &s, std::vector<std::pair<int, StreetObstacle>> *who = nullptr);
// The segments whose furniture the walkers keep (TrafficState::furniture) is not what the owner places there now: a cache an edit left
// unforgotten (the probes').
int stale_furniture(const CityState &s);
// The pieces of the whole map standing in front of a lot's gate, a car's or a person's (traffic_probe): the signal posts alone
// with `signals` (their place is the signals' rule), every other kind without.
int furniture_in_gates(const CityState &s, bool signals);

// ped_routes.cpp
bool walkable(const CityState &s, int seg);
int seg_between(const CityState &s, int a, int b); // a walkable segment joining two nodes, or -1
bool find_walk_route(const CityState &s, int from, int goal, std::vector<int> &out); // nodes from..goal
// Waypoints round node `n` from the end of `seg_in` (sidewalk `side_in`) to the start of `seg_out`: kerb points of
// each crosswalk crossed (the far one carries the arm). Returns how many (-1: no way); `side_out` = exit sidewalk, the one
// with fewest crossings or `want_side` when it has that side (-1: either).
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, int want_side, Waypoint *ways, int &side_out);
// The last point of a way round a node, the next sidewalk's start (w.seg, w.side, w.s): put at the place across that sidewalk
// where the walker stands now, inside its room. A walker keeps its place across from one stretch of its way to the next (the
// stripes move it to its own side once it is on the sidewalk), so no two lines onto a sidewalk cross. Straight off its sidewalk
// (wi 0: no crosswalk), the way round the corner of w.way_node comes before it, from where it stands heading (way_x, way_z).
void aim_onto_sidewalk(const CityState &s, Pedestrian &w);

} // namespace traffic
} // namespace city
