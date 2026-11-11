// The swath of every vehicle off the lanes (the lots' cars and the lorries at the docks): the discs of its body where it stands
// and, while it drives in or out, the discs of the rest of its way. It is the one thing that says where those vehicles are and will
// be. A way's discs are laid once, when its manoeuvre is made (lay_way: the body every man_disc_step_m along it); filing a swath only
// copies the stretch of them still ahead, so filing it before the lots move and again before the walkers step costs a copy, and no
// question of it builds anything. The vehicles read it to keep their bodies apart: one starts on its way only when no standing body
// of another lies on it (those that started before it excepted: it follows them), and moves only while its body, with its gap ahead,
// stays off the swaths of those that started before it; an earlier one never waits for a later one, so no two wait on each other. A
// car turns in from the street only when no swath leading out to the street lies on its way in, else it would stand on the lane over
// the stretch that one needs. The walkers read it too (ped_swaths.cpp): they never walk into a swath and step out of one.
#pragma once

#include "sim/traffic/manoeuvre.h"

#include <cstdint>
#include <vector>

namespace city {

struct CityState;
struct VehicleClass;
struct Swath;
struct SwathDisc;

namespace traffic {

// A body of class `c` along the whole of `path`: three discs a pose (its middle and a point toward either end, half its width round),
// a pose every man_disc_step_m and one at the end, into `out`.
void lay_way(const CityState &s, const VehicleClass &c, const Manoeuvre &path, std::vector<SwathDisc> &out);
// The first disc of a laid way that is not behind `at` metres along it by a step or more.
int way_from(const CityState &s, float at, int size);
// The end of a laid way's discs through the pose at `at` metres along it (a way that stops there: a car waiting inside its gate).
int way_to(const CityState &s, float at, int size);
// Every vehicle off the lanes' swath where it stands now (ParkingState::swaths): before the lots move and again before the walkers do.
void file_swaths(CityState &s);
// A body at `x`, `z` with `reach` round it may touch the swath's discs: within its box.
bool near_swath(const Swath &w, float x, float z, float reach);
// The order the next vehicle starting on its way takes (ParkingState::starts): every one later than all before it.
uint32_t next_start(CityState &s);
// Discs `from` on (up to `end`, -1 the way's end) of the laid way of `vehicle`, a disc pad wider, cross the body of another vehicle off
// the lanes that has not started on its way: it may not start on it.
bool way_has_standing(const CityState &s, int vehicle, const std::vector<SwathDisc> &way, int from, int end = -1);
// The same discs cross the swath of a vehicle whose way leads out to the street, or a standing body: a car may not turn in by them.
bool way_meets_outward(const CityState &s, int vehicle, const std::vector<SwathDisc> &way, int from);
// How far, up to `want`, `vehicle` (class `c`, started on its way at `order`) may move along `path` this tick: its body moved that far
// and its gap (its class's min_gap) beyond, a disc pad wider, keeps off the swaths of every vehicle that started before it.
float room_on_way(const CityState &s, int vehicle, uint32_t order, const Manoeuvre &path, const VehicleClass &c, float want);

} // namespace traffic
} // namespace city
