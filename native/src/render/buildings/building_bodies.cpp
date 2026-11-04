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
constexpr float SHADE_BASE = 0.93f, SHADE_SPAN = 0.12f, SHADE_MOD = 1000.f;
constexpr uint32_t GOLDEN = 2654435761u;
constexpr uint64_t EDITED_MIX = 0x9e3779b97f4a7c15ull;
constexpr uint64_t SEED_BITS = 32, ZONE_BITS = 8, FLOOR_BITS = 16, SIZE_Q = 10;
uint64_t key_of(const Building &r) {
	uint64_t k = uint64_t(r.seed) << SEED_BITS;
	k ^= uint64_t(r.zone) | (uint64_t(r.floors) << ZONE_BITS) | (uint64_t(r.fn) << (ZONE_BITS + FLOOR_BITS));
	k ^= uint64_t(std::lround(r.w * SIZE_Q)) * GOLDEN ^ (uint64_t(std::lround(r.d * SIZE_Q)) << ZONE_BITS);
	return k ^ uint64_t(r.body) * EDITED_MIX; // an edited body is its own, whatever its seed says
}

} // namespace

const b40::Body &BuildingBodies::body(const b40::Data &data, const Building &rec) {
	uint64_t k = key_of(rec);
	auto it = cache_.find(k);
	if (it != cache_.end()) return *it->second;
	if (cache_.size() >= CACHE_MAX) clear();
	auto b = std::make_unique<b40::Body>();
	b40::record_body(data, rec, *b);
	return *(cache_[k] = std::move(b));
}

void BuildingBodies::place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl) {
	const b40::Body &b = body(data, rec);
	const b40::LotFrame f = b40::lot_frame(rec);
	Vector3 o(f.o.x, 0, f.o.y), e0(f.e0.x, 0, f.e0.y), e1(f.e1.x, 0, f.e1.y), e2(0, 1, 0);
	float hi = 0, lo = 0;
	b40::lot_ground(b, rec, f, [&](float x, float z) { return ground.height_at(x, z); }, hi, lo);
	float floor = hi + b40::LOT_PLINTH_M;
	pl.data = &data;
	pl.look = &look;
	pl.b = &b;
	std::unique_ptr<b40::PaneRooms> &rooms = rooms_[key_of(rec)];
	if (!rooms) rooms = std::make_unique<b40::PaneRooms>(data, b, b40::purpose_of(b, rec.fn, rec.seed));
	pl.rooms = rooms.get();
	pl.lot = b40::Basis{ o + e2 * floor, e0, e1, e2 };
	pl.id = rec.seed;
	pl.shade = SHADE_BASE + std::fmod(float(rec.seed * GOLDEN), SHADE_MOD) / SHADE_MOD * SHADE_SPAN;
	pl.ground_lo = lo - floor;
	const HeightPatch *patch = &ground;
	pl.ground = [patch](const Vector3 &p) { return patch->height_at(p.x, p.z); };
}

} // namespace city
