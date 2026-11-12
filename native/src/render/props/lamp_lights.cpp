// The lamps a light of the pool can go to (lamp_lights.h).
#include "render/props/lamp_lights.h"

#include "core/grid.h"
#include "core/math.h"
#include "render/render_layers.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/string_name.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float PLACE_GRAIN = 16.f; // of a metre: the grid a lamp's key is read on
constexpr float PUBLISHED_M = 0.25f; // a smaller change of the lit radius is not published
constexpr float CHUNK_HALF_DIAGONAL = 0.7072f;
constexpr double STRAIGHT_DOWN = -PI_D / 2; // a spot shines along its -Z
constexpr int HALF_KEY_BITS = 32;
constexpr uint32_t CASTERS = LAYER_FURNITURE | LAYER_VEHICLES | LAYER_PEOPLE; // a wall in every lamp's map is most of the cost

uint64_t lamp_key(const Vector3 &at) {
	const uint32_t x = uint32_t(int32_t(std::lround(at.x * PLACE_GRAIN))), z = uint32_t(int32_t(std::lround(at.z * PLACE_GRAIN)));
	return (uint64_t(x) << HALF_KEY_BITS | z) + 1;
}

float flat_distance(const Vector3 &a, const Vector3 &b) { return std::hypot(a.x - b.x, a.z - b.z); }
} // namespace

void LampLights::init(const RID &scenario, const Tuning &t, const Color &glass) {
	LightLook l;
	l.color = glass;
	l.energy = float(t.render_lamp_energy);
	l.range = float(t.render_lamp_range_m);
	l.angle_deg = float(t.render_lamp_cone_deg);
	l.cone_fall = float(t.render_lamp_cone_fall);
	l.decay = float(t.render_lamp_decay);
	l.specular = float(t.render_lamp_specular);
	l.fade_from = float(t.render_lamp_gives_way_from);
	l.cast_reach = float(t.render_lamp_cast_reach_m);
	l.size = float(t.render_lamp_lens_m);
	l.shadow = ShadowLook{ float(t.render_lamp_shadow_blur), float(t.render_lamp_shadow_bias), float(t.render_lamp_shadow_normal_bias) };
	l.layer = LAYER_FURNITURE;
	l.casters = CASTERS;
	pool_.init(scenario, l);
	reach_ = float(t.render_lamp_reach_m);
	range_ = l.range;
	publish(Vector2());
}

void LampLights::reset() {
	chunks_.clear();
	wanted_.clear();
	pool_.reset();
}

void LampLights::set_chunk(int64_t key, std::vector<Vector3> &&lamps) {
	if (lamps.empty()) chunks_.erase(key);
	else chunks_[key] = std::move(lamps);
}

void LampLights::drop_chunk(int64_t key) { chunks_.erase(key); }

void LampLights::lenses_near(float x, float z, float r, PackedVector3Array &out) const {
	const Vector3 at(x, 0, z);
	for (const auto &kv : chunks_)
		for (const Vector3 &lens : kv.second)
			if (flat_distance(lens, at) <= r) out.push_back(lens);
}

// The lamps whose light can reach the view: one behind the eye still counts when its light lies before it.
void LampLights::gather(const Vector3 &eye, const ViewCull &view) {
	const Basis down(Vector3(1, 0, 0), real_t(STRAIGHT_DOWN));
	const float chunk_pad = float(CHUNK) * CHUNK_HALF_DIAGONAL, half = float(CHUNK) * 0.5f;
	for (const auto &kv : chunks_) {
		const Vector3 middle(float(key_x(kv.first)) * float(CHUNK) + half, eye.y, float(key_y(kv.first)) * float(CHUNK) + half);
		if (flat_distance(middle, eye) > reach_ + chunk_pad) continue;
		for (const Vector3 &at : kv.second) {
			const float far = flat_distance(at, eye);
			if (far <= reach_ && view.sees(at, range_)) wanted_.push_back({ far, at.distance_to(eye), lamp_key(at), Transform3D(down, at) });
		}
	}
}

void LampLights::publish(const Vector2 &lit) {
	if (std::fabs(lit.x - published_.x) < PUBLISHED_M && std::fabs(lit.y - published_.y) < PUBLISHED_M) return;
	published_ = lit;
	static const StringName name("lamp_lit");
	RenderingServer::get_singleton()->global_shader_parameter_set(name, lit);
}

void LampLights::update(const Vector3 &eye, const ViewCull &view, float frame_s) {
	wanted_.clear();
	if (pool_.wanted()) gather(eye, view);
	pool_.update(wanted_, reach_, frame_s, false);
	publish(pool_.lit_span());
}

} // namespace city
