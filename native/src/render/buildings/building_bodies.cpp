// Body cache and world placement of the building records: lot frame at the middle of the lot front, +y away from the
// street;
// floor = highest ground under the ground volumes (corners and a 1.5 m grid) + 0.15 m plinth.
#include "render/buildings/building_bodies.h"

#include "sim/buildings/b40/record.h"

#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr size_t CACHE_MAX = 8192;
constexpr float SHADE_BASE = 0.93f, SHADE_SPAN = 0.12f, SHADE_MOD = 1000.f;
constexpr size_t CUT_CACHE_MAX = 1024;
constexpr int CUT_LEVEL_SHIFT = 40, CUT_DIR_SHIFT = 52;
constexpr uint32_t GOLDEN = 2654435761u;
constexpr uint64_t CUT_MIX = 0x9e3779b97f4a7c15ull;

} // namespace

const b40::Body &BuildingBodies::body(const b40::Data &data, const Building &rec) {
	uint64_t k = b40::body_key(rec);
	auto it = cache_.find(k);
	if (it != cache_.end()) return *it->second;
	if (cache_.size() >= CACHE_MAX) clear();
	auto b = std::make_unique<b40::Body>();
	b40::record_mass(data, rec, *b);
	return *(cache_[k] = std::move(b));
}

const b40::Body &BuildingBodies::cut_body(const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut) {
	uint64_t k = b40::body_key(rec);
	uint32_t turn;
	std::memcpy(&turn, &rec.rot, sizeof turn);
	k = (k ^ (uint64_t(cut.level) << CUT_LEVEL_SHIFT) ^ (uint64_t(cut.dir) << CUT_DIR_SHIFT) ^ uint64_t(turn)) * CUT_MIX;
	auto it = cuts_.find(k);
	if (it != cuts_.end()) return *it->second;
	if (cuts_.size() >= CUT_CACHE_MAX) cuts_.clear();
	const b40::Point2 world = b40::cut_direction(cut.dir);
	const b40::Point2 local{ world.x * f.e0.x + world.y * f.e0.y, world.x * f.e1.x + world.y * f.e1.y }; // the camera's way in the lot's frame
	return *(cuts_[k] = std::make_unique<b40::Body>(b40::cut_open(whole, cut.level, local)));
}

void BuildingBodies::place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
		const b40::CutView *cut) {
	const b40::LotFrame f = b40::lot_frame(rec);
	const b40::Body &whole = body(data, rec);
	const b40::Body &b = cut ? cut_body(whole, rec, f, *cut) : whole;
	Vector3 o(f.o.x, 0, f.o.y), e0(f.e0.x, 0, f.e0.y), e1(f.e1.x, 0, f.e1.y), e2(0, 1, 0);
	float hi = 0, lo = 0;
	b40::lot_ground(whole, rec, f, [&](float x, float z) { return ground.height_at(x, z); }, hi, lo); // the whole's: a cut keeps the ground floor
	float floor = hi + b40::LOT_PLINTH_M;
	pl.data = &data;
	pl.look = &look;
	pl.b = &b;
	pl.whole = &whole;
	pl.fn = rec.fn;
	pl.rooms = nullptr; // a cut building shows its real inside, not a room behind its panes
	if (!cut) {
		std::unique_ptr<b40::PaneRooms> &rooms = rooms_[b40::body_key(rec)];
		if (!rooms) rooms = std::make_unique<b40::PaneRooms>(data, b, b40::purpose_of(b, rec.fn, rec.seed));
		pl.rooms = rooms.get();
	}
	pl.lot = b40::Basis{ o + e2 * floor, e0, e1, e2 };
	pl.id = rec.seed;
	pl.shade = SHADE_BASE + std::fmod(float(rec.seed * GOLDEN), SHADE_MOD) / SHADE_MOD * SHADE_SPAN;
	pl.ground_lo = lo - floor;
	const HeightPatch *patch = &ground;
	pl.ground = [patch](const Vector3 &p) { return patch->height_at(p.x, p.z); };
}

} // namespace city
