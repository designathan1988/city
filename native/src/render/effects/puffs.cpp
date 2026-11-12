// The puff pool (puffs.h).
#include "render/effects/puffs.h"

#include "core/math.h"

#include <godot_cpp/classes/quad_mesh.hpp>

#include <godot_cpp/core/math.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float SHADER_MIN_LIFE = 0.001f; // puffs.gdshader
constexpr double EXPIRY_ROUNDING_ULPS = 8; // outward bound for the shader's float clock/subtraction/division
constexpr float SIZE_MIN = 0.6f, SIZE_SPAN = 0.8f; // each puff's size and life vary by these shares
constexpr float LIFE_LOW = 0.7f, LIFE_SPAN = 0.6f;
constexpr float SPREAD_FLATTEN = 0.4f; // the vertical share of a burst's spread
constexpr float SPREAD_FLOOR = 0.3f; // puffs sit this much of the spread under and over the burst's height
} // namespace

void Puffs::init(const RID &scenario, const RID &material, int pool, float sector_m) {
	pool_ = pool;
	material_ = material;
	Ref<QuadMesh> quad;
	quad.instantiate();
	quad->set_size(Vector2(1, 1));
	layer_.init(scenario, quad, material, pool, false, AABB(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M)), std::max(1.f, sector_m));
	reset();
}

AABB Puffs::lifetime_bounds(const Puff &p) const {
	// puffs.gdshader: all live ages are in [0,1]. Include the entire trajectory, growth and any billboard orientation;
	// the bounding box remains valid while only the shader clock advances. Dead vertices are clipped by POSITION.
	const float half_life = Math::half_to_float(Math::make_half_float(p.life));
	const double life = std::max({ double(SHADER_MIN_LIFE), double(p.life), std::isfinite(half_life) ? double(half_life) : 0.0 });
	const double wx = std::fabs(double(wind_.x)), wz = std::fabs(double(wind_.y));
	double dx = 0, dz = 0, low = 0, high = 0, grow = 0;
	if (p.kind > 8.5f) {
		dx = 11 + wx * .9 * life + .8; dz = 11 + wz * .9 * life + .8;
		low = -.55 * life - .4; high = 9.4;
	} else if (p.kind > 7.5f) {
		dx = (2.2 + wx * .8) * life; dz = (2.2 + wz * .8) * life;
		low = -9.8 * life * life; high = 36 * life; grow = 1.2;
	} else if ((p.kind > 3.5f && p.kind < 4.5f) || (p.kind > 5.5f && p.kind < 7.5f)) {
		const double fling = p.kind > 5.5f && p.kind < 6.5f ? 3 : 1;
		dx = dz = 11 * fling * life; low = -12.25 * life * life; high = 15 * fling * life;
		grow = p.kind > 5.5f ? .2 : .6;
	} else if (p.kind > 4.5f) {
		dx = (4 + wx * 1.5) * life; dz = (4 + wz * 1.5) * life;
		low = -.3; high = 6 * life + .3; grow = 1.6;
	} else {
		const double spread = p.kind > 2.5f ? 2.2 : p.kind > .5f ? 1.2 : .5;
		dx = wx * (3 * life + .6 * life * life) + spread * life;
		dz = wz * (3 * life + .6 * life * life) + spread * life;
		low = -.15; high = 3.5 * life + .15;
		grow = p.kind > 2.5f ? 3.2 : p.kind > 1.5f ? 2.6 : 2;
	}
	constexpr double STEP_M = .4; // puffs.gdshader
	const double radius = std::fabs(double(p.size)) * (.5 + grow) * std::sqrt(.5);
	const double terms = std::max({ dx, dz, std::fabs(low), high, life * life, 1.0 });
	const double error = 64 * std::numeric_limits<float>::epsilon() *
			(std::max({ std::fabs(double(p.at.x)), std::fabs(double(p.at.y)), std::fabs(double(p.at.z)) }) + terms + radius + 1);
	const Vector3 extent(float(dx * STEP_M + radius + error), 0, float(dz * STEP_M + radius + error));
	const Vector3 lo = p.at - extent + Vector3(0, float(low * STEP_M - radius - error), 0);
	const Vector3 hi = p.at + extent + Vector3(0, float(high * STEP_M + radius + error), 0);
	return AABB(lo, hi - lo);
}

void Puffs::reset() {
	slots_.resize(size_t(std::max(0, pool_)));
	live_.clear();
	deaths_.clear();
	layer_.count(0);
	next_ = 0;
	dirty_ = true;
	last_death_ = 0;
}

void Puffs::release() { layer_.release(); slots_.clear(); live_.clear(); deaths_.clear(); }

void Puffs::burst(float x, float y, float z, int count, int kind, float spread, float size, float life) {
	if (pool_ <= 0) return;
	for (int n = 0; n < count; n++) {
		const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * spread;
		const float s = size * (SIZE_MIN + SIZE_SPAN * rng_.f()), l = life * (LIFE_LOW + LIFE_SPAN * rng_.f());
		const Vector3 at(x + std::cos(a) * r, y + (rng_.f() - SPREAD_FLOOR) * spread * SPREAD_FLATTEN, z + std::sin(a) * r);
		Puff &p = slots_[size_t(next_)];
		if (live_.count(next_)) deaths_.erase({p.expiry, next_});
		p.at = at; p.size = s; p.born = float(now_); p.kind = float(kind); p.seed = rng_.f(); p.life = l;
		const double shader_life = std::max(l, SHADER_MIN_LIFE);
		auto expiry = [](double born, double life) {
			return born + life + EXPIRY_ROUNDING_ULPS * std::numeric_limits<float>::epsilon() * (std::fabs(born) + life + 1);
		};
		// Compatibility stores custom data as halfs. Preserve either backend's complete visible lifetime.
		p.expiry = expiry(p.born, shader_life);
		const float half_born = Math::half_to_float(Math::make_half_float(p.born)), half_life = Math::half_to_float(Math::make_half_float(p.life));
		if (std::isfinite(half_born) && std::isfinite(half_life)) p.expiry = std::max(p.expiry, expiry(half_born, std::max(half_life, SHADER_MIN_LIFE)));
		live_.insert(next_);
		deaths_.insert({p.expiry, next_});
		next_ = (next_ + 1) % pool_;
		last_death_ = std::max(last_death_, p.expiry);
	}
	dirty_ = dirty_ || count > 0;
}

void Puffs::begin_frame(double now) {
	now_ = now;
	const double shader_now = double(float(now));
	while (!deaths_.empty() && deaths_.begin()->first < shader_now) {
		live_.erase(deaths_.begin()->second);
		deaths_.erase(deaths_.begin());
		dirty_ = true;
	}
}

void Puffs::flush() {
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "now", float(now_));
	rs->material_set_param(material_, "wind_dir", wind_);
	if (!dirty_) { layer_.settle(); return; }
	int row = 0;
	for (int slot : live_) {
		const Puff &p = slots_[size_t(slot)];
		layer_.put(row++, Vector3(p.size, 0, 0), Vector3(0, p.size, 0), Vector3(0, 0, p.size), p.at,
			p.born, p.kind, p.seed, p.life);
		layer_.bounds(row - 1, lifetime_bounds(p));
	}
	// Visible count excludes dead capacity; no INSTANCE_ID input is used by the puff shader.
	layer_.count(row);
	layer_.shown(row > 0);
	layer_.upload();
	dirty_ = false;
}

} // namespace city
