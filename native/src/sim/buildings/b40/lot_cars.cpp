// Where a house's car stands and how the side strips are used. Every house gets its bay: in a closed garage or under a
// carport standing on its own drive - at the drive's front, or at its end in the yard with the drive carried there -
// or, on a lot too narrow for a drive, on a car pad in the front garden. The garage is a volume of the body on the
// drive's strip with its door on the drive, so the car drives straight in; on an uphill lot it is dug into the slope at the
// drive's front, its door at the street's level (LotSpace::dug). Other lots' drives get their drain, their
// side paths a lamp, drains, bins and crates (an alley used as one), side gardens a row of trees and shrubs.
#include "sim/buildings/b40/lot_layout.h"
#include "sim/buildings/b40/massing.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

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
	const LotCarsShape &K = s.data.shapes.cars;
	const LotPartShape &P = s.data.shapes.parts;
	const LotRules &l = s.rules;
	const int line = s.paint("stall_line");
	if (!s.dug.empty()) { // the car drives straight in from the street, under the house's level
		s.flat(EL_STALL, Rect::around(s.dug.cx(), s.dug.y0 + l.stall_d * HALF, l.stall_w, l.stall_d), 0, line, FACE_STREET);
		s.edge(EL_DUG_GARAGE, s.dug, l.car_clear_h, s.paint("concrete"));
		s.built.push_back(s.dug);
		return;
	}
	Rect pad = car_pad(s);
	if (!pad.empty()) {
		s.cover(pad, "concrete", 2);
		s.flat(EL_STALL, Rect::around(pad.cx(), s.F * HALF, l.stall_w, std::min(l.stall_d, s.F)), 0, line, FACE_STREET);
		s.keep_clear(pad);
		return;
	}
	const Strip *st = drive_strip(s);
	if (!st) return;
	const float len = l.stall_d + K.garage_wall, yard = s.D - s.B;
	const bool back_fits = yard >= len + 2 * K.garage_set, front_fits = s.B - s.F >= len + K.garage_set;
	const bool back = back_fits && (!front_fits || s.r.f() < l.garage_back_share);
	if (!back && !front_fits) return;
	const bool closed = s.r.f() < l.garage_closed_share;
	const float y0 = back ? s.B + K.garage_set : s.F + K.garage_set;
	const Rect shed{ st->drive.x0, y0, st->drive.x1, y0 + len };
	if (back) {
		// The drive carried on past the house to the garage at its end.
		s.cover({ shed.x0, s.B, shed.x1, y0 }, "concrete", 1);
		s.keep_clear({ shed.x0 + P.way_side, s.B, shed.x1 - P.way_side, y0 });
	}
	s.flat(EL_STALL, Rect::around(shed.cx(), y0 + l.stall_d * HALF, l.stall_w, l.stall_d), 0, line, FACE_STREET);
	s.flat(EL_DRAIN, Rect::around(shed.cx(), y0 - K.drain_before, P.drain, P.drain), P.drain_h, s.paint("grille"));
	if (closed) {
		garage(s, shed);
		return;
	}
	const int post = s.paint("lamp_post"), roof = s.paint("canopy");
	for (float x : { shed.x0 + K.carport_post, shed.x1 - K.carport_post })
		for (float y : { shed.y0 + K.carport_post, shed.y1 - K.carport_post })
			s.edge(EL_PILLAR, Rect::around(x, y, K.carport_post, K.carport_post), l.car_clear_h, post);
	LotElement slab;
	slab.kind = EL_SLAB; slab.x = shed.cx(); slab.y = shed.cy(); slab.w = shed.w(); slab.d = shed.d();
	slab.z = l.car_clear_h; slab.h = K.carport_slab; slab.paint = roof;
	s.b.elements.push_back(slab);
}

void alley(LotSpace &s, const Rect &w) {
	const LotCarsShape &K = s.data.shapes.cars;
	const LotPartShape &P = s.data.shapes.parts;
	const float len = w.d();
	if (len > K.alley_lamp_min) s.put(EL_LAMP, Rect::around(w.x0 + P.lamp, w.cy(), P.lamp, P.lamp), K.alley_lamp_h, s.paint("lamp_post"));
	for (float y = w.y0 + K.drain_every * HALF; y < w.y1 - 1; y += K.drain_every)
		s.flat(EL_DRAIN, Rect::around(w.cx(), y, K.alley_drain, K.alley_drain), P.drain_h, s.paint("grille"));
	const float x = w.x1 - K.bin_d * HALF - P.lamp;
	s.put(EL_BIN, Rect::around(x, w.y1 - K.back_end, K.bin_d, K.bin_w), K.bin_h, s.paint("bin"));
	s.put(EL_PALLET, Rect::around(x, w.y1 - K.back_end - K.crate_gap, K.crate, K.crate), K.crate_h, s.paint("shed"));
}

void side_garden(LotSpace &s, const Rect &g) {
	const LotCarsShape &K = s.data.shapes.cars;
	bool tree = true;
	for (float y = g.y0 + K.side_tree_every * HALF; y < g.y1 - K.side_tree * HALF; y += K.side_tree_every, tree = !tree)
		s.put(tree ? EL_TREE : EL_SHRUB, Rect::around(g.cx(), y, K.side_tree, K.side_tree), tree ? K.side_tree_h : K.side_shrub_h, s.paint("hedge"));
}
} // namespace

Rect car_pad(const LotSpace &s) {
	const LotCarsShape &K = s.data.shapes.cars;
	if (s.plan.front != FRONT_CARPAD || s.b.kind != KIND_HOUSE) return {};
	const float w = s.rules.stall_w + 2 * K.pad_margin;
	// On the side away from the door, clear of the side wall.
	return s.door_x > (s.L + s.R) * HALF ? Rect{ s.L + K.pad_margin, 0, s.L + K.pad_margin + w, s.F }
										 : Rect{ s.R - K.pad_margin - w, 0, s.R - K.pad_margin, s.F };
}

void lay_cars(LotSpace &s) {
	const LotCarsShape &K = s.data.shapes.cars;
	const LotPartShape &P = s.data.shapes.parts;
	const bool house = s.b.kind == KIND_HOUSE;
	if (house) house_car(s);
	for (const Strip &st : s.strips) {
		if (!house && !st.drive.empty() && s.B - s.F > K.drive_drain_at)
			s.flat(EL_DRAIN, Rect::around(st.drive.cx(), s.F + K.drive_drain_at, P.drain, P.drain), P.drain_h, s.paint("grille"));
		if (!house && st.use == SIDE_PATH) alley(s, st.walk);
		if (st.use == SIDE_GARDEN) side_garden(s, st.r);
	}
}

} // namespace city::b40
