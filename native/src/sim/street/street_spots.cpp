// The player's records as pieces of a street, see street_spots.h.
#include "sim/street/street_spots.h"

#include "sim/roads/junction.h"
#include "sim/roads/street_furniture_internal.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace city {

using namespace furniture;

namespace {
// The pieces are looked for this far round a segment's bounds: the widest street's footway.
constexpr float SEGMENT_PAD_M = 40.f;
constexpr int CHAIN_GUARD = 4000; // no street runs through more segments
constexpr int MIN_CORNER_ARMS = 2; // a street's name stands where another street meets it or it goes on

FurnitureSpot piece_on(const RoadNetwork &net, FurnitureKind k, const FootwayHit &h) {
	const Profile &p = net.profile_of(h.seg);
	return spot(net, k, h.seg, p, h.v, h.u, toward_road(h.side), 0.f);
}

bool touches(const FurnitureSpot &a, const FurnitureSpot &b) {
	return a.side == b.side && std::fabs(a.v - b.v) < a.half_v + b.half_v && std::fabs(a.u - b.u) < a.half_u + b.half_u;
}
} // namespace

std::vector<int> street_chain(const RoadNetwork &net, int s0) {
	int prof = net.segs[s0].profile;
	const Profile &p = net.cat->profile(prof);
	std::vector<int> out{ s0 };
	std::unordered_set<int> in{ s0 }; // what `out` holds: a street meets itself only by a loop
	for (int start_end : { 1, 0 }) {
		int sg = s0, end = start_end;
		for (int guard = 0; guard < CHAIN_GUARD; guard++) {
			int n = end == 1 ? net.segs[sg].b : net.segs[sg].a;
			Arm here = arm_of(net.segs[sg].q, p, end);
			double best_dot = net.cat->tune.edit_street_cos;
			int best = -1, best_end = 0;
			for (int t : net.nodes[n].segs) {
				if (in.count(t) || net.segs[t].profile != prof || net.cat->profile(prof).ring) continue;
				int t_end = net.segs[t].a == n ? 0 : 1;
				if (p.one_way && t_end != (end == 1 ? 0 : 1)) continue;
				Arm arm = arm_of(net.segs[t].q, p, t_end);
				double d = -(arm.dx * here.dx + arm.dz * here.dz);
				if (d > best_dot) { best_dot = d; best = t; best_end = t_end; }
			}
			if (best < 0) break;
			out.push_back(best);
			in.insert(best);
			sg = best;
			end = best_end == 0 ? 1 : 0;
		}
	}
	return out;
}

FurnitureKind item_piece(ItemKind k) {
	switch (k) {
		case ItemKind::TREE: return FurnitureKind::TREE;
		case ItemKind::SHRUB: return FurnitureKind::SHRUB;
		case ItemKind::BENCH: return FurnitureKind::BENCH;
		case ItemKind::BIN: return FurnitureKind::BIN;
		case ItemKind::LAMP: return FurnitureKind::COLUMN;
		case ItemKind::HYDRANT: return FurnitureKind::HYDRANT;
		case ItemKind::POSTBOX: return FurnitureKind::POSTBOX;
		case ItemKind::PHONE: return FurnitureKind::PHONE;
		case ItemKind::DRAIN: return FurnitureKind::DRAIN;
		case ItemKind::SIGN: return FurnitureKind::SIGN;
		default: return FurnitureKind::COUNT;
	}
}

void merge_street(const StreetWorks &w, const RoadNetwork &net, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out, size_t first) {
	if (w.empty()) return;
	const RoadSegment &g = net.segs[size_t(sid)];
	const float x0 = g.minx - SEGMENT_PAD_M, z0 = g.minz - SEGMENT_PAD_M, x1 = g.maxx + SEGMENT_PAD_M, z1 = g.maxz + SEGMENT_PAD_M;
	const float stand = defs(net).items.stand_reach_m, cut = defs(net).items.cut_reach_m;
	std::vector<FurnitureSpot> mine;
	w.items_in(x0, z0, x1, z1, [&](int id) {
		const StreetItem &it = w.items.items[size_t(id)];
		const FurnitureKind k = item_piece(it.kind);
		if (k == FurnitureKind::COUNT) return;
		const FootwayHit h = footway_at(net, it.x, it.z, stand);
		if (!h.ok || h.seg != sid) return;
		FurnitureSpot s = piece_on(net, k, h);
		s.item = id;
		mine.push_back(s);
	});
	w.poles_in(x0, z0, x1, z1, [&](int id) {
		const UtilityPole &pole = w.poles.items[size_t(id)];
		const FootwayHit h = footway_at(net, pole.x, pole.z, stand);
		if (h.ok && h.seg == sid) mine.push_back(piece_on(net, FurnitureKind::POLE, h));
	});
	std::vector<FurnitureSpot> kept(out.begin(), out.begin() + long(first)); // what other segments put there stays
	for (size_t i = first; i < out.size(); i++) {
		const FurnitureSpot &s = out[i];
		double x, z, y;
		footway_point(net, s.seg, s.v, s.u, x, z, y);
		bool gone = false;
		w.cuts_in(float(x) - cut, float(z) - cut, float(x) + cut, float(z) + cut, [&](int id) {
			const StreetCut &c = w.cuts.items[size_t(id)];
			gone = gone || std::hypot(c.x - x, c.z - z) <= cut;
		});
		if (s.kind != FurnitureKind::SIGNAL)
			gone = gone || std::any_of(mine.begin(), mine.end(), [&](const FurnitureSpot &m) { return touches(s, m); });
		if (!gone) kept.push_back(s);
	}
	out.swap(kept);
	for (const FurnitureSpot &s : mine)
		if (kinds & furniture_bit(s.kind)) out.push_back(s);
}

void placed_sign(const RoadNetwork &net, const StreetItem &item, std::vector<SignStand> &out) {
	const FootwayHit h = footway_at(net, item.x, item.z, defs(net).items.stand_reach_m);
	if (!h.ok) return;
	double fx, fz, lx, lz, x, z, y;
	segment_frame(net, h.seg, h.v, fx, fz, lx, lz);
	footway_point(net, h.seg, h.v, h.u, x, z, y);
	SignStand s;
	s.x = item.x;
	s.y = float(y);
	s.z = item.z;
	s.type = item.sign;
	s.text = item.text;
	const bool read_from_street = item.sign == SignType::STREET || item.sign == SignType::DIRECTION || item.sign == SignType::INFO;
	const float oncoming = h.side == 0 ? -1.f : 1.f; // the traffic on its side comes at it: the right sidewalk's runs a -> b
	s.face_x = read_from_street ? h.kx : float(fx) * oncoming;
	s.face_z = read_from_street ? h.kz : float(fz) * oncoming;
	out.push_back(s);
}

void name_plates(const RoadNetwork &net, const StreetItem &item, const std::vector<int> &chain, std::vector<SignStand> &out) {
	const StreetFurnitureDef &d = defs(net);
	const float from_kerb = kerbside_reach(net, FurnitureKind::SIGN);
	for (int sid : chain) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (!g.alive || p.edges.empty()) continue;
		for (int end = 0; end < 2; end++) {
			const int side = end == 0 ? 0 : 1; // on the sidewalk at the right seen from the corner
			const int n = end == 0 ? g.a : g.b;
			if (!walk_on(p, side) || int(net.nodes[size_t(n)].segs.size()) < MIN_CORNER_ARMS) continue;
			const double v = end == 0 ? g.retreat_a + d.signs.name_past_mouth_m : g.len - g.retreat_b - d.signs.name_past_mouth_m;
			if (v <= 0 || v >= g.len) continue;
			double x, z, y, fx, fz, lx, lz;
			footway_point(net, sid, v, kerb_in(p, side, from_kerb), x, z, y);
			segment_frame(net, sid, v, fx, fz, lx, lz);
			SignStand s;
			s.x = float(x);
			s.y = float(y);
			s.z = float(z);
			s.type = SignType::STREET;
			s.text = item.text;
			s.face_x = float(lx); // along the street, read from both sides
			s.face_z = float(lz);
			out.push_back(s);
		}
	}
}

} // namespace city
