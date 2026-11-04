// The puff pool (puffs.h).
#include "render/effects/puffs.h"

#include "core/math.h"

#include <godot_cpp/classes/quad_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float WORLD_HALF_M = 200000.f; // the pool's box: the whole map
constexpr float WORLD_UP_M = 20000.f;
constexpr float NEVER = -1e6f; // a slot's birth until it is written: long dead
constexpr float SIZE_MIN = 0.6f, SIZE_SPAN = 0.8f; // each puff's size and life vary by these shares
constexpr float LIFE_LOW = 0.7f, LIFE_SPAN = 0.6f;
constexpr float SPREAD_FLATTEN = 0.4f; // the vertical share of a burst's spread
constexpr float SPREAD_FLOOR = 0.3f; // puffs sit this much of the spread under and over the burst's height
} // namespace

void Puffs::init(const RID &scenario, const RID &material, int pool) {
	pool_ = pool;
	material_ = material;
	Ref<QuadMesh> quad;
	quad.instantiate();
	quad->set_size(Vector2(1, 1));
	layer_.init(scenario, quad, material, pool, false, AABB(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M)));
	reset();
}

void Puffs::reset() {
	for (int i = 0; i < pool_; i++) layer_.put(i, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), Vector3(), NEVER, 0, 0, 1);
	next_ = 0;
	dirty_ = true;
	last_death_ = 0;
}

void Puffs::release() { layer_.release(); }

void Puffs::burst(float x, float y, float z, int count, int kind, float spread, float size, float life) {
	for (int n = 0; n < count; n++) {
		const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * spread;
		const float s = size * (SIZE_MIN + SIZE_SPAN * rng_.f()), l = life * (LIFE_LOW + LIFE_SPAN * rng_.f());
		const Vector3 at(x + std::cos(a) * r, y + (rng_.f() - SPREAD_FLOOR) * spread * SPREAD_FLATTEN, z + std::sin(a) * r);
		layer_.put(next_, Vector3(s, 0, 0), Vector3(0, s, 0), Vector3(0, 0, s), at, float(now_), float(kind), rng_.f(), l);
		next_ = (next_ + 1) % pool_;
		last_death_ = std::max(last_death_, now_ + double(l));
	}
	dirty_ = dirty_ || count > 0;
}

void Puffs::update(double now) {
	now_ = now;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "now", float(now));
	rs->material_set_param(material_, "wind_dir", wind_);
	if (!dirty_) return;
	layer_.upload();
	dirty_ = false;
}

} // namespace city
