// A light for the vehicles nearest the eye (head_lights.h).
#include "render/agents/head_lights.h"

#include "core/units.h"
#include "render/render_layers.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/transform3d.hpp>

#include <cmath>

using namespace godot;

namespace city {

void HeadLights::init(const RID &scenario, const Tuning &t, const Color &beam) {
	LightLook l;
	l.color = beam;
	l.energy = float(t.render_head_energy);
	l.range = float(t.render_head_range_m);
	l.angle_deg = float(t.render_head_cone_deg);
	l.cone_fall = float(t.render_head_cone_fall);
	l.decay = float(t.render_head_decay);
	l.specular = float(t.render_head_specular);
	l.fade_from = float(t.render_head_gives_way_from);
	l.layer = LAYER_VEHICLES;
	pool_.init(scenario, l);
	reach_ = float(t.render_head_reach_m);
	range_ = l.range;
	height_ = float(t.render_head_height_m);
	dip_ = float(t.render_head_dip_deg * DEG_TO_RAD);
}

void HeadLights::add(uint32_t id, const Pose &p, float nose_m, const Vector3 &eye, const ViewCull &view) {
	if (!pool_.wanted()) return;
	const Vector3 ahead(p.fx, p.rise - std::tan(dip_), p.fz), at(p.x + p.fx * nose_m, p.y + height_, p.z + p.fz * nose_m);
	const float far = std::hypot(at.x - eye.x, at.z - eye.z), half = range_ * 0.5f;
	if (far > reach_ || !view.sees(at + ahead * half, half)) return;
	wanted_.push_back({ far, at.distance_to(eye), uint64_t(id) + 1, Transform3D(Basis::looking_at(ahead.normalized(), Vector3(0, 1, 0)), at) });
}

} // namespace city
