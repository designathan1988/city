// The inside of a building cut open at a floor (the Interior layer): the floor, the cores, the walls of the plan and the
// furniture (interior_emit.cpp, furniture_emit.cpp), and the outside walls of the floor seen (interior_cut.cpp). The body
// is the cut one (sim/buildings/interior/cut.h): what is above the floor is already gone.
#pragma once

#include "render/buildings/facade_bay.h"
#include "sim/buildings/interior/interior.h"

namespace city::b40 {

// The outside wall of the floor seen, from the camera's side: the walls between the camera and the floor come down to
// this height; the rest keep theirs and show a face of render inside.
constexpr float CUT_WALL_LOW_M = 0.9f;
// How thick the outside wall is seen from inside (the inner face stands this far behind the outer one).
constexpr float CUT_SHELL_DEPTH = 0.25f;

// The floor, cores, walls of the plan and furniture of floor `pl.b->cut_level`; nothing when the body is not cut.
void emit_interior(Shell &shell, Shell &parts, const Placed &pl);
// One piece of furniture standing on the floor at height z (lot frame): its model's parts as instanced boxes in `parts` and
// its round parts as fine geometry in `shell`.
void emit_furniture(Shell &shell, Shell &parts, const Placed &pl, const Furniture &f, float z);
// A bay of the floor seen: true when the wall comes down to CUT_WALL_LOW_M (nothing more of the bay is drawn); otherwise
// the wall gets its inner face and the bay is drawn as usual. `out`: the wall's outward normal (lot frame).
bool cut_bay(Shell &shell, const Placed &pl, const BayCtx &c, V2 out);

} // namespace city::b40
