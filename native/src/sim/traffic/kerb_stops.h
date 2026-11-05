// Errands at the kerb (kerb_stops.cpp): a vehicle's trip draws one at birth, pulls up for it on the kerb lane, and the person moves
// between the door and the footway; the IDM holds it at its stop.
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

void assign_errand(const CityState &s, Vehicle &v, int id); // at its birth: what this trip does at the kerb
void step_kerb_stops(CityState &s, float dt);
float kerb_stop_gap(const TrafficState &t, const Vehicle &v); // metres to where its front stops
void kerb_forget(CityState &s, int id); // it left the scene

// pedestrians.cpp: a walker on the footway at `arc` of the sidewalk, with a trip, held by the kerb stop (PedMode::KERB), -1 with
// no room there; a walker gone from the scene (aboard).
int ped_on_footway(CityState &s, int seg, int side, float arc);
void ped_gone(CityState &s, int id);

} // namespace traffic
} // namespace city
