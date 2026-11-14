// The roads' heights kept in step with the network and the ground (sim/roads/road_grade.h): after an edit, an undo or
// redo, a stroke of the terrain, a load and a new city. Only here are the network and the ground seen together: the
// solve reads the natural ground (water standing as a floor), the roads whose heights moved shape the ground again
// (sim/terrain/road_shape.h) and tell the lanes, the furniture and the render (the ChangeLog).
#pragma once

#include "sim/roads/road_grade.h"

namespace city {

struct CityState;

struct Quad;

// The ground the roads of this city are laid over: the natural heights (the player's sculpting included, the roads' shaping
// not), the water of a river or a lake standing as a floor, and the sea's level. What the solve reads, and a road solved
// alone (road_grade.h solve_alone).
GroundAt road_ground(const CityState &s);

// Solves what changed since the last call and carries the heights on.
void sync_grade(CityState &s);
// Whether a road laid along q would cross water: laid at grade it is a bridge (the game's rule: water stands over no road;
// the 4.0's ground has no standing water, its player lifts a road over a river by its height).
bool crosses_water(const CityState &s, const Quad &q);

} // namespace city
