// The Build tools' edits of a building's body. An `op_` works on a copy, knows nothing of the city and answers "" or the
// strings.csv key of why it cannot; `edit_body` is the one way an edit reaches a record, so each is one undo step, is
// checked before it is kept, and saves with the city. Lengths in metres in the lot frame; `vol` an index into the volumes.
#pragma once

#include "sim/buildings/b40/types.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/commands.h"

#include <functional>
#include <string>

namespace city {

struct CityState;

using BodyOp = std::function<std::string(b40::Body &)>;

// Runs `op` on a copy of the building's body, checks it and stores it as the record's new body. An op that changes nothing
// stores nothing.
EditResult edit_body(CityState &s, int id, const BodyOp &op);
// Why a body cannot stand, or "": sizes, levels, support.
std::string body_problem(const Tuning &t, const b40::Body &b);

// The mass. `snap` lays lengths on the Build grid; without it they go to the centimetre.
std::string op_set_storeys(b40::Body &b, const Tuning &t, int vol, int count);
std::string op_insert_storey(b40::Body &b, const Tuning &t, int vol, int at, bool above);
std::string op_resize(b40::Body &b, const Tuning &t, int vol, int side, float delta, bool snap);
std::string op_add_wing(b40::Body &b, const Tuning &t, int vol, int side, float depth, float length);
std::string op_add_setback(b40::Body &b, const Tuning &t, int vol, float inset, int storeys);
std::string op_split_at_floor(b40::Body &b, const Tuning &t, int vol, int level);
std::string op_remove_volume(b40::Body &b, const Tuning &t, int vol);
std::string op_set_heights(b40::Body &b, const Tuning &t, float ground_h, float storey_h);

} // namespace city
