// The gardens' plants held against their rooms, for the probes (TreeRenderer::garden_rooms and garden_fit): each plant element as
// place_garden stands it (render/trees/garden_place.h), what its instances draw worked out point by point from the kit's measure of
// their species (drawn_body.h) through the basis the renderer sends (tree_place.h tree_basis), in the lot's frame, against the
// element's own rectangle and height: the room the lot's layout kept off the boundary, the walls and the other parts.
#pragma once

#include "render/trees/garden_place.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace city {

// Every lot of the city: `lots` with plants, `plants`, `out` drawn past their room in plan by more than a centimetre (`worst_m` the
// farthest, `first` the first: building, kind, x, z, metres), `over` drawn taller than their element (`over_m`; a flower bed's clumps
// have their own height), `by_kind` the count of `out` by element kind. A plant laid at an angle is held in its own turned frame.
// `sights`: where a capture looks, of each kind the plant with the longest room and the one with the shortest (building, x, z, w, d, h).
godot::Dictionary garden_rooms(const CityView &s, const TreeLook &T, const TreeKit &kit);
// One plant element of that kind (b40::Element) `w` by `d` and `h` tall, laid `turn` radians to its lot, alone on the ground with
// the draws of `seed`: how many instances it is, the box they draw in its own frame (x0, y0, x1, y1 from its middle) and in the
// lot's (`lot_box`), their top, and how far past the room and over its height they stand.
godot::Dictionary garden_fit(const CityView &s, const TreeLook &T, const TreeKit &kit, int kind, float w, float d, float h, float turn, int seed);
// The grove of an open lot of function `fn` (BuildingFn), `w` by `d`, alone at the land's middle with the draws of `seed`
// (tree_place.h place_grove): its `trees`, how far past the lot's lines the farthest leaf stands (`past_m`), the most slender tree
// (`slender`, its height over its width) against the land's bound (`slender_most`), the least height and the least width drawn.
godot::Dictionary grove_fit(const CityView &s, const TreeLook &T, const TreeKit &kit, int fn, float w, float d, int seed);
// Every open lot of the city with a grove: `lots`, `trees`, `out` with a leaf past their lot's lines (`worst_m` the farthest).
godot::Dictionary grove_rooms(const CityView &s, const TreeLook &T, const TreeKit &kit);

} // namespace city
