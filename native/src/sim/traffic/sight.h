// What the player may see of the traffic, the one owner of the Roadcraft lab's sight rule (sim/ambient/ambient.ts traffic, sim/vehicles/
// spawn.ts stepDespawn): a car is never made nor taken away where the render may show it, the radius the poses are published in round the
// camera (traffic.json publish_radius_m); a trip begins out of sight but for a city just opened, and a car in sight that arrives drives on.
#pragma once

namespace city {
struct CityState;
struct Vehicle;

namespace traffic {

// Whether a trip may begin on directed segment `ds`: past the published radius, or anywhere while a city just opened is first filled
// (spawn_fresh_s).
bool unseen_start(const CityState &s, int ds);
// Whether `v` is the city's own traffic (no bus, bay, yard, errand or kerb stop: those end their own trips) drawn in sight: it drives on.
bool drives_on(const CityState &s, const Vehicle &v);

} // namespace traffic
} // namespace city
