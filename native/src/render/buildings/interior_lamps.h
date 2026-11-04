// The lamps of the floor a building is cut open at, in the world: where the room lights of the Interior layer go.
#pragma once

#include "sim/state/city_state.h"

#include <godot_cpp/variant/vector3.hpp>

#include <vector>

namespace city {

// The ceiling, floor and table lamps of floor `level` of building `id`, in the order the floor's furniture lists them (the 4.0's room lights take the first six); none when it
// is not there. A ceiling lamp hangs `LAMP_DROP_M` under the ceiling, the others sit at the share of their height `LAMP_RISE`.
std::vector<godot::Vector3> interior_lamps(const CityState &s, int id, int level);

} // namespace city
