// The Street tab's commands (Roadcraft editor/streetscapeTool.ts, barriers.ts, bulldozer.ts, roads/streetKit.ts): placing an
// item or a row of them, a Stop or Give Way plate that becomes its approach's rule, a barrier, and taking away what stands
// at a point. What a road carries goes with it or moves with its new sidewalks.
#include "sim/commands/commands.h"
#include "sim/commands/street_commands.h"
#include "sim/commands/street_journal.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"
#include "sim/street/street_names.h"
#include "sim/roads/parking_layout.h"
#include "sim/street/street_spots.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

namespace city {

namespace {
constexpr int MIN_RULE_ARMS = 3;
constexpr double ROW_STEP_EPS_M = 0.002; // a row reaches its last station past the rounding of the sum
constexpr int KIT_ARC_STEPS = 64; // the arc table a moved road's kit is put back along (as road_commands.cpp lays its pieces)

// The approach a Stop or Give Way plate at (x, z) governs ("the plate is the rule"), or -1: the node of the segment end it stands
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
	j_set_control(s, n, CTL_PRIORITY); // the 4.0's streetscapeTool: a plate makes the junction one by priority
	j_set_rule(s, seg, s.roads.segs[size_t(seg)].a == n ? 0 : 1, rule);
}

template <typename F>
void each_piece_on(const CityState &s, int seg, F &&fn) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const float pad = s.catalog.furniture.poles.footway_catch_m + s.catalog.furniture.items.stand_reach_m;
	const float reach = s.catalog.furniture.items.stand_reach_m;
	s.street.items_in(g.minx - pad, g.minz - pad, g.maxx + pad, g.maxz + pad, [&](int id) {
		const StreetItem &o = s.street.items.items[size_t(id)];
		if (!street_kit_piece(o.kind)) return; // stays where it was put
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

EditResult set_approach_rule(CityState &s, int node, int seg, int rule) {
	EditResult r;
	const std::vector<int> &legs = s.roads.nodes[size_t(node)].segs;
	if (std::find(legs.begin(), legs.end(), seg) == legs.end() || legs.size() < 3) {
		r.error = "hint.control.notJunction";
		return r;
	}
	set_rule(s, seg, node, uint16_t(rule));
	r.ok = true;
	return r;
}

EditResult place_item(CityState &s, ItemKind kind, SignType sign, const std::string &text, double x, double z, double reach) {
	EditResult r;
	const ItemSnap snap = snap_item(s, kind, x, z, reach, FurnitureRead::PLACED);
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
			return r;
		}
	}
	StreetItem it;
	it.kind = kind;
	it.sign = sign;
	it.x = float(snap.x);
	it.z = float(snap.z);
	if (kind != ItemKind::SIGN || sign_has_text(sign)) it.text = text;
	j_add_item(s, it); // a street's name: its plates are made at the settle (settle_name_plates)
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
		const ItemSnap snap = snap_item(s, kind, ax + (bx - ax) * t, az + (bz - az) * t, reach, FurnitureRead::PLACED); // each placed one counts for the next
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
	r.ok = true;
	return r;
}

bool remove_street_at(CityState &s, double x, double z, double reach) {
	const float fx = float(x), fz = float(z), fr = float(reach);
	const int item = s.street.item_near(fx, fz, fr);
	if (item >= 0) {
		j_remove_item(s, item); // a street's name: its plates go at the settle (settle_name_plates)
		return true;
	}
	const int pole = s.street.pole_near(fx, fz, fr);
	if (pole >= 0) {
		j_remove_pole(s, pole);
		return true;
	}
	const int barrier = s.street.barrier_near(fx, fz, fr);
	if (barrier >= 0) {
		j_remove_barrier(s, barrier);
		return true;
	}
	// A piece of the street's own kit: it stays cut by its place.
	const FootwayHit h = footway_at(s.roads, x, z, reach);
	if (!h.ok) return false;
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, h.seg, FURNITURE_ALL & ~furniture_bit(FurnitureKind::SIGNAL), spots, FurnitureRead::PLACED);
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

void carry_kit(CityState &s, const std::vector<CarriedPiece> &kit, const KitPlace &place) {
	std::map<int, int> pole_to; // a carried pole's id then -> now (-1: it went)
	std::vector<std::pair<int, int>> wires; // the spans of the carried poles, by the ids of their ends then
	for (const CarriedPiece &c : kit) {
		if (c.pole < 0) continue;
		for (int span : s.street.poles.items[size_t(c.pole)].spans) {
			const UtilitySpan &sp = s.street.spans.items[size_t(span)];
			wires.push_back(std::minmax(sp.a, sp.b));
		}
	}
	std::sort(wires.begin(), wires.end());
	wires.erase(std::unique(wires.begin(), wires.end()), wires.end());
	for (const CarriedPiece &c : kit) {
		double x = 0, z = 0;
		const bool stays = place(c, x, z);
		if (c.item >= 0) {
			StreetItem it = s.street.items.items[size_t(c.item)];
			j_remove_item(s, c.item);
			if (!stays) continue;
			it.x = float(x);
			it.z = float(z);
			j_add_item(s, it);
			continue;
		}
		UtilityPole moved = s.street.poles.items[size_t(c.pole)];
		j_remove_pole(s, c.pole); // and its spans, strung again below once every pole stands where it goes
		pole_to[c.pole] = -1;
		if (!stays) continue;
		moved.spans.clear();
		moved.x = float(x);
		moved.z = float(z);
		pole_to[c.pole] = j_add_pole(s, moved);
	}
	const auto now = [&](int id) { const auto it = pole_to.find(id); return it == pole_to.end() ? id : it->second; };
	for (const auto &w : wires) {
		const int a = now(w.first), b = now(w.second);
		if (a >= 0 && b >= 0) j_add_span(s, a, b);
	}
}

void carry_street_kit(CityState &s, int seg, const std::vector<CarriedPiece> &kit) {
	const Profile &p = s.roads.profile_of(seg);
	carry_kit(s, kit, [&](const CarriedPiece &c, double &x, double &z) {
		if (c.side < 0 || !sidewalk_on(p, c.side)) return false;
		double y;
		footway_point(s.roads, seg, c.v, sidewalk_u(p, c.side, c.from_kerb), x, z, y);
		return true;
	});
}

void carry_kit_along(CityState &s, const std::vector<CarriedPiece> &kit, double old_len, const Quad &q, const std::vector<int> &laid) {
	const std::vector<double> table = arc_table(q, KIT_ARC_STEPS);
	carry_kit(s, kit, [&](const CarriedPiece &c, double &x, double &z) {
		if (old_len <= 0) return false;
		double px, pz;
		q.at(t_at_length(table, table.back() * std::clamp(c.v / old_len, 0.0, 1.0)), px, pz);
		int on = -1;
		double best_t = 0, best_d = 0;
		for (int sid : laid) {
			if (!s.roads.segs[size_t(sid)].alive) continue;
			double t, d;
			closest_t(s.roads.segs[size_t(sid)].q, px, pz, t, d);
			if (on < 0 || d < best_d) { on = sid; best_t = t; best_d = d; }
		}
		if (on < 0) return false;
		const Profile &p = s.roads.profile_of(on);
		if (c.side < 0 || !sidewalk_on(p, c.side)) return false;
		double y;
		footway_point(s.roads, on, arc_of_t(s.roads.segs[size_t(on)], best_t), sidewalk_u(p, c.side, c.from_kerb), x, z, y);
		return true;
	});
}

} // namespace city
