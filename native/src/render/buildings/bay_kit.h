// The openings of the mid level as instances of one shared mesh, the way a shape grammar places a kit of modules: a bay's frame,
// proportions and colours are an instance's 64 bytes (shell.h BayInstance), where its own geometry would cost kilobytes. The city
// repeats its bays by the hundred (the census: 172,508 bays, 256 forms cover 82 %), so geometry reaches every bay of the view.
#pragma once

#include "render/rs_util.h"

namespace city::b40 {

constexpr float BAY_MESH_MARK = 2.f; // UV2.y of the bay mesh: building.gdshader's vertex stage places it from the instance

OwnedMesh bay_mesh(const godot::RID &material); // made with the walls' material: the engine has its pipelines before a bay is drawn

} // namespace city::b40
