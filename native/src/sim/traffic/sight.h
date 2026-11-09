// What the player may see of the traffic and the walkers, the one owner of the Roadcraft lab's sight rule (sim/ambient/ambient.ts `seen`,
// sim/vehicles/spawn.ts stepDespawn): nobody is made nor taken away where the render may show them. The render draws a vehicle within
// traffic.json publish_radius_m and a walker within publish_people_m of the camera's eye on the ground (CityState::camera_x/z: the render's
// agent levels measure the same flat distance from the same eye, render/agents/agent_renderer.cpp), and only what is published inside
// those radii; so past them nothing can be on screen. A trip or a walk begins out of sight but for a city just opened (spawn_fresh_s), and
// one that ends in sight goes on, or stands, until it is out of sight.
#pragma once

namespace city {
struct CityState;
struct Vehicle;

namespace traffic {

// Whether the render may show a vehicle at (x, z) / a walker at (x, z).
bool vehicle_may_show(const CityState &s, float x, float z);
bool person_may_show(const CityState &s, float x, float z);
// Whether a trip may begin on lane `lane` of directed segment `ds`, `at` metres along it: out of sight, or anywhere while a city just
// opened is first filled (spawn_fresh_s).
bool unseen_start(const CityState &s, int ds, int lane, float at);
// Whether a walker may be born on a footway at (x, z) (not out of a door): out of sight, or anywhere while a city just opened is filled.
bool unseen_birth(const CityState &s, float x, float z);
// Whether `v` is the city's own traffic (no bus, bay, yard, kerb or gate stop under way: those end their own trips) and may be seen: it
// drives on. An errand still to do goes on with it to the next trip.
bool drives_on(const CityState &s, const Vehicle &v);
// Vehicle `id`'s trip ends on the lane it is on, with nowhere else to go: out of sight it leaves; in sight it stands short of the lane's
// end until it is out of sight (Vehicle::ended). A bus is the transit's, which ends its runs itself (transit_sim.cpp).
void trip_ends_here(CityState &s, int id);

} // namespace traffic
} // namespace city
