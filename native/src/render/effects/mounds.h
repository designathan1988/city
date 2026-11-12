// The mound a building brought down leaves: one rough hill of rubble, a unit mesh placed over the ruin's footprint in one MultiMesh, rising as the
// building falls into it (render/shaders/rubble.gdshader). The loose pieces of effect_ruins.cpp lie on it.
#pragma once

#include "render/effects/mm_layer.h"
#include "render/projected_error.h"

#include <array>
#include <godot_cpp/classes/array_mesh.hpp>

#include <godot_cpp/variant/color.hpp>
#include <unordered_map>
#include <vector>

namespace city {

class Mounds {
public:
	void init(const godot::RID &scenario, const godot::RID &material, int capacity, float sector_m = 0, float error_px = 0, float fade_s = 0);
	void reset();
	void release();
	// `at` the middle of its foot; w along the front, d deep, h high; born: the effects clock when it began to rise.
	void add(uint32_t owner, const godot::Vector3 &at, float rot, float w, float d, float h, const godot::Color &wall, float seed, double born);
	void remove(uint32_t owner);
	bool holds(uint32_t owner) const { return slots_.count(owner) != 0; }
	void set_view(const ProjectedErrorView &view) { view_ = view; }
	void update(double now);
	static float hill_at(float x, float z); // the unit hill's height at (x, z), both -0.5..0.5

private:
	static constexpr int LEVELS = 4;
	MmLayer layer_; // full-detail caster projection and canonical CPU rows
	std::array<godot::Ref<godot::ArrayMesh>, LEVELS> meshes_;
	SpatialInstanceDraw main_[LEVELS];
	struct Detail {
		std::array<double, LEVELS> error{};
		godot::AABB bounds;
		int level = 0, target = -1;
		double since = 0;
	};
	std::vector<Detail> detail_;
	std::vector<godot::Vector3> fine_positions_;
	std::vector<double> relief_scratch_;
	ProjectedErrorView view_, shown_view_;
	float error_px_ = 0, fade_s_ = 0;
	bool presentation_ = false, transitioning_ = false;
	Detail prepare_detail(const float *row);
	godot::RID material_;
	int capacity_ = 0, shown_ = 0;
	bool dirty_ = false;
	std::vector<uint32_t> owners_;
	std::unordered_map<uint32_t, int> slots_;
};

} // namespace city
