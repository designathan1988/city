// The lot's answer to a split level (Roadcraft world/growth/lotPlan.ts furnishLot on a hill: lift, retain, flightBack, the garage's
// apron, stepYard's wall and flight; zoning.ts tryOnLot's choice of when to step and when a garage goes under): the back of the lot
// at the back's level, retaining walls where the levels meet, a flight down a passage, the apron before a garage under the house,
// and a back yard's own terrace wall and flight. The lot's walls and flights stand on the ground the render reads under them
// (render/buildings/lot_emit.cpp wall, lot_parts.cpp stair), so each is measured from the site's ground at its place.
#include "sim/buildings/b40/split_level.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city::b40 {

namespace {
constexpr float MIDDLE = 0.5f; // a piece stands in the middle of its share of a run
constexpr int APRON_LAYER = 2; // the apron over the drive's cover (layer 1), as a car pad lies (lot_cars.cpp)

bool boundary(Element k) { return k == EL_WALL || k == EL_FENCE || k == EL_HEDGE || k == EL_RAILING || k == EL_GATE || k == EL_CAR_GATE; }
bool on_face(Element k) { return k == EL_CANOPY || k == EL_AC || k == EL_AWNING || k == EL_CLOCK; } // moved with its block (split_level.cpp)

struct LotAnswer {
	const Data &data;
	Body &b;
	const LotSite &site;
	float W, D;
	const LotSlopeShape &K = data.shapes.slope;
	int stone = -1;

	void add(Element k, float x, float y, float w, float d, float z, float h, int facing = FACE_STREET, float base = 0) {
		if (int(b.elements.size()) >= data.lots.max_elements) return;
		if (stone < 0) stone = b.add_paint(data.color("civic_stone"));
		LotElement e;
		e.kind = k; e.x = x; e.y = y; e.w = w; e.d = d; e.z = z; e.h = h; e.facing = facing; e.base = base;
		e.paint = stone;
		b.elements.push_back(e);
	}
	// A wall piece standing on the ground at (x, y) up to `top` over the floor, a hand's breadth over the ground at the least.
	void wall_to(float x, float y, float w, float d, float top) { add(EL_WALL, x, y, w, d, 0, std::max(K.retain_over, top - site.ground_at(x, y, W, D))); }
	// A wall in pieces from (x0, y0) to (x1, y1), across or along the lot, up to `top` (lotPlan.ts retain).
	void run(float x0, float y0, float x1, float y1, float top) {
		const bool along_y = std::fabs(x1 - x0) < std::fabs(y1 - y0);
		const float length = along_y ? y1 - y0 : x1 - x0;
		const int n = int(std::ceil(length / K.wall_piece));
		for (int k = 0; k < n && length > K.retain_t; k++) {
			const float a = (float(k) + MIDDLE) / float(n);
			if (along_y) wall_to(x0, y0 + (y1 - y0) * a, K.retain_t, (y1 - y0) / float(n), top);
			else wall_to(x0 + (x1 - x0) * a, y0, (x1 - x0) / float(n), K.retain_t, top);
		}
	}
	// A retaining wall between the street floor's level and `lift`: to the higher level and a hand's breadth over it, or a guard's
	// height over the street floor above a drop.
	void retain(float x0, float y0, float x1, float y1, float lift) { run(x0, y0, x1, y1, lift > 0 ? lift + K.retain_over : K.guard_h); }
	// A flight `width` wide at `x`, its foot at `y`, from level `from` to level `to` over the floor (lotPlan.ts flightBack).
	float flight(float x, float y, float width, float from, float to) {
		const int risers = std::max(SLOPE_WHOLE, int(std::lround(std::fabs(to - from) / RISER_M)));
		const float run_d = float(risers + 1) * K.step_run, cy = y + run_d / 2, g = site.ground_at(x, cy, W, D);
		add(EL_STAIR, x, cy, width, run_d, std::min(from, to), std::fabs(to - from), to > from ? FACE_STREET : FACE_BACK, std::min(from, to) - g);
		return run_d;
	}
};

struct Drive {
	bool left = false, any = false;
	float x0 = 0, x1 = 0; // across the lot (lot frame)
};
Drive drive_of(const LotPlan &p, float W) {
	Drive d;
	const bool l = p.left.use == SIDE_DRIVE || p.left.use == SIDE_DRIVE_PATH, r = p.right.use == SIDE_DRIVE || p.right.use == SIDE_DRIVE_PATH;
	d.any = l || r;
	d.left = l;
	d.x0 = l ? -W / 2 : W / 2 - p.right.width;
	d.x1 = l ? -W / 2 + p.left.width : W / 2;
	return d;
}

// What stands behind the step goes to the back's level with it (lotPlan.ts lift); a cover across the line in two. The drive keeps
// the street floor to its end, the boundary stands on the ground.
void lift_behind(Body &b, const SplitStep &s, const Drive &drive, float B) {
	std::vector<LotElement> extra;
	for (LotElement &e : b.elements) {
		if (boundary(e.kind) || on_face(e.kind) || (drive.any && e.x > drive.x0 && e.x < drive.x1 && e.y < B)) continue;
		const float y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
		if (e.kind == EL_SURFACE && y0 < s.y - EPS && y1 > s.y + EPS) {
			LotElement back = e;
			back.y = (s.y + y1) / 2;
			back.d = y1 - s.y;
			back.z += s.lift;
			extra.push_back(back);
			e.y = (y0 + s.y) / 2;
			e.d = s.y - y0;
			continue;
		}
		if (e.y >= s.y) e.z += s.lift;
	}
	b.elements.insert(b.elements.end(), extra.begin(), extra.end());
}

// The side strips at the step: a wall across a side garden, a flight down a passage, a wall across the drive's end where the back
// is at its own level (none where it goes on down as a ramp); the earth held along the building's sides behind the step.
void side_strips(LotAnswer &a, const LotPlan &p, const SplitStep &s, float B, bool ramped) {
	const LotSlopeShape &K = a.K;
	const float W = a.W;
	for (const bool left : { true, false }) {
		const SidePlan &sp = left ? p.left : p.right;
		if (sp.width <= 0) continue;
		const float x0 = left ? -W / 2 : W / 2 - sp.width, x1 = left ? -W / 2 + sp.width : W / 2;
		if (sp.use == SIDE_GARDEN && s.y > p.front_depth + K.ramp_min && s.y < B - K.ramp_min) a.retain(x0 + K.retain_in, s.y, x1 - K.retain_in, s.y, s.lift);
		if (sp.use == SIDE_PATH && s.y > p.front_depth + K.walk_clear && s.y < B - K.walk_clear)
			a.flight((x0 + x1) / 2, s.y, std::max(K.flight_w_min, x1 - x0 - K.flight_trim), 0, s.lift);
		if ((sp.use == SIDE_DRIVE || sp.use == SIDE_DRIVE_PATH) && a.D - B > 0 && !ramped)
			a.retain(x0 + K.retain_in, B + K.retain_in, x1 - K.retain_in, B + K.retain_in, s.lift);
		if (sp.use != SIDE_ATTACHED) {
			const float x = left ? p.x0 - W / 2 - K.retain_in : p.x1 - W / 2 + K.retain_in;
			a.retain(x, s.y, x, B, s.lift);
		}
	}
}

// The apron behind the house at the garage's floor, where a car turns into it (lotPlan.ts GARAGE_APRON); returns where the garden
// beyond it starts.
float garage_apron(LotAnswer &a, const SplitStep &s, const Drive &drive, V2 door, float door_w, float from) {
	const LotSlopeShape &K = a.K;
	Body &b = a.b;
	if (a.D - door.y < K.garage_apron + K.apron_spare) return from;
	const float ax0 = std::min(drive.x0, door.x - door_w / 2), ax1 = std::max(drive.x1, door.x + door_w / 2 + K.garage_door_out);
	const float ay0 = door.y, ay1 = door.y + K.garage_apron;
	b.elements.erase(std::remove_if(b.elements.begin(), b.elements.end(), [&](const LotElement &e) {
		return !boundary(e.kind) && e.kind != EL_SURFACE && e.x > ax0 && e.x < ax1 && e.y > ay0 && e.y < ay1;
	}), b.elements.end());
	const LotPartShape &P = a.data.shapes.parts;
	LotElement apron;
	apron.kind = EL_SURFACE; apron.x = (ax0 + ax1) / 2; apron.y = (ay0 + ay1) / 2; apron.w = ax1 - ax0; apron.d = ay1 - ay0;
	apron.z = s.lift; apron.h = P.cover_h + a.data.shapes.space.cover_step * float(APRON_LAYER);
	apron.paint = b.add_paint(a.data.color("concrete"));
	b.elements.push_back(apron);
	LotElement drain = apron;
	drain.kind = EL_DRAIN; drain.y = ay0 + K.drain_in; drain.w = drain.d = P.drain; drain.h = P.drain_h;
	drain.paint = b.add_paint(a.data.color("grille"));
	b.elements.push_back(drain);
	return std::max(from, ay1);
}

// A house's back yard on a hillside: the garden beyond the terrace a platform of its own, its wall and flight where it starts
// (lotPlan.ts stepYard), when the land falls or rises on from the back's level far enough for whole risers.
void step_yard(LotAnswer &a, const LotPlan &p, const SplitStep &s, float from) {
	const LotSlopeShape &K = a.K;
	const float W = a.W, D = a.D;
	if (p.back != BACK_YARD || p.back_depth <= 0) return;
	const float fall = a.site.ground_at(0, D - p.back_depth / 2, W, D) - s.lift;
	if (std::fabs(fall) < K.terrace_min) return;
	const int risers = std::min(K.terrace_risers_max, int(std::lround(std::fabs(fall) * K.terrace_share / RISER_M)));
	if (risers < K.terrace_risers_min) return;
	const float rise = float(risers) * RISER_M * (fall > 0 ? 1.f : -1.f), run = float(risers + 1) * K.step_run;
	if (D - from < run + K.ramp_min || W < K.terrace_lot_min) return;
	const float sx = (p.x0 + p.x1) / 2 - W / 2;
	Body &b = a.b;
	b.elements.erase(std::remove_if(b.elements.begin(), b.elements.end(), [&](const LotElement &e) { // the flight's place and the wall's line
		if (boundary(e.kind) || e.kind == EL_SURFACE || e.y < from - K.stair_clear) return false;
		const float half = std::max(e.w, e.d) / 2;
		return std::fabs(e.y - from) < half + K.stair_clear || (std::fabs(e.x - sx) < half + K.stair_w && e.y < from + run + K.stair_clear);
	}), b.elements.end());
	for (LotElement &e : b.elements)
		if (!boundary(e.kind) && e.y >= from) e.z += rise;
	a.flight(sx, from, K.stair_w, s.lift, s.lift + rise);
	const float top = rise > 0 ? s.lift + rise + K.retain_over : s.lift + K.guard_h, y = from - K.retain_in;
	a.run(-W / 2 + K.retain_in, y, sx - K.stair_w / 2 - K.retain_in, y, top);
	a.run(sx + K.stair_w / 2 + K.retain_in, y, W / 2 - K.retain_in, y, top);
}
} // namespace

void follow_slope(const Data &data, Body &b, const LotSite &site, float W, float D) {
	if (!site.sloped || b.volumes.empty()) return;
	const LotSlopeShape &K = data.shapes.slope;
	const LotPlan &p = b.plan;
	// A car park or a loading yard behind goes to the back's level only as far as its drive's ramp reaches; with no ramp, no step.
	const bool yard_back = p.back == BACK_PARKING || p.back == BACK_LOADING;
	DriveRamp ramp;
	const bool ramped = yard_back && yard_ramp(K, p, ramp);
	if (yard_back && !ramped) return;
	const Rect yard{ -W / 2, ramp.end, W / 2, D };
	SplitStep s;
	if (!step_to_slope(data, b, site, W, D, ramped ? &yard : nullptr, ramped ? ramp.reach() : K.max_lift, s)) return;
	// A house a whole storey down with a drive: its garage in the lower ground floor on the drive's side, when the drive can go down
	// that far as a ramp, the yard has room for the apron and no garage stands on the lot already.
	bool garaged = false;
	for (const Volume &v : b.volumes)
		for (const auto &e : v.bays) garaged = garaged || e.second == BAY_GARAGE_DOOR;
	const Drive drive = drive_of(p, W);
	DriveRamp down;
	V2 door;
	float door_w = 0;
	const bool garage = s.steps == -SLOPE_WHOLE && drive.any && !garaged && garage_ramp(K, p, down) && p.back_depth >= K.garage_back_min &&
			-s.lift <= down.reach() && garage_under(data, b, drive.left, door, door_w);
	LotAnswer a{ data, b, site, W, D };
	const float B = D - p.back_depth;
	lift_behind(b, s, drive, B);
	side_strips(a, p, s, B, ramped || garage);
	const float from = garage ? garage_apron(a, s, drive, door, door_w, B + K.terrace_from) : B + K.terrace_from;
	step_yard(a, p, s, from);
}

} // namespace city::b40
