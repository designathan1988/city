// The pedestrians' part of the traffic (Roadcraft src/sim/peds.ts): who walks (ped_demand), where from (ped_doors, ped_spawn,
// ped_party), how they move among each other and the street furniture (ped_crowd, ped_kerb, ped_furniture) and by which ways
// (ped_routes). traffic_system.h includes this; the crosswalk and signal rules the walkers meet are declared there.
#pragma once

#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

namespace city {
namespace traffic {

// pedestrians.cpp: the walk's own steps, shared with the door walk
inline float arc_lo(const RoadSegment &g) { return float(g.retreat_a); } // the sidewalk's first and last arc
inline float arc_hi(const RoadSegment &g) { return float(g.len - g.retreat_b); }
void aim_line(Pedestrian &w); // a straight line toward ways[wi] from where the walker stands
void onto_sidewalk(CityState &s, Pedestrian &w); // onto the sidewalk at the same place across as where it stands
void despawn(CityState &s, int id, bool arrived); // retires only this pedestrian namespace before recycling

// ped_door_walk.cpp: the first and last steps at a lot's door
void aim_at_door(CityState &s, Pedestrian &w); // on the last street of its route: a door ahead to go in at
void go_in(CityState &s, int id); // at its door's place on the sidewalk: across to the foot of the way in, then the door
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
bool door_current(const CityState &s, const PedDoor &door); // bounded canonical source witness
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
void commit_walker(TrafficState &t, int id, Pedestrian &&walker); // identity at actual slot installation, not a rejected birth proposal
void spawn_walkers(CityState &s, const PeopleNow &now, bool from_door, int attempts); // captures one batch; all attempts join common route admission

// ped_party.cpp
void add_company(CityState &s, int leader); // somebody at the leader's side
void follow_leaders(CityState &s, float dt); // after the leaders moved
void step_act(const TrafficState &t, const TrafficParams &p, Pedestrian &w, float dt); // what it does while it stands

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
// Waypoints round node `n` from the end of `seg_in` (sidewalk `side_in`) to the start of `seg_out`: kerb points of
// each crosswalk crossed (the far one carries the arm). Returns how many (-1: no way); `side_out` = exit sidewalk, the one
// with fewest crossings or `want_side` when it has that side (-1: either).
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, int want_side, Waypoint *ways, int &side_out);

} // namespace traffic
} // namespace city
