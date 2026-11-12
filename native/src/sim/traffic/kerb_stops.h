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
float kerb_footway_arc(const RoadSegment &g, const V2 &foot); // canonical foot projection, reused after query wait
float kerb_arc_of(const DirSeg &d, float at); // the arc along its street of metre `at` of a directed segment
int kerb_hailable(const CityState &s, const DirSeg &d, float a0, float a1); // the first walker alone on the kerb's footway there, or -1
bool kerb_hail(CityState &s, int ped, const V2 &door, float reach);
void kerb_place(CityState &s, int ped, const V2 &at, const V2 &facing);
void kerb_let_go(CityState &s, int ped, bool on_foot);
std::shared_ptr<PedBirthTicket> kerb_walker_out(CityState &s, int vehicle, int ds, const V2 &foot); // explicit pending outcome, no placeholder actor
inline bool kerb_current(const TrafficState &t, int ped, uint64_t generation) {
	return ped >= 0 && size_t(ped) < t.peds.size() && t.peds[size_t(ped)].alive && t.peds[size_t(ped)].generation == generation;
}
inline bool kerb_holds(const TrafficState &t, int ped, uint64_t generation) { return kerb_current(t,ped,generation) && t.peds[size_t(ped)].mode == PedMode::KERB; }

// bus_calls.cpp: who gets off and on at a stop. True when that is over; `over` lets go of whoever is left when the bus goes.
bool bus_call(CityState &s, int id, BusCall &c, float dt);
void bus_call_over(CityState &s, BusCall &c);

// pedestrians.cpp: a walker gone from the scene (aboard).
void ped_gone(CityState &s, int id);

} // namespace traffic
} // namespace city
