// Body cache and world placement of the building records: lot frame at the middle of the lot front, +y away from the
// street;
// floor = highest ground under the ground volumes (corners and a 1.5 m grid) + 0.15 m plinth.
#include "render/buildings/building_bodies.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/blocks.h"

#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr float SHADE_BASE = 0.93f, SHADE_SPAN = 0.12f, SHADE_MOD = 1000.f;
constexpr uint32_t GOLDEN = 2654435761u;

} // namespace

const b40::Body &BuildingBodies::body(const b40::Data &data, const Building &rec, const b40::BodyInput *input) {
	const b40::BodyInput source = input ? *input : rec.body ? data.edited->input(rec.body) : b40::BodyInput{ data.edited->generation(), {} };
	const BodyIdentity identity = body_identity(rec, source);
	if (current_ && identity_ == identity) return *current_;
	rooms_.reset(); cut_.reset(); source_ = source;
	current_ = cache_->get(data, rec, source); identity_ = identity;
	return *current_;
}

const b40::Body &BuildingBodies::cut_body(const b40::Body &whole, const Building &rec, const b40::LotFrame &f, const b40::CutView &cut) {
	uint32_t turn;
	std::memcpy(&turn, &rec.rot, sizeof turn);
	if (cut_ && cut_level_ == cut.level && cut_dir_ == cut.dir && cut_turn_ == turn) return *cut_;
	const b40::Point2 world = b40::cut_direction(cut.dir);
	const b40::Point2 local{ world.x * f.e0.x + world.y * f.e0.y, world.x * f.e1.x + world.y * f.e1.y }; // the camera's way in the lot's frame
	cut_ = std::make_unique<b40::Body>(b40::cut_open(whole, cut.level, local));
	cut_level_ = cut.level; cut_dir_ = cut.dir; cut_turn_ = turn;
	return *cut_;
}

void BuildingBodies::place(const b40::Data &data, const b40::BuildingLook &look, const HeightPatch &ground, const Building &rec, b40::Placed &pl,
		const b40::CutView *cut, const b40::BodyInput *input) {
	const b40::LotFrame f = b40::lot_frame(rec);
	const b40::Body &whole = body(data, rec, input);
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
	if (!rooms_) rooms_ = cache_->rooms(data, current_, b40::purpose_of(whole, rec.fn, rec.seed));
	pl.interior = &rooms_->inside;
	pl.rooms = cut ? nullptr : rooms_.get(); // a cut shows its actual interior, with clear pane glass
	pl.lot = b40::Basis{ o + e2 * floor, e0, e1, e2 };
	pl.id = rec.seed;
	pl.shade = SHADE_BASE + std::fmod(float(rec.seed * GOLDEN), SHADE_MOD) / SHADE_MOD * SHADE_SPAN;
	pl.ground_lo = lo - floor;
	const HeightPatch *patch = &ground;
	pl.ground = [patch](const Vector3 &p) { return patch->height_at(p.x, p.z); };
}

} // namespace city
