// Pushes the authoritative heights (sim/terrain) into a Terrain3D node: all regions on load, only the
// touched regions afterwards (Terrain3DData.update_maps with all_regions = false).
#pragma once

#include "sim/terrain/heightfield.h"

#include <godot_cpp/classes/object.hpp>

namespace city {

class TerrainBridge {
public:
	void push_all(const Heightfield &hf, godot::Object *terrain3d);
	// rects: x0,z0,x1,z1 sample rectangles from ChangeLog::terrain_rects.
	void push_rects(const Heightfield &hf, godot::Object *terrain3d, const std::vector<int> &rects);

private:
	void write_region(const Heightfield &hf, godot::Object *data, int bx, int bz, bool create);
};

} // namespace city
