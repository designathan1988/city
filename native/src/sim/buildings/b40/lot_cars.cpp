// Where a house's car stands and how the side strips are used. Every house gets its bay: in a closed garage or under a
// carport standing on its own drive - at the drive's front, or at its end in the yard with the drive carried there -
// or, on a lot too narrow for a drive, on a car pad in the front garden. The garage is a volume of the body on the
// drive's strip with its door on the drive, so the car drives straight in; standing on the lot's side, its wall closes that
// stretch of the boundary (laid after it, lot_boundary.cpp). A carport stands inside the boundary's band, the wall along it.
// Other lots' drives get their drain, their
// side paths a lamp, drains, bins and crates (an alley used as one), side gardens a row of trees and shrubs.
#include "sim/buildings/b40/lot_layout.h"
#include "sim/buildings/b40/massing.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float GARAGE_WALL = 0.2f; // a garage's back wall behind its bay (the door stands at the bay's front)
constexpr float GARAGE_SET = 0.5f; // a garage stands this far behind the front garden, or behind the house in the yard
constexpr float WAY_SIDE = 0.3f; // a car's way is its drive less this each side (lot_ground.cpp)
constexpr float CARPORT_POST = 0.15f, CARPORT_SLAB = 0.15f; // a carport's posts and its roof slab, over the car's headroom
constexpr float PAD_MARGIN = 0.3f; // a car pad clears the side wall and the bay's edges by this
constexpr float DRAIN = 0.5f, DRAIN_H = 0.02f, DRAIN_BEFORE = 1.f, DRIVE_DRAIN_AT = 7.f;
constexpr float ALLEY_LAMP_MIN = 12.f, LAMP = 0.3f, ALLEY_LAMP_H = 3.5f, DRAIN_EVERY = 5.f, ALLEY_DRAIN = 0.35f;
constexpr float BIN_W = 1.1f, BIN_D = 0.6f, BIN_H = 1.1f, CRATE = 0.6f, CRATE_H = 0.5f, BACK_END = 2.f, CRATE_GAP = 1.4f;
constexpr float SIDE_TREE_EVERY = 5.f, SIDE_TREE = 2.4f, SIDE_TREE_H = 4.f, SIDE_SHRUB_H = 1.4f, HALF = 0.5f;

const Strip *drive_strip(const LotSpace &s) {
	for (const Strip &st : s.strips)
		if (!st.drive.empty()) return &st;
	return nullptr;
}

// A closed garage over `bay`: a one-storey volume on the drive's strip, its door the whole front bay.
void garage(LotSpace &s, const Rect &shed) {
	int v = add_block(s.b, { shed.x0, shed.y0, shed.w(), shed.d(), 0, 1, ROOF_FLAT, BAY_WALL, BAY_NONE, nullptr });
	Volume &g = s.b.volumes[size_t(v)];
	g.keep = true; // not composed: plain walls round its one door
	g.faces[0].bays = 1;
	g.bays[bay_key(0, 0, 0)] = BAY_GARAGE_DOOR;
	g.roof_paint = s.paint("membrane");
	s.built.push_back(shed);
}

void house_car(LotSpace &s) {
	const LotRules &l = s.rules;
	const int line = s.paint("stall_line");
	Rect pad = car_pad(s);
	if (!pad.empty()) {
		s.cover(pad, "concrete", COVER_OWN);
		s.flat(EL_STALL, Rect::around(pad.cx(), s.F * HALF, l.stall_w, std::min(l.stall_d, s.F)), 0, line, FACE_STREET);
		s.keep_clear(pad);
		return;
	}
	const Strip *st = drive_strip(s);
	if (!st) return;
	const float len = l.stall_d + GARAGE_WALL, yard = s.D - s.B;
	// In front, a garage stands behind the front zone and behind the boundary's reach of the street line: its door never closes
	// the edge the drive's gate opens.
	const float front = std::max(s.F, block_reach(s.rules));
	const bool back_fits = yard >= len + 2 * GARAGE_SET, front_fits = s.B - front >= len + GARAGE_SET;
	const bool back = back_fits && (!front_fits || s.r.f() < l.garage_back_share);
	if (!back && !front_fits) return;
	const bool closed = s.r.f() < l.garage_closed_share;
	const float y0 = back ? s.B + GARAGE_SET : front + GARAGE_SET;
	const Rect shed{ st->drive.x0, y0, st->drive.x1, y0 + len };
	if (back) {
		// The drive carried on past the house to the garage at its end.
		s.cover({ shed.x0, s.B, shed.x1, y0 }, "concrete", COVER_WAY);
		s.keep_clear({ shed.x0 + WAY_SIDE, s.B, shed.x1 - WAY_SIDE, y0 });
	}
	s.flat(EL_STALL, Rect::around(shed.cx(), y0 + l.stall_d * HALF, l.stall_w, l.stall_d), 0, line, FACE_STREET);
	s.flat(EL_DRAIN, Rect::around(shed.cx(), y0 - DRAIN_BEFORE, DRAIN, DRAIN), DRAIN_H, s.paint("grille"));
	if (closed) {
		garage(s, shed);
		return;
	}
	// A carport is open: the side wall runs along it, and its posts and roof stand on the land inside the boundary's band.
	const Rect in = s.inner();
	const Rect port{ std::max(shed.x0, in.x0), shed.y0, std::min(shed.x1, in.x1), std::min(shed.y1, in.y1) };
	const int post = s.paint("lamp_post"), roof = s.paint("canopy");
	for (float x : { port.x0 + CARPORT_POST, port.x1 - CARPORT_POST })
		for (float y : { port.y0 + CARPORT_POST, port.y1 - CARPORT_POST })
			s.edge(EL_PILLAR, Rect::around(x, y, CARPORT_POST, CARPORT_POST), l.car_clear_h, post);
	LotElement slab;
	slab.kind = EL_SLAB; slab.x = port.cx(); slab.y = port.cy(); slab.w = port.w(); slab.d = port.d();
	slab.z = l.car_clear_h; slab.h = CARPORT_SLAB; slab.paint = roof;
	s.b.elements.push_back(slab);
}

void alley(LotSpace &s, const Rect &w) {
	const float len = w.d();
	if (len > ALLEY_LAMP_MIN) s.put(EL_LAMP, Rect::around(w.x0 + LAMP, w.cy(), LAMP, LAMP), ALLEY_LAMP_H, s.paint("lamp_post"));
	for (float y = w.y0 + DRAIN_EVERY * HALF; y < w.y1 - 1; y += DRAIN_EVERY)
		s.flat(EL_DRAIN, Rect::around(w.cx(), y, ALLEY_DRAIN, ALLEY_DRAIN), DRAIN_H, s.paint("grille"));
	const float x = w.x1 - BIN_D * HALF - LAMP;
	s.put(EL_BIN, Rect::around(x, w.y1 - BACK_END, BIN_D, BIN_W), BIN_H, s.paint("bin"));
	s.put(EL_PALLET, Rect::around(x, w.y1 - BACK_END - CRATE_GAP, CRATE, CRATE), CRATE_H, s.paint("shed"));
}

void side_garden(LotSpace &s, const Rect &g) {
	bool tree = true;
	for (float y = g.y0 + SIDE_TREE_EVERY * HALF; y < g.y1 - SIDE_TREE * HALF; y += SIDE_TREE_EVERY, tree = !tree)
		s.put(tree ? EL_TREE : EL_SHRUB, Rect::around(g.cx(), y, SIDE_TREE, SIDE_TREE), tree ? SIDE_TREE_H : SIDE_SHRUB_H, s.paint("hedge"));
}
} // namespace

Rect car_pad(const LotSpace &s) {
	// a people's way bent along the front garden (its gate off a crosswalk's landing) leaves no room in it for a pad
	if (s.plan.front != FRONT_CARPAD || s.b.kind != KIND_HOUSE || s.gate_x != s.door_x) return {};
	const float w = s.rules.stall_w + 2 * PAD_MARGIN;
	const Rect left{ s.L + PAD_MARGIN, 0, s.L + PAD_MARGIN + w, s.F }, right{ s.R - PAD_MARGIN - w, 0, s.R - PAD_MARGIN, s.F };
	const bool door_right = s.door_x > (s.L + s.R) * HALF;
	// On the side away from the door, clear of the side wall; on the door's side when the other's gate would open onto a junction's
	// kerb and this one's opens off it and off the person's gate; none when neither does.
	const Rect &away = door_right ? left : right, &near = door_right ? right : left;
	if (!s.plan.edges.shut(LotEdges::CAR, away.x0, away.x1)) return away;
	const float gate = s.rules.person_gate[0] * HALF;
	if (!s.plan.edges.shut(LotEdges::CAR, near.x0, near.x1) && (near.x1 < s.door_x - gate || near.x0 > s.door_x + gate)) return near;
	return {};
}

void lay_cars(LotSpace &s) {
	const bool house = s.b.kind == KIND_HOUSE;
	if (house) house_car(s);
	for (const Strip &st : s.strips) {
		if (!house && !st.drive.empty() && s.B - s.F > DRIVE_DRAIN_AT)
			s.flat(EL_DRAIN, Rect::around(st.drive.cx(), s.F + DRIVE_DRAIN_AT, DRAIN, DRAIN), DRAIN_H, s.paint("grille"));
		if (!house && st.use == SIDE_PATH) alley(s, st.walk);
		if (st.use == SIDE_GARDEN) side_garden(s, st.r);
	}
}

} // namespace city::b40
