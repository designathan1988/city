// A body is blocks put together like bricks, and the blocks are never changed by that (the 4.0's world/buildings/blocks.ts): a void block takes its space from the solid
// ones, an intersect block keeps only what lies in it, and an exclusive block does both. What is drawn is the solid blocks that come out.
#pragma once

#include "sim/buildings/b40/types.h"

namespace city::b40 {

bool has_modes(const Body &b);
// `b` as its solid blocks: nothing changes in a body whose blocks are all solid.
void resolve_blocks(Body &b);

} // namespace city::b40
