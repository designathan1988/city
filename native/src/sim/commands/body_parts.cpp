// The Parts tab's edits: free elements set against a bay or on the ground, changed, repeated and taken away (body_ops.h). How a part
// fits a wall is data (`PartSpec`, from data/ui/builder.json): a part stands in the lot frame, w across its facing, d along it.
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_TARGET = "tool.failed";
constexpr float AREA_EPS = 1e-3f, TOUCH_EPS = 1e-3f;
constexpr int MAX_REPEAT = 64;

// The frame of a side of a block: where its bays start, the way they run, and out of the wall.
struct WallFrame {
	Point2 start, along, out;
	float length = 0;
};

WallFrame wall_of(const Volume &v, int side) {
	const Ring ring = ring_of(v);
	Point2 p = ring[size_t(side) % ring.size()], q = ring[size_t(side + 1) % ring.size()];
	WallFrame f;
	f.length = std::hypot(q.x - p.x, q.y - p.y);
	f.start = p;
	f.along = { (q.x - p.x) / f.length, (q.y - p.y) / f.length };
	f.out = { f.along.y, -f.along.x };
	return f;
}

// The element's size in the lot frame: w runs across the facing, so a part facing a side has its two lengths swapped.
void set_size(LotElement &e, float across, float deep) {
	bool front = e.facing == FACE_STREET || e.facing == FACE_BACK;
	e.w = front ? across : deep;
	e.d = front ? deep : across;
}

int facing_of(Point2 out) {
	if (std::fabs(out.x) > std::fabs(out.y)) return out.x > 0 ? FACE_RIGHT : FACE_LEFT;
	return out.y > 0 ? FACE_BACK : FACE_STREET;
}

// The first block the part would stand inside, or null. Touching is allowed; what hangs on a wall only clashes by its middle.
const Volume *clash(const Body &b, const LotElement &e, bool on_wall) {
	const Ring box = { { e.x - e.w / 2, e.y - e.d / 2 }, { e.x + e.w / 2, e.y - e.d / 2 }, { e.x + e.w / 2, e.y + e.d / 2 }, { e.x - e.w / 2, e.y + e.d / 2 } };
	for (const Volume &v : b.volumes) {
		float z0 = b.level_z(v.base), z1 = b.level_z(v.top());
		if (e.z >= z1 - TOUCH_EPS || z0 >= e.z + e.h - TOUCH_EPS) continue;
		bool in = on_wall ? contains_point(ring_of(v), { e.x, e.y }) : overlap_area(ring_of(v), box) > AREA_EPS;
		if (in) return &v;
	}
	return nullptr;
}

bool has(const Body &b, int part) { return part >= 0 && part < int(b.elements.size()); }
bool hung(const PartSpec &s) { return s.fit == FIT_OVER || s.fit == FIT_UNIT || s.fit == FIT_DIAL; }

void place(LotElement &e, const WallFrame &f, float along, float out) {
	e.x = f.start.x + f.along.x * along + f.out.x * out;
	e.y = f.start.y + f.along.y * along + f.out.y * out;
}
} // namespace

std::string op_add_part(Body &b, const Tuning &t, int vol, int storey, int side, int bay, const PartSpec &s) {
	if (vol < 0 || vol >= int(b.volumes.size())) return NO_TARGET;
	const Volume &v = b.volumes[size_t(vol)];
	if (side < 0 || side >= v.sides() || storey < 0 || storey >= v.storeys) return NO_TARGET;
	if (int(b.elements.size()) >= int(t.build_max_parts)) return "building.problem.size";
	WallFrame f = wall_of(v, side);
	float width = f.length / float(bays_of(v, side)), along = (float(bay) + 0.5f) * width;
	float floor = b.level_z(v.base + storey), next = b.level_z(v.base + storey + 1), tall = next - floor;
	LotElement e;
	e.kind = Element(s.kind);
	e.facing = facing_of(f.out);
	e.h = s.h;
	float across = s.w, deep = s.d, out = s.out;
	switch (s.fit) {
		case FIT_SPAN: across = std::max(width, s.w); break;
		case FIT_OVER:
			across = std::max(width, s.w);
			out = s.d / 2;
			e.z = std::max(floor + s.rise, next - s.drop);
			break;
		case FIT_STOREY:
			along = std::round(along / width) * width;
			out = s.d / 2 + s.out;
			e.z = floor;
			e.h = tall;
			break;
		case FIT_DECK:
			across = std::max(width, s.w);
			out = s.d / 2;
			e.z = std::max(0.f, floor - s.h);
			break;
		case FIT_DIAL: {
			float size = std::min({ s.w, width * float(t.build_dial_share), tall * float(t.build_dial_share) });
			across = size;
			e.h = size;
			out = s.d / 2;
			e.z = floor + (tall - size) / 2;
			break;
		}
		case FIT_UNIT:
			out = s.d / 2;
			e.z = floor + std::min(s.rise, tall / 2);
			break;
		case FIT_FLIGHT: {
			float rise = std::max(floor, RISER_M);
			bool ramp = s.kind == EL_RAMP;
			float steps = std::ceil(rise / RISER_M);
			deep = ramp ? rise * float(t.build_ramp_run_per_rise) : steps * float(t.build_step_run_m);
			out = deep / 2;
			e.h = rise;
			break;
		}
		default: break;
	}
	set_size(e, across, deep);
	place(e, f, along, out);
	if (clash(b, e, hung(s))) return "building.problem.overlap";
	b.elements.push_back(e);
	return "";
}

std::string op_add_part_at(Body &b, const Tuning &t, float x, float y, int facing, const PartSpec &s) {
	if (int(b.elements.size()) >= int(t.build_max_parts)) return "building.problem.size";
	LotElement e;
	e.kind = Element(s.kind);
	e.facing = facing;
	e.h = s.h;
	float grid = float(t.build_grid_m);
	e.x = std::round(x / grid) * grid;
	e.y = std::round(y / grid) * grid;
	set_size(e, s.w, s.d);
	if (clash(b, e, false)) return "building.problem.overlap";
	b.elements.push_back(e);
	return "";
}

std::string op_remove_part(Body &b, int part) {
	if (!has(b, part)) return NO_TARGET;
	b.elements.erase(b.elements.begin() + part);
	return "";
}

std::string op_turn_part(Body &b, int part) {
	if (!has(b, part)) return NO_TARGET;
	LotElement &e = b.elements[size_t(part)];
	e.facing = (e.facing + 1) % 4;
	std::swap(e.w, e.d);
	return clash(b, e, false) ? "building.problem.overlap" : "";
}

// A row across the part's facing, out to the corners of the building's plan, as far as it stays clear of the blocks.
std::string op_repeat_part(Body &b, const Tuning &t, int part, float spacing) {
	if (!has(b, part) || b.volumes.empty()) return NO_TARGET;
	const LotElement base = b.elements[size_t(part)];
	bool along_x = base.facing == FACE_STREET || base.facing == FACE_BACK;
	float across = along_x ? base.w : base.d;
	float step = spacing > 0 ? spacing : std::max(across * 2, float(t.build_module_m));
	float lo = FAR_AWAY, hi = -FAR_AWAY;
	for (const Volume &v : b.volumes)
		for (const Point2 &p : ring_of(v)) {
			lo = std::min(lo, along_x ? p.x : p.y);
			hi = std::max(hi, along_x ? p.x : p.y);
		}
	float start = along_x ? base.x : base.y;
	for (float dir : { 1.f, -1.f })
		for (int k = 1; k < MAX_REPEAT && int(b.elements.size()) < int(t.build_max_parts); k++) {
			float at = start + dir * float(k) * step;
			if (at < lo - EPS || at > hi + EPS) break;
			LotElement copy = base;
			(along_x ? copy.x : copy.y) = at;
			if (clash(b, copy, false)) break;
			b.elements.push_back(copy);
		}
	return "";
}

} // namespace city
