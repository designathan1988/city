// The street lamps' real light: a spot under the lens of the lamps nearest the eye, as many as the graphics tier pays for, so
// what is lit shows its own albedo and whatever stands in the street is lit too. Past them (`lamp_lit`, a global of the shaders)
// the road draws the lamps' pools itself (render/shaders/road_body.gdshaderinc), and the two fade into each other. A light keeps
// its lamp from frame to frame: it is placed again only when the lamp leaves the list, and an instance moved re-culls.
#pragma once

#include "render/view_cull.h"
#include "sim/catalog/catalog.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

// tuning.json render.lamp_*, and the colour of the lamps' glass.
struct LampLightLook {
	godot::Color color;
	float energy = 0, range = 0, angle_deg = 0, cone_fall = 0, decay = 0, specular = 0;
	float reach = 0; // farthest a lamp is lit from the eye
	float fade_from = 0; // of how far the lit lamps go, where they start giving way to the road's pools
	float cast_reach = 0; // farthest a lamp casts from the eye: a shadow under a car costs the pass whether it shows or not
	static LampLightLook of(const Tuning &t, const godot::Color &glass);
};

class LampLights {
public:
	void init(const godot::RID &scenario, const LampLightLook &look);
	void reset();
	void set_chunk(int64_t key, std::vector<godot::Vector3> &&lamps);
	void drop_chunk(int64_t key);
	// The graphics tier's lights, and how many of them cast; none: the road draws every pool.
	void set_budget(int count, int casting) {
		budget_ = count;
		cast_budget_ = casting;
	}
	void set_night(float night) { night_ = night; }
	void update(const godot::Vector3 &eye, const ViewCull &view, float frame_s);
	int lit() const { return lit_; } // lights on now (render_cost probe)
	godot::Vector2 handed_over() const { return published_; } // `lamp_lit`, for the tests: the engine reads a global back only in the editor

private:
	struct Slot {
		godot::RID light, instance;
		uint64_t lamp = 0; // the key of its lamp, 0: none
		float energy = -1, opacity = -1;
		bool casts = false;
	};
	struct Wanted {
		float eye_m = 0; // on the ground, as the road measures it
		float sight_m = 0; // through the air
		uint64_t key = 0;
		godot::Vector3 at;
	};
	Slot make_slot() const;
	void free_slot(Slot &s) const;
	void fit();
	void pick(const godot::Vector3 &eye, const ViewCull &view);
	void place();
	void shine();
	void cast(Slot &s, float sight_m);
	void publish(const godot::Vector2 &lit);
	godot::RID scenario_;
	LampLightLook look_;
	std::unordered_map<int64_t, std::vector<godot::Vector3>> chunks_;
	std::vector<Slot> slots_;
	std::vector<Wanted> wanted_;
	godot::Vector2 published_ = godot::Vector2(-1, -1);
	float night_ = 0, reach_ = 0, target_ = 0, cast_reach_ = 0, cast_target_ = 0;
	int budget_ = 0, cast_budget_ = 0, lit_ = 0;
};

} // namespace city
