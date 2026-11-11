// LotSpace: the lot in its own frame (edges, zones, side strips, ground volumes, front, back and loading doors) and the
// rule every part obeys - nothing solid in a way kept clear, on the building, on another part or across the lot's edge.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cstring>

namespace city::b40 {

namespace {
constexpr float LOT_CLEAR_IN = 0.15f; // a part keeps this far inside the lot's edge (the boundary stands on it)
constexpr float PART_GAP = 0.05f; // two solid parts never touch
constexpr uint32_t BYTE = 0xff;
constexpr int STOREY_SHIFT = KEY_STOREY_SHIFT, SIDE_SHIFT = KEY_SIDE_SHIFT, BACK_SIDE = 2;
constexpr float HALF = 0.5f;
constexpr float GATE_STEP = 0.1f; // a people's gate moved off the door's line is looked for along the front this often

Rect footprint(const Volume &v) { return { v.x, v.y, v.x + v.w, v.y + v.d }; }
} // namespace

LotSpace::LotSpace(const Data &d, Body &body, Xoshiro128 &rng) : data(d), b(body), r(rng), rules(d.lots), plan(body.plan) {
	L = -plan.W / 2;
	R = plan.W / 2;
	D = plan.D;
	F = plan.front_depth;
	B = D - plan.back_depth;
	for (int s = 0; s < 2; s++) {
		const SidePlan &side = s == 0 ? plan.left : plan.right;
		Strip &st = strips[s];
		st.left = s == 0;
		st.use = side.use;
		float x0 = s == 0 ? L : R - side.width, x1 = s == 0 ? L + side.width : R;
		st.r = { x0, F, x1, B };
		if (side.use == SIDE_DRIVE || side.use == SIDE_DRIVE_PATH) {
			float walk = side.use == SIDE_DRIVE_PATH ? side.walk : 0;
			st.drive = s == 0 ? Rect{ x0, 0, x1 - walk, B } : Rect{ x0 + walk, 0, x1, B };
			if (walk > 0) st.walk = s == 0 ? Rect{ x1 - walk, F, x1, B } : Rect{ x0, F, x0 + walk, B };
		} else if (side.use == SIDE_PATH) {
			st.walk = st.r;
		}
	}
	for (const Volume &v : b.volumes) {
		if (v.base != 0) continue;
		Rect f = footprint(v);
		built.push_back(f);
		built_box = built_box.empty() ? f : Rect{ std::min(built_box.x0, f.x0), std::min(built_box.y0, f.y0), std::max(built_box.x1, f.x1), std::max(built_box.y1, f.y1) };
		int n = bays_of(v, BACK_SIDE);
		for (const auto &bay : v.bays) {
			int storey = int(bay.first >> STOREY_SHIFT), side = int((bay.first >> SIDE_SHIFT) & BYTE), index = int(bay.first & BYTE);
			if (storey != 0 || side != BACK_SIDE) continue;
			float x = v.x + v.w - (float(index) + HALF) * v.w / float(n); // the back face runs from +x to -x
			if (bay.second == BAY_DOOR || bay.second == BAY_DOUBLE_DOOR) { back_door = true; back_door_x = x; }
			if (bay.second == BAY_LOADING_DOOR) { loading_x.push_back(x); loading_y = v.y + v.d; }
		}
	}
	std::sort(loading_x.begin(), loading_x.end());
	own_parts = built.size();
	door_x = b.entrance_x;
	door_wall = F;
	bool walled = false;
	for (const Rect &f : built)
		if (door_x >= f.x0 && door_x <= f.x1 && (!walled || f.y0 < door_wall)) {
			door_wall = f.y0;
			walled = true;
		}
	gate_x = people_gate_x();
}

float LotSpace::people_gate_x() const {
	if (F <= 0) return door_x; // no front zone: no people's gate
	const float g = rules.person_gate[b.kind == KIND_HOUSE ? 0 : 1] * HALF;
	if (!plan.edges.shut(LotEdges::WALK, door_x - g, door_x + g)) return door_x;
	const auto opens = [&](float x) {
		if (x - g < L + HEDGE_THICK || x + g > R - HEDGE_THICK || plan.edges.shut(LotEdges::WALK, x - g, x + g)) return false;
		for (const Strip &st : strips)
			if (!st.drive.empty() && x - g < st.drive.x1 && st.drive.x0 < x + g) return false;
		return true;
	};
	for (float d = GATE_STEP; d < R - L; d += GATE_STEP)
		for (float x : { door_x - d, door_x + d })
			if (opens(x)) return x;
	return door_x;
}

std::vector<Rect> LotSpace::people_way(float half, float depth) const {
	if (gate_x == door_x) return { { door_x - half, 0, door_x + half, depth } };
	const float link = door_wall * HALF;
	return { { gate_x - half, 0, gate_x + half, link - half }, { std::min(gate_x, door_x) - half, link - half, std::max(gate_x, door_x) + half, link + half },
		{ door_x - half, link + half, door_x + half, depth } };
}

int LotSpace::paint(const char *name) {
	for (const auto &p : paints_)
		if (std::strcmp(p.first, name) == 0) return p.second;
	int i = b.add_paint(data.color(name));
	paints_.push_back({ name, i });
	return i;
}

void LotSpace::add(Element k, const Rect &rc, float h, int p, int facing, float z, float base) {
	LotElement e;
	e.kind = k; e.x = rc.cx(); e.y = rc.cy(); e.w = rc.w(); e.d = rc.d(); e.h = h; e.z = z; e.paint = p;
	e.facing = facing; e.base = base;
	b.elements.push_back(e);
}

void LotSpace::cover(const Rect &rc, const char *name, CoverLayer layer) {
	Rect c{ std::max(rc.x0, L), std::max(rc.y0, 0.f), std::min(rc.x1, R), std::min(rc.y1, D) };
	if (c.empty() || full()) return;
	add(EL_SURFACE, c, cover_h(layer), paint(name), FACE_STREET, 0, 0);
}

bool LotSpace::on_building(const Rect &rc) const {
	for (const Rect &f : built)
		if (f.meets(rc)) return true;
	return false;
}

bool LotSpace::clear(const Rect &rc) const {
	if (rc.x0 < L + LOT_CLEAR_IN || rc.x1 > R - LOT_CLEAR_IN || rc.y0 < LOT_CLEAR_IN || rc.y1 > D - LOT_CLEAR_IN) return false;
	if (on_building(rc)) return false;
	for (const Rect &w : ways_)
		if (w.meets(rc)) return false;
	Rect g = rc.grown(PART_GAP);
	for (const Rect &s : solid_)
		if (s.meets(g)) return false;
	return true;
}

bool LotSpace::put(Element k, const Rect &rc, float h, int p, int facing, float z) {
	if (full() || rc.empty() || !clear(rc)) return false;
	add(k, rc, h, p, facing, z, 0);
	solid_.push_back(rc);
	return true;
}

bool LotSpace::flat(Element k, const Rect &rc, float h, int p, int facing) {
	if (full() || rc.empty() || on_building(rc)) return false;
	if (rc.x0 < L || rc.x1 > R || rc.y0 < 0 || rc.y1 > D) return false;
	add(k, rc, h, p, facing, 0, 0);
	if (k == EL_STALL || k == EL_DOCK) ways_.push_back(rc);
	return true;
}

void LotSpace::edge(Element k, const Rect &rc, float h, int p, float base, float z) {
	if (rc.empty()) return;
	add(k, rc, h, p, FACE_STREET, z, base);
	solid_.push_back(rc);
}

float LotSpace::built_back(float x0, float x1) const {
	float back = F;
	for (const Rect &f : built)
		if (f.x0 < x1 && x0 < f.x1) back = std::max(back, f.y1);
	return back;
}

void lay_out_lot(const Data &data, Body &b, Xoshiro128 &r) {
	LotSpace s(data, b, r);
	lay_ground(s);
	open_boundary(s);
	lay_cars(s);
	lay_front(s);
	lay_back(s);
	lay_yard(s);
	lay_boundary(s); // last: every block that stands on an edge (a garage) is known
}

} // namespace city::b40
