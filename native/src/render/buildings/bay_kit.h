// The openings of the mid level as instances of one shared mesh, the way a shape grammar places a kit of modules: a bay's frame,
// proportions and colours are an instance's 64 bytes (shell.h BayInstance), where its own geometry would cost kilobytes. The city
// repeats its bays by the hundred (the census: 172,508 bays, 256 forms cover 82 %), so geometry reaches every bay of the view.
#pragma once


#include <godot_cpp/classes/array_mesh.hpp>

namespace city::b40 {

godot::Ref<godot::ArrayMesh> bay_mesh();

} // namespace city::b40
