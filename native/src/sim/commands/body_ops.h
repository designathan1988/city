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

// How a part fits the wall it is set against: on the ground in front of the bay, spanning it, over its opening, as tall as the storey,
// a deck at its floor, a dial on the wall, a unit on the wall, a flight down to the ground.
enum PartFit : uint8_t { FIT_GROUND, FIT_SPAN, FIT_OVER, FIT_STOREY, FIT_DECK, FIT_DIAL, FIT_UNIT, FIT_FLIGHT };
// A part as data/ui/builder.json gives it. `out`: how far from the wall; `rise`, `drop`: over the floor, under the next.
struct PartSpec {
	int kind = 0, fit = FIT_GROUND;
	float w = 1, d = 1, h = 1, out = 0, rise = 0, drop = 0;
};

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

// The facade. `what` a b40::Bay, `scope` a b40::FacadeScope. A pattern goes on the whole building (`vol` < 0), a volume, one
// storey or one side of it (`storey`, `side` < 0 for all). Fields of `patch` left at their "not set" value stay as they are.
std::string op_set_bay(b40::Body &b, int vol, int storey, int side, int bay, int what, int scope);
std::string op_set_zone(b40::Body &b, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what);
std::string op_set_pattern(b40::Body &b, int vol, int storey, int side, int pattern);
std::string op_set_face(b40::Body &b, const Tuning &t, int vol, int side, const b40::FaceGeometry &patch);
std::string op_set_relief(b40::Body &b, const Tuning &t, int vol, const b40::Relief &region, bool snap);

// The roof, and what things are finished in. `pitch` < 0 goes back to the roof kind's own, `ridge` and `fall` -1 likewise;
// `scope` a b40::PaintScope, `finish` a b40::Finish, `rgb` 0xRRGGBB.
std::string op_set_roof(b40::Body &b, int vol, int roof);
std::string op_roof_shape(b40::Body &b, const Tuning &t, int vol, float pitch, int ridge, int fall);
std::string op_paint(b40::Body &b, int scope, int vol, int side, int storey, int finish, uint32_t rgb);
// What stands on a roof, at (x, y) of the plan; `gear` an index into the block's. It must fit the roof, corners and all.
std::string op_add_gear(b40::Body &b, const Tuning &t, int vol, int kind, float x, float y, float w, float d, float h);
std::string op_remove_gear(b40::Body &b, int vol, int gear);
std::string op_turn_gear(b40::Body &b, const Tuning &t, int vol, int gear);
// The flag of the mast (spire, lantern) at (x, y) of the plan: its design (a b40::FlagPattern, FLAG_NONE takes it down) and
// one of its three colours (`slot`, 0xRRGGBB; colouring a mast flying none raises the default flag, as the Roadcraft does).
std::string op_set_flag(b40::Body &b, int vol, float x, float y, int pattern);
std::string op_flag_colour(b40::Body &b, int vol, float x, float y, int slot, uint32_t rgb);

// The parts. `part` an index into the elements; a free part stands at (x, y) of the lot frame, facing a b40::Facing.
std::string op_add_part(b40::Body &b, const Tuning &t, int vol, int storey, int side, int bay, const PartSpec &s);
std::string op_add_part_at(b40::Body &b, const Tuning &t, float x, float y, int facing, const PartSpec &s);
std::string op_remove_part(b40::Body &b, int part);
std::string op_turn_part(b40::Body &b, int part);
std::string op_repeat_part(b40::Body &b, const Tuning &t, int part, float spacing);

} // namespace city
