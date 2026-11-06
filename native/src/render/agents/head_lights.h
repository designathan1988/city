// The vehicles' headlamps at night: a spot out of the nose of the vehicles nearest the eye, as many as the graphics tier pays for,
// on the road ahead of each. The lamps themselves are the bodies' own glass; this is the light they throw.
#pragma once

#include "render/light_pool.h"
#include "render/view_cull.h"
#include "sim/catalog/catalog.h"
#include "sim/state/pose_buffer.h"

#include <godot_cpp/variant/vector3.hpp>

#include <vector>

namespace city {

class HeadLights {
public:
	void init(const godot::RID &scenario, const Tuning &t, const godot::Color &beam);
	void reset() { pool_.reset(); }
	void set_budget(int count) { pool_.set_budget(count, 0); }
	void set_night(float night) { pool_.set_night(night); }
	void begin() { wanted_.clear(); }
	// A vehicle at `p`, its nose `nose_m` ahead of it, as the fleet is walked this frame.
	void add(uint32_t id, const Pose &p, float nose_m, const godot::Vector3 &eye, const ViewCull &view);
	void shine(float frame_s) { pool_.update(wanted_, reach_, frame_s, true); }
	int lit() const { return pool_.lit(); }

private:
	LightPool pool_;
	std::vector<LightWanted> wanted_;
	float reach_ = 0, range_ = 0, height_ = 0, dip_ = 0;
};

} // namespace city
