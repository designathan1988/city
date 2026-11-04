// The Street tab's commands (Roadcraft editor/streetscapeTool.ts, barriers.ts, bulldozer.ts, roads/streetKit.ts): placing an
// item or a row of them, a Stop or Give Way plate that becomes its approach's rule, a barrier, and taking away what stands
// at a point. What a road carries goes with it or moves with its new sidewalks.
#include "sim/commands/commands.h"
#include "sim/commands/street_journal.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint8_t CTL_PRIORITY_NODE = 1; // RoadNode::ctl of a junction by priority (junction_control.cpp)
constexpr int MIN_RULE_ARMS = 3;
constexpr double ROW_STEP_EPS_M = 0.002; // a row reaches its last station past the rounding of the sum

// The approach a Stop or Give Way plate at (x, z) governs ("placa = regra"), or -1: the node of the segment end it stands
// by, within signs.approach_reach_m of its mouth, where traffic arrives.
int sign_approach(const CityState &s, const FootwayHit &h) {
	const RoadNetwork &net = s.roads;
	const RoadSegment &g = net.segs[size_t(h.seg)];
	const Profile &p = net.profile_of(h.seg);
	const float reach = s.catalog.furniture.signs.approach_reach_m;
	int best = -1;
	double best_d = 0;
	for (int end = 0; end < 2; end++) {
		const int n = end == 0 ? g.a : g.b;
		const double d = end == 0 ? h.v - g.retreat_a : g.len - g.retreat_b - h.v;
		const bool arrives = end == 0 ? !p.back.empty() : !p.fwd.empty();
		if (int(net.nodes[size_t(n)].segs.size()) < MIN_RULE_ARMS || !arrives || d > reach || (best >= 0 && d >= best_d)) continue;
		best = n;
		best_d = d;
	}
	return best;
}

void set_rule(CityState &s, int seg, int n, uint16_t rule) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const uint16_t mask = g.a == n ? SEG_RULE_A_MASK : SEG_RULE_B_MASK;
	const int shift = g.a == n ? SEG_RULE_A_SHIFT : SEG_RULE_B_SHIFT;
	j_set_ctl(s, n, CTL_PRIORITY_NODE);
	j_set_flags(s, seg, uint16_t((g.flags & ~mask) | (rule << shift)));
}

// A street's name stands at the corners of its whole street: their chunks change with it.
void mark_name(CityState &s, const StreetItem &item) {
	const FootwayHit h = footway_at(s.roads, item.x, item.z, s.catalog.furniture.poles.footway_catch_m);
	if (!h.ok) return;
	for (int seg : street_chain(s.roads, h.seg)) mark_furniture(s, seg);
}

template <typename F>
void each_piece_on(const CityState &s, int seg, F &&fn) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const float pad = s.catalog.furniture.poles.footway_catch_m + s.catalog.furniture.items.stand_reach_m;
	const float reach = s.catalog.furniture.items.stand_reach_m;
	s.street.items_in(g.minx - pad, g.minz - pad, g.maxx + pad, g.maxz + pad, [&](int id) {
		const StreetItem &o = s.street.items.items[size_t(id)];
		const FootwayHit h = footway_at(s.roads, o.x, o.z, reach);
		if (h.ok && h.seg == seg) fn(id, -1, h);
	});
	s.street.poles_in(g.minx - pad, g.minz - pad, g.maxx + pad, g.maxz + pad, [&](int id) {
		const UtilityPole &o = s.street.poles.items[size_t(id)];
		const FootwayHit h = footway_at(s.roads, o.x, o.z, reach);
		if (h.ok && h.seg == seg) fn(-1, id, h);
	});
}
} // namespace

EditResult place_item(CityState &s, ItemKind kind, SignType sign, const std::string &text, double x, double z, double reach) {
	EditResult r;
	const ItemSnap snap = snap_item(s, kind, x, z, reach);
	if (!snap.ok) {
		r.error = snap.reason;
		return r;
	}
	if (kind == ItemKind::SIGN && (sign == SignType::STOP || sign == SignType::YIELD)) {
		const FootwayHit h = footway_at(s.roads, snap.x, snap.z, s.catalog.furniture.items.stand_reach_m);
		const int n = h.ok ? sign_approach(s, h) : -1;
		if (n >= 0) {
			set_rule(s, h.seg, n, sign == SignType::STOP ? RULE_STOP : RULE_YIELD);
			r.note = sign == SignType::STOP ? "hint.streetscape.rule.stop" : "hint.streetscape.rule.yield";
			r.ok = true;
			finish_edit(s, snap.x, snap.z, snap.x, snap.z);
			return r;
		}
	}
	StreetItem it;
	it.kind = kind;
	it.sign = sign;
	it.x = float(snap.x);
	it.z = float(snap.z);
	if (kind != ItemKind::SIGN || sign_has_text(sign)) it.text = text;
	j_add_item(s, it);
	if (kind == ItemKind::STREETNAME) mark_name(s, it);
	finish_edit(s, snap.x, snap.z, snap.x, snap.z);
	r.ok = true;
	return r;
}

EditResult place_item_row(CityState &s, ItemKind kind, double ax, double az, double bx, double bz, double every_m) {
	EditResult r;
	const double len = std::hypot(bx - ax, bz - az);
	const float reach = s.catalog.furniture.items.stand_reach_m;
	int placed = 0;
	for (double at = 0; at <= len + ROW_STEP_EPS_M; at += std::max(every_m, ROW_STEP_EPS_M)) {
		const double t = len > 0 ? at / len : 0;
		const ItemSnap snap = snap_item(s, kind, ax + (bx - ax) * t, az + (bz - az) * t, reach);
		if (!snap.ok) {
			if (r.error.empty()) r.error = snap.reason;
			continue;
		}
		StreetItem it;
		it.kind = kind;
		it.x = float(snap.x);
		it.z = float(snap.z);
		j_add_item(s, it);
		placed++;
	}
	if (placed > 0) {
		r.ok = true;
		r.error.clear();
		r.note = "hint.streetscape.row";
		finish_edit(s, std::min(ax, bx), std::min(az, bz), std::max(ax, bx), std::max(az, bz));
	}
	return r;
}

EditResult build_barrier(CityState &s, BarrierKind kind, const std::vector<float> &pts) {
	EditResult r;
	r.error = barrier_problem(s, kind, pts);
	if (!r.error.empty()) return r;
	Barrier b;
	b.kind = kind;
	b.pts = pts;
	j_add_barrier(s, b);
	finish_edit(s, pts[0], pts[1], pts[0], pts[1]);
	r.ok = true;
	return r;
}

bool remove_street_at(CityState &s, double x, double z, double reach) {
	const float fx = float(x), fz = float(z), fr = float(reach);
	const int item = s.street.item_near(fx, fz, fr);
	if (item >= 0) {
		const StreetItem it = s.street.items.items[size_t(item)];
		j_remove_item(s, item);
		if (it.kind == ItemKind::STREETNAME) mark_name(s, it);
		finish_edit(s, x, z, x, z);
		return true;
	}
	const int pole = s.street.pole_near(fx, fz, fr);
	if (pole >= 0) {
		j_remove_pole(s, pole);
		finish_edit(s, x, z, x, z);
		return true;
	}
	const int barrier = s.street.barrier_near(fx, fz, fr);
	if (barrier >= 0) {
		j_remove_barrier(s, barrier);
		finish_edit(s, x, z, x, z);
		return true;
	}
	// A piece of the street's own kit: it stays cut by its place.
	const FootwayHit h = footway_at(s.roads, x, z, reach);
	if (!h.ok) return false;
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, h.seg, FURNITURE_ALL & ~furniture_bit(FurnitureKind::SIGNAL), spots);
	double best = reach;
	bool found = false;
	double cx = 0, cz = 0;
	for (const FurnitureSpot &p : spots) {
		double px, pz, py;
		footway_point(s.roads, p.seg, p.v, p.u, px, pz, py);
		const double d = std::hypot(px - x, pz - z);
		if (d > best) continue;
		best = d;
		found = true;
		cx = px;
		cz = pz;
	}
	if (!found) return false;
	j_add_cut(s, float(cx), float(cz));
	mark_furniture(s, h.seg);
	finish_edit(s, x, z, x, z);
	return true;
}

void remove_street_kit(CityState &s, int seg) {
	std::vector<int> items, poles;
	each_piece_on(s, seg, [&](int item, int pole, const FootwayHit &) {
		if (item >= 0) items.push_back(item);
		if (pole >= 0) poles.push_back(pole);
	});
	for (int id : items) j_remove_item(s, id);
	for (int id : poles) j_remove_pole(s, id);
}

std::vector<CarriedPiece> street_kit_of(const CityState &s, int seg) {
	std::vector<CarriedPiece> kit;
	const Profile &p = s.roads.profile_of(seg);
	each_piece_on(s, seg, [&](int item, int pole, const FootwayHit &h) {
		CarriedPiece c;
		c.item = item;
		c.pole = pole;
		c.side = h.side;
		c.v = h.v;
		c.from_kerb = std::fabs(sidewalk_u(p, h.side, 0) - h.u);
		kit.push_back(c);
	});
	return kit;
}

void carry_street_kit(CityState &s, int seg, const std::vector<CarriedPiece> &kit) {
	const Profile &p = s.roads.profile_of(seg);
	for (const CarriedPiece &c : kit) {
		if (c.side < 0 || !sidewalk_on(p, c.side)) {
			if (c.item >= 0) j_remove_item(s, c.item);
			else j_remove_pole(s, c.pole);
			continue;
		}
		double x, z, y;
		footway_point(s.roads, seg, c.v, sidewalk_u(p, c.side, c.from_kerb), x, z, y);
		if (c.item >= 0) {
			StreetItem it = s.street.items.items[size_t(c.item)];
			it.x = float(x);
			it.z = float(z);
			j_remove_item(s, c.item);
			j_add_item(s, it);
		} else { // a pole keeps its wires: strung again to the same neighbours
			UtilityPole moved = s.street.poles.items[size_t(c.pole)];
			std::vector<int> others;
			for (int span : moved.spans) {
				const UtilitySpan &sp = s.street.spans.items[size_t(span)];
				others.push_back(sp.a == c.pole ? sp.b : sp.a);
			}
			moved.spans.clear();
			moved.x = float(x);
			moved.z = float(z);
			j_remove_pole(s, c.pole);
			const int id = j_add_pole(s, moved);
			for (int o : others) j_add_span(s, id, o);
		}
	}
}

} // namespace city
