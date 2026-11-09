// What the mid level throws in the sun: its own walls, welded and simplified (meshoptimizer), positions alone. The shadow pass then runs on a fraction of the
// triangles and in the engine's shared depth material (shadow_caster.gdshader), where the walls' material pays its vertex() in every cascade.
#pragma once

#include "render/buildings/shell.h"
#include "render/rs_util.h"

namespace city {

// The walls and panes of a sub-chunk as one mesh; no mesh when they hold nothing. `error_m`: how far a surface may move, in metres.
OwnedMesh cast_hull(const b40::Shell &walls, const b40::Shell &panes, float error_m, const godot::RID &material);

} // namespace city
