// World placement of the building records, with the rooms and cuts the render derives from their bodies (building_bodies.h): lot
// frame at the middle of the lot front, +y away from the street; floor = highest ground under the ground volumes (corners and a
// 1.5 m grid) + 0.15 m plinth.
#include "render/buildings/building_bodies.h"

#include "sim/buildings/b40/record.h"

#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
// The body's shade from its id: Knuth's multiplicative hash, taken in thousandths.
constexpr float SHADE_BASE = 0.93f, SHADE_SPAN = 0.12f, SHADE_MOD = 1000.f;
constexpr uint32_t GOLDEN = 2654435761u;

size_t body_cost(const b40::Body &b) { return sizeof(b40::Body) + b40::heap_bytes(b); }
} // namespace

void BuildingBodies::set_budget(size_t bytes) {
	rooms_.set_budget(bytes);
	cuts_.set_budget(bytes);
}

size_t BuildingBodies::KeyHash::operator()(const Key &k) const {
	return size_t(b40::BodyKeyHash::mix(b40::BodyKeyHash()(k.body), k.generation));
}
size_t BuildingBodies::KeyHash::operator()(const CutKey &k) const {
	constexpr int HALF_BITS = 32;
	const uint64_t h = b40::BodyKeyHash::mix((*this)(k.body), uint64_t(uint32_t(k.level)) | (uint64_t(uint32_t(k.dir)) << HALF_BITS));
	return size_t(b40::BodyKeyHash::mix(h, k.turn));
}

void BuildingBodies::clear() {
	rooms_.clear();
	cuts_.clear();
}

std::shared_ptr<b40::PaneRooms> BuildingBodies::rooms(const b40::Data &data, const Key &k, const std::shared_ptr<const b40::Body> &whole, const Building &rec) {
	return rooms_.get(k, [&]() {
		return std::make_shared<b40::PaneRooms>(data, whole, b40::purpose_of(*whole, rec.fn, rec.seed)); // it holds the body it reads
	}, [&](const b40::PaneRooms &) { return sizeof(b40::PaneRooms) + body_cost(*whole); }); // the body it keeps alive is its cost
}

std::shared_ptr<const b40::Body> BuildingBodies::cut_body(const Key &k, const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut) {
	uint32_t turn;
	std::memcpy(&turn, &rec.rot, sizeof turn);
	return cuts_.get(CutKey{ k, cut.level, cut.dir, turn }, [&]() {
		const b40::Point2 world = b40::cut_direction(cut.dir);
		const b40::Point2 local{ world.x * f.e0.x + world.y * f.e0.y, world.x * f.e1.x + world.y * f.e1.y }; // the camera's way in the lot's frame
		return std::shared_ptr<const b40::Body>(std::make_shared<b40::Body>(b40::cut_open(whole, cut.level, local)));
	}, body_cost);
}

void BuildingBodies::place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
		const b40::CutView *cut, bool with_rooms) {
	const b40::LotFrame f = b40::lot_frame(rec);
	const Key k = key_of(data, rec);
	pl.hold = b40::PlacedHold();
	pl.hold.whole = b40::record_mass(data, rec);
	const b40::Body &whole = *pl.hold.whole;
	if (cut) pl.hold.cut = cut_body(k, whole, rec, f, *cut);
	const b40::Body &b = cut ? *pl.hold.cut : whole;
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
	if (!cut && with_rooms) {
		pl.hold.rooms = rooms(data, k, pl.hold.whole, rec);
		pl.rooms = pl.hold.rooms.get();
	}
	pl.lot = b40::Basis{ o + e2 * floor, e0, e1, e2 };
	pl.id = rec.seed;
	pl.wear = wear_of(rec);
	// a run-down building's paint has gone duller all over; its blotches and the grime at its foot are the shell's (shell.cpp shaded)
	pl.shade = (SHADE_BASE + std::fmod(float(rec.seed * GOLDEN), SHADE_MOD) / SHADE_MOD * SHADE_SPAN) * (1 - look.wear_shade * pl.wear);
	pl.ground_lo = lo - floor;
	const HeightPatch *patch = &ground;
	pl.ground = [patch](const Vector3 &p) { return patch->height_at(p.x, p.z); };
}

} // namespace city
