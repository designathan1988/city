// Errands at the kerb (kerb_stops.cpp): a vehicle's trip draws one at birth, pulls up for it on the kerb lane, and the person moves
// between the door and the footway; the IDM holds it at its stop.
#pragma once

#include "sim/traffic/bus_call.h"
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

void assign_errand(const CityState &s, Vehicle &v, int id); // at its birth: what this trip does at the kerb
void step_kerb_stops(CityState &s, float dt);
float kerb_stop_gap(const TrafficState &t, const Vehicle &v); // metres to where its front stops
uint32_t kerb_door_level(const CityState &s, const Vehicle &v); // how far its kerb door stands open, in LOOK_DOOR_MASK steps
void kerb_forget(CityState &s, int id); // it left the scene
// The one setter of KerbStop::phase (kerb_phase.cpp KERB_PHASES, the errand's order): false, the phase kept, for a move it never makes.
bool enter_kerb_phase(KerbStop &k, KerbPhase next);

// What a stop does with a person, shared with the bus's calls (bus_calls.cpp).
struct KerbDoors {
	V2 door, foot; // at the vehicle's side, and on the footway across from it
	float arc = 0; // the foot's arc along its street
};
KerbDoors kerb_door_at(const CityState &s, const Vehicle &v, float back_m); // this far behind its front
float kerb_arc_of(const DirSeg &d, float at); // the arc along its street of metre `at` of a directed segment
inline int kerb_side(const DirSeg &d) { return d.sign > 0 ? 0 : 1; } // the sidewalk on its kerb side (0 right, 1 left of a -> b)
// The first walker on the kerb's footway between two arcs who may be called (alone, no child, not called already), or -1; with `on_foot`,
// how many stand there at all.
int kerb_hailable(CityState &s, const DirSeg &d, float a0, float a1, int *on_foot = nullptr);
// Somebody called to a vehicle's door: it walks its own sidewalk as any walker (the crowd keeps it off the street's pieces and the
// others) to `at`, the arc across from the door, turning round for it when that is behind it; there it stands (kerb_came) and the stop
// walks it to the door by its way in (kerb_way_in). False when it may not come (one who may not be called, or farther than `reach`).
bool kerb_call(CityState &s, int ped, const V2 &door, float at, float reach);
void kerb_came(const CityState &s, Pedestrian &w); // pedestrians.cpp walk: at the arc it was called to, held there by the stop
// The call it walked to is over: it walks its own way again, turned back when it had turned round for it (`back`; a fright that turns
// it has chosen its way already).
void kerb_forget_call(Pedestrian &w, bool back);
// Where somebody a stop has in hand is: on its way to the door's arc (called, still walking), held by the stop, or lost to it (gone, or
// walking its own way again).
enum class KerbHold : uint8_t { LOST, COMING, HELD };
KerbHold kerb_hold(const TrafficState &t, int ped);
inline bool kerb_holds(const TrafficState &t, int ped) { return kerb_hold(t, ped) == KerbHold::HELD; }
// The one called has come to the door's arc: its foot, the place on its footway nearest where it stands whose straight ways from it and on
// to the door pass clear of the street's pieces; false with none (it cannot get to the door from there).
bool kerb_way_in(CityState &s, int ped, const V2 &door, V2 &foot);
// Its way in: from where it stood when it came across to its foot, and on to the door; how long, and where it is `metres` along (with
// the way it heads there).
float kerb_way_len(const V2 &from, const V2 &foot, const V2 &door);
V2 kerb_way_at(const V2 &from, const V2 &foot, const V2 &door, float metres, V2 &heading);
void kerb_place(CityState &s, int ped, const V2 &at, const V2 &facing); // moved there, its way heading `facing`
void kerb_show(CityState &s, int ped, const V2 &at, const V2 &facing); // where somebody held first shows: there, facing that way
void kerb_let_go(CityState &s, int ped, bool on_foot);
int kerb_walker_out(CityState &s, int ds, const V2 &foot); // somebody out on the footway beside `foot`, -1 with no room
// Where somebody out of the door of a vehicle on `ds` sets foot on the footway: the place nearest the one across from the door whose
// straight way from the door passes clear of the street's pieces (ped_furniture.cpp clear_foot); false with none near.
bool kerb_foot(CityState &s, int ds, const KerbDoors &at, V2 &foot);

// bus_calls.cpp: who gets off and on at a stop. True when that is over; `over` lets go of whoever is left when the bus goes.
bool bus_call(CityState &s, int id, BusCall &c, float dt);
void bus_call_over(CityState &s, BusCall &c);

// pedestrians.cpp: a walker on the footway at `arc` of the sidewalk, with a trip, held by the kerb stop (PedMode::KERB), -1 with
// no room there; a walker gone from the scene (aboard).
int ped_on_footway(CityState &s, int seg, int side, float arc);
void ped_gone(CityState &s, int id);

} // namespace traffic
} // namespace city
