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

// What a stop does with a person, shared with the bus's calls (bus_calls.cpp).
struct KerbDoors {
	V2 door, foot; // at the vehicle's side, and on the footway across from it
};
KerbDoors kerb_door_at(const CityState &s, const Vehicle &v, float back_m); // this far behind its front
float kerb_arc_of(const DirSeg &d, float at); // the arc along its street of metre `at` of a directed segment
inline int kerb_side(const DirSeg &d) { return d.sign > 0 ? 0 : 1; } // the sidewalk on its kerb side (0 right, 1 left of a -> b)
int kerb_hailable(const CityState &s, const DirSeg &d, float a0, float a1); // the first walker there who may be called (alone, no child), or -1
bool kerb_hail(CityState &s, int ped, const V2 &door, float reach); // within reach, its straight way to the door clear of the street's pieces
void kerb_place(CityState &s, int ped, const V2 &at, const V2 &facing); // moved there, its way heading `facing`
void kerb_show(CityState &s, int ped, const V2 &at, const V2 &facing); // where somebody held first shows: there, facing that way
void kerb_let_go(CityState &s, int ped, bool on_foot);
int kerb_walker_out(CityState &s, int ds, const V2 &foot); // somebody out on the footway beside `foot`, -1 with no room
inline bool kerb_holds(const TrafficState &t, int ped) { return t.peds[size_t(ped)].alive && t.peds[size_t(ped)].mode == PedMode::KERB; }

// bus_calls.cpp: who gets off and on at a stop. True when that is over; `over` lets go of whoever is left when the bus goes.
bool bus_call(CityState &s, int id, BusCall &c, float dt);
void bus_call_over(CityState &s, BusCall &c);

// pedestrians.cpp: a walker on the footway at `arc` of the sidewalk, with a trip, held by the kerb stop (PedMode::KERB), -1 with
// no room there; a walker gone from the scene (aboard).
int ped_on_footway(CityState &s, int seg, int side, float arc);
void ped_gone(CityState &s, int id);

} // namespace traffic
} // namespace city
