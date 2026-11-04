// Body cache and world placement of the building records: lot frame at the middle of the lot front, +y away from the
// street;
// floor = highest ground under the ground volumes (corners and a 1.5 m grid) + 0.15 m plinth.
#include "render/buildings/building_bodies.h"

#include "sim/buildings/b40/record.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr size_t CACHE_MAX = 8192;
constexpr float PLINTH_MIN = 0.15f, GRID_STEP = 1.5f, SHADE_BASE = 0.93f, SHADE_SPAN = 0.12f, SHADE_MOD = 1000.f;
constexpr int LOT_GRID_MAX = 24;
constexpr uint32_t GOLDEN = 2654435761u;
constexpr uint64_t SEED_BITS = 32, ZONE_BITS = 8, FLOOR_BITS = 16, SIZE_Q = 10;
uint64_t key_of(const Building &r) {
	uint64_t k = uint64_t(r.seed) << SEED_BITS;
	k ^= uint64_t(r.zone) | (uint64_t(r.floors) << ZONE_BITS) | (uint64_t(r.fn) << (ZONE_BITS + FLOOR_BITS));
	k ^= uint64_t(std::lround(r.w * SIZE_Q)) * GOLDEN ^ (uint64_t(std::lround(r.d * SIZE_Q)) << ZONE_BITS);
	return k;
}

} // namespace

const b40::Body &BuildingBodies::body(const b40::Data &data, const Building &rec) {
	uint64_t k = key_of(rec);
	auto it = cache_.find(k);
	if (it != cache_.end()) return *it->second;
	if (cache_.size() >= CACHE_MAX) cache_.clear();
	auto b = std::make_unique<b40::Body>();
	b40::record_body(data, rec, *b);
	return *(cache_[k] = std::move(b));
}

void BuildingBodies::place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl) {
	const b40::Body &b = body(data, rec);
	const b40::LotFrame f = b40::lot_frame(rec);
	Vector3 o(f.o.x, 0, f.o.y), e0(f.e0.x, 0, f.e0.y), e1(f.e1.x, 0, f.e1.y), e2(0, 1, 0);
	auto ground_at = [&](float x, float y) {
		Vector3 p = o + e0 * x + e1 * y;
		return ground.height_at(p.x, p.z);
	};
	float hi = -b40::FAR_AWAY, lo = b40::FAR_AWAY;
	auto sample = [&](float x, float y) {
		float g = ground_at(x, y);
		hi = std::max(hi, g);
		lo = std::min(lo, g);
	};
	for (const b40::Volume &v : b.volumes) {
		if (v.base != 0) continue;
		int nx = std::min(LOT_GRID_MAX, int(std::ceil(v.w / GRID_STEP))), ny = std::min(LOT_GRID_MAX, int(std::ceil(v.d / GRID_STEP)));
		for (int i = 0; i <= nx; i++)
			for (int j = 0; j <= ny; j++) sample(v.x + v.w * float(i) / float(nx), v.y + v.d * float(j) / float(ny));
	}
	if (hi < lo) { sample(0, rec.d / 2); }
	float floor = hi + PLINTH_MIN;
	pl.data = &data;
	pl.look = &look;
	pl.b = &b;
	pl.lot = b40::Basis{ o + e2 * floor, e0, e1, e2 };
	pl.id = rec.seed;
	pl.shade = SHADE_BASE + std::fmod(float(rec.seed * GOLDEN), SHADE_MOD) / SHADE_MOD * SHADE_SPAN;
	pl.ground_lo = lo - floor;
	const HeightPatch *patch = &ground;
	pl.ground = [patch](const Vector3 &p) { return patch->height_at(p.x, p.z); };
}

} // namespace city
