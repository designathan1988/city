// The street lamps' real light: a spot under the lens of the lamps nearest the eye, as many as the graphics tier pays for, so
// what is lit shows its own albedo and whatever stands in the street is lit too. Past them (`lamp_lit`, a global of the shaders)
// the road draws the lamps' pools itself (render/shaders/road_body.gdshaderinc), and the two fade into each other.
#pragma once

#include "render/light_pool.h"
#include "render/view_cull.h"
#include "sim/catalog/catalog.h"

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

class LampLights {
public:
	void init(const godot::RID &scenario, const Tuning &t, const godot::Color &glass);
	void reset();
	void set_chunk(int64_t key, std::vector<godot::Vector3> &&lamps);
	void drop_chunk(int64_t key);
	// The graphics tier's lights, and how many of them cast; none: the road draws every pool.
	void set_budget(int count, int casting) { pool_.set_budget(count, casting); }
	void set_night(float night) { pool_.set_night(night); }
	void update(const godot::Vector3 &eye, const ViewCull &view, float frame_s);
	int lit() const { return pool_.lit(); } // lights on now (render_cost probe)
	const LightLook &look() const { return pool_.look(); } // their lens and how their shadows are read (render_cost probe)
	godot::Vector2 handed_over() const { return published_; } // `lamp_lit`, for the tests: the engine reads a global back only in the editor

private:
	void gather(const godot::Vector3 &eye, const ViewCull &view);
	void publish(const godot::Vector2 &lit);
	LightPool pool_;
	std::unordered_map<int64_t, std::vector<godot::Vector3>> chunks_;
	std::vector<LightWanted> wanted_;
	godot::Vector2 published_ = godot::Vector2(-1, -1);
	float reach_ = 0, range_ = 0; // farthest a lamp is lit from the eye, and how far its light goes
};

} // namespace city
