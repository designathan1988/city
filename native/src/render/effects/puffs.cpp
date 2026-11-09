// The puff pool (puffs.h).
#include "render/effects/puffs.h"
#include "render/surface_data.h"

#include "core/math.h"

#include <godot_cpp/classes/quad_mesh.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float NEVER = -1e6f; // a slot's birth until it is written: long dead
} // namespace

void Puffs::init(const RID &scenario, const RID &material, const EffectParams &params) {
	params_ = params;
	pool_ = params.pool;
	material_ = material;
	Ref<QuadMesh> quad;
	quad.instantiate();
	quad->set_size(Vector2(1, 1));
	layer_.init(scenario, OwnedMesh::of(mesh_of_arrays(quad->get_mesh_arrays(), material)), pool_, false, AABB(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M)));
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
	const PuffsShape &K = params_.shapes.puffs;
	for (int n = 0; n < count; n++) {
		const float a = rng_.f() * TAU_F, r = std::sqrt(rng_.f()) * spread;
		const float s = size * (K.size_min + K.size_span * rng_.f()), l = life * (K.life_low + K.life_span * rng_.f());
		const Vector3 at(x + std::cos(a) * r, y + (rng_.f() - K.spread_floor) * spread * K.spread_flatten, z + std::sin(a) * r);
		layer_.put(next_, Vector3(s, 0, 0), Vector3(0, s, 0), Vector3(0, 0, s), at, float(now_), float(kind), rng_.f(), l);
		next_ = (next_ + 1) % pool_;
		last_death_ = std::max(last_death_, now_ + double(l));
	}
	dirty_ = dirty_ || count > 0;
}

void Puffs::stamp(double now) {
	now_ = now;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "now", float(now));
	rs->material_set_param(material_, "wind_dir", wind_);
}

void Puffs::flush() {
	if (dirty_) {
		layer_.upload();
		dirty_ = false;
	}
	const bool live = active(now_);
	if (live == shown_) return;
	layer_.shown(live);
	shown_ = live;
}

} // namespace city
