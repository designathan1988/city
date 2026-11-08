// A roof's sizes the near level (roof_emit.cpp) and the mid level (far_mass.cpp) both draw, and the roof's planting keeps clear of
// (roof_plant.cpp): one owner, so nothing jumps when a building hands over from one level to the other.
#pragma once

namespace city::b40 {

constexpr float ROOF_EDGE_H = 0.85f; // a parapet's height over the roof
constexpr float ATTIC_H = 1.5f; // an attic crown's
constexpr float COPING_IN = 0.3f; // how far a coping reaches in over the roof
constexpr float ROOF_TEST_OUT = 0.03f; // how far out of a side the test for a volume covering it looks
constexpr float MACHINE_H = 3.f; // a machine room's height
constexpr float MACHINE_ROOM_M = 3.f; // a machine room's side, over its core
constexpr float SLAB_LIFT = 0.002f; // the roof's slab over the eaves, so the two never fight for a pixel

} // namespace city::b40
