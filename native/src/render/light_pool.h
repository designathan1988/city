// Lights for what the city has too many of: a fixed count of spots handed each frame to the nearest of a list, each kept on its
// holder from frame to frame, faded by how far the lit ones reach so that one coming or going is never seen to pop. The nearest few
// may cast. Whoever fills the list says what the lights are for: render/props/lamp_lights.h, render/agents/head_lights.h.
#pragma once

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <cstdint>
#include <vector>

namespace city {

struct LightLook {
	godot::Color color;
	float energy = 0, range = 0, angle_deg = 0, cone_fall = 0, decay = 0, specular = 0;
	float fade_from = 0; // of how far the lit ones go, where they start to fade
	float cast_reach = 0; // farthest one casts from the eye: a shadow costs the pass whether it shows or not
	uint32_t layer = 0, casters = 0; // the layer it is hidden with, and what casts for it (render_layers.h)
};

// One that asks for a light: on the ground from the eye, through the air, who it is (0 is nobody) and where the light stands.
struct LightWanted {
	float eye_m = 0, sight_m = 0;
	uint64_t key = 0;
	godot::Transform3D at;
};

class LightPool {
public:
	void init(const godot::RID &scenario, const LightLook &look);
	void reset();
	void set_budget(int count, int casting) {
		budget_ = count;
		cast_budget_ = casting;
	}
	void set_night(float night) { night_ = night; }
	bool wanted() const { return budget_ > 0 && night_ > 0; }
	// The nearest of `list` take the lights; `most_m`: as far as any is lit. `moving`: every light is placed again, not only those that changed hands.
	void update(std::vector<LightWanted> &list, float most_m, float frame_s, bool moving);
	godot::Vector2 lit_span() const; // on the ground from the eye, from where they fade to where they end
	int lit() const { return lit_; }

private:
	struct Slot {
		godot::RID light, instance;
		uint64_t holder = 0; // 0: none
		float energy = -1, opacity = -1;
		bool casts = false;
	};
	Slot make_slot() const;
	void free_slot(Slot &s) const;
	void fit();
	void pick(std::vector<LightWanted> &list, float most_m);
	void place(const std::vector<LightWanted> &list, bool moving);
	void shine(const std::vector<LightWanted> &list);
	void cast(Slot &s, float sight_m);
	void put_out();
	godot::RID scenario_;
	LightLook look_;
	std::vector<Slot> slots_;
	float night_ = 0, reach_ = 0, target_ = 0, cast_reach_ = 0, cast_target_ = 0;
	int budget_ = 0, cast_budget_ = 0, lit_ = 0;
};

} // namespace city
