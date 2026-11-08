// The mound a building brought down leaves: one rough hill of rubble, a unit mesh placed over the ruin's footprint in one MultiMesh, rising as the
// building falls into it (render/shaders/rubble.gdshader). The loose pieces of effect_ruins.cpp lie on it.
#pragma once

#include "render/effects/mm_layer.h"

#include <godot_cpp/variant/color.hpp>

namespace city {

constexpr float MOUND_SPREAD = 1.15f; // a mound slumps past the walls it fell from, by this share of their footprint

class Mounds {
public:
	void init(const godot::RID &scenario, const godot::RID &material, int capacity);
	void reset();
	void release();
	// `at` the middle of its foot; w along the front, d deep, h high; born: the effects clock when it began to rise.
	void add(const godot::Vector3 &at, float rot, float w, float d, float h, const godot::Color &wall, float seed, double born);
	void update(double now);
	static float hill_at(float x, float z); // the unit hill's height at (x, z), both -0.5..0.5

private:
	MmLayer layer_;
	godot::RID material_;
	int capacity_ = 0, shown_ = 0;
	bool dirty_ = false;
};

} // namespace city
