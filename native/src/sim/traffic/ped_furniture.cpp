// The street furniture of a segment, placed (sim/roads/street_furniture.cpp: the kit and its ends' signal posts where junction
// control signals them; what the player put up and cut; what a blow left standing) or read kept for the streets' revision
// (sim/roads/furniture_kept.h), and as the walkers meet it: bodies standing on their sidewalks, footprints along the arc and
// across from the sidewalk's middle, and in the world. A segment's bodies are built when a walker first meets it, dropped when
// an edit touches the segment or one of its nodes (lane_graph_edit.cpp, reset_traffic) and when the walkers are far. The one
// owner of what stands clear of the furniture: a body at a point, a straight way (the exact distance of the segment from each
// footprint) and the place nearest a wanted one where somebody is put on the footway (a door's foot, a kerb stop's).
#include "sim/buildings/lot_access.h"
#include "sim/street/street_spots.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace city {
namespace traffic {

namespace {

// A body touching a piece (its centre a body radius off it, to the rounding of the sum) stands beside it, not in it.
constexpr float TOUCH_EPS_M = 1e-3f;
constexpr int BOX_CORNERS = 4;

// A point in a footprint's own frame: along it and across it, from its middle.
V2 in_frame(const StreetObstacle &o, const V2 &p) {
	const float rx = p.x - o.x, rz = p.y - o.z;
	return V2(rx * o.fx + rz * o.fz, rx * o.fz - rz * o.fx);
}

// Distance from a point in the footprint's frame to the footprint (0 inside it).
float box_distance(const StreetObstacle &o, const V2 &c) {
	return std::hypot(std::max(0.f, std::fabs(c.x) - o.half_along), std::max(0.f, std::fabs(c.y) - o.half_across));
}

// Distance from (x, z) to a footprint (0 inside it).
float distance_to(const StreetObstacle &o, float x, float z) { return box_distance(o, in_frame(o, V2(x, z))); }

// The least distance from the segment a..b to a footprint, 0 when it goes through it. In the footprint's frame the footprint is a
// box round the middle: a segment that misses it comes nearest it at one of its own ends or at one of the box's corners.
float segment_distance_to(const StreetObstacle &o, const V2 &a, const V2 &b) {
	const V2 p = in_frame(o, a), d = in_frame(o, b) - p;
	// through it: what of the segment lies inside the box on both axes is not empty (Liang-Barsky clipping)
	const float from[2] = { p.x, p.y }, dir[2] = { d.x, d.y }, half[2] = { o.half_along, o.half_across };
	float t0 = 0, t1 = 1;
	bool through = true;
	for (int k = 0; k < 2 && through; k++) {
		if (dir[k] == 0) {
			through = std::fabs(from[k]) <= half[k];
			continue;
		}
		const float ta = (-half[k] - from[k]) / dir[k], tb = (half[k] - from[k]) / dir[k];
		t0 = std::max(t0, std::min(ta, tb));
		t1 = std::min(t1, std::max(ta, tb));
		through = t0 <= t1;
	}
	if (through) return 0.f;
	float least = std::min(box_distance(o, p), box_distance(o, p + d));
	const float len2 = d.dot(d);
	for (int k = 0; k < 4; k++) {
		const V2 corner(k & 1 ? o.half_along : -o.half_along, k & 2 ? o.half_across : -o.half_across);
		const float t = len2 > 0 ? clampf((corner - p).dot(d) / len2, 0.f, 1.f) : 0.f;
		least = std::min(least, (p + d * t - corner).len());
	}
	return least;
}

void build(const CityState &s, int seg, SidewalkFurniture &out) {
	std::vector<FurnitureSpot> spots;
	street_furniture_of(s, seg, FURNITURE_ALL, spots);
	const Profile &p = s.roads.profile_of(seg);
	for (const FurnitureSpot &f : spots) {
		if (f.side < 0) continue; // not on a sidewalk: no walker meets it
		const float middle = f.side == 0 ? p.walk_right : p.walk_left, across = f.u - middle;
		Pose at;
		sidewalk_pose(s, seg, f.side, float(f.v), across, at);
		StreetObstacle o;
		// Its spans along the arc and across, read back from the four corners of its footprint where it stands: on a bend a metre of the
		// sidewalk is not a metre of the road's middle line the arc is measured on, so a span from its length alone leaves part of it out.
		o.s0 = o.u0 = BIG;
		o.s1 = o.u1 = -BIG;
		for (int k = 0; k < BOX_CORNERS; k++) {
			const float a = k & 1 ? f.half_v : -f.half_v, c = k & 2 ? f.half_u : -f.half_u;
			double v;
			float u, past;
			segment_place(s.roads, seg, at.x + at.fx * a + at.fz * c, at.z + at.fz * a - at.fx * c, v, u, past);
			o.s0 = std::min(o.s0, float(v));
			o.s1 = std::max(o.s1, float(v));
			o.u0 = std::min(o.u0, u - middle);
			o.u1 = std::max(o.u1, u - middle);
		}
		o.x = at.x;
		o.z = at.z;
		o.fx = at.fx;
		o.fz = at.fz;
		o.half_along = f.half_v;
		o.half_across = f.half_u;
		out.side[f.side].push_back(o);
		out.longest = std::max(out.longest, o.s1 - o.s0);
	}
	for (auto &side : out.side)
		std::sort(side.begin(), side.end(), [](const StreetObstacle &a, const StreetObstacle &b) { return a.s0 < b.s0; });
}

GoneKind gone_kind(FurnitureKind k) {
	switch (k) {
		case FurnitureKind::SIGNAL: return GoneKind::POST;
		case FurnitureKind::LAMP: return GoneKind::POLE;
		case FurnitureKind::HYDRANT: return GoneKind::HYDRANT;
		case FurnitureKind::BIN: return GoneKind::BIN;
		case FurnitureKind::BENCH: return GoneKind::BENCH;
		default: return GoneKind::TREE;
	}
}
constexpr float GONE_MATCH_M = 0.25f; // a piece knocked out is the one standing within this of where it stood
} // namespace

bool furniture_xz(const CityState &s, int seg, const FurnitureSpot &f, float &x, float &z) {
	if (f.side < 0) return false;
	const Profile &p = s.roads.profile_of(seg);
	Pose at;
	sidewalk_pose(s, seg, f.side, float(f.v), f.u - (f.side == 0 ? p.walk_right : p.walk_left), at);
	x = at.x;
	z = at.z;
	return true;
}

namespace {
// The segment's furniture placed now, appended to `out`; returns the ends it placed signalled (FurnitureKept::Ends), which the kept
// placement holds for the invariant furniture.kept_matches_control (sim/checks/furniture.cpp).
FurnitureKept::Ends place_street_furniture(const CityState &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	if (!g.alive) return 0;
	const bool signalled[2] = { control_of(s, g.a) == Control::SIGNAL, control_of(s, g.b) == Control::SIGNAL };
	const FurnitureKept::Ends ends = FurnitureKept::ends(signalled[0], signalled[1]);
	// the ways into the lots (NBR 9050: the service strip at the kerb holds the furniture and the vehicle ramps alike): a person's
	// gate keeps a walker's body clear; a car's what the kerb's bays keep (the car sweeps the footway past its gate as it turns) and a
	// walker's body more, so nobody is held between a car on the footway and a piece
	const TrafficParams &tp = s.catalog.traffic;
	std::vector<ArcSpan> gates[2];
	street_gates(s.roads, s.buildings, sid, tp.kerb_gate_margin_m + tp.ped_body_m, gates, tp.ped_body_m);
	// the plates the junctions' rules stand on it: their posts are pieces of its placement
	std::vector<RulePlate> rules;
	segment_plates(s, sid, rules);
	std::vector<PlateWanted> plates;
	for (const RulePlate &r : rules) plates.push_back(PlateWanted{ r.v, r.side, r.node });
	const size_t first = out.size();
	segment_furniture(s.roads, sid, signalled, gates, &plates, kinds, out);
	merge_street(s.street, s.roads, sid, kinds, out, first); // what the player put up and cut
	if (s.damage.gone.empty()) return ends;
	// what a blow knocked out is not placed again (sim/demolition/damage_state.h)
	size_t keep = first;
	for (size_t i = first; i < out.size(); i++) {
		float x, z;
		if (!furniture_xz(s, sid, out[i], x, z) || !s.damage.is_gone(gone_kind(out[i].kind), x, z, GONE_MATCH_M)) out[keep++] = out[i];
	}
	out.resize(keep);
	return ends;
}
} // namespace

void street_furniture_of(const CityState &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out, FurnitureRead read) {
	if (read == FurnitureRead::PLACED) {
		place_street_furniture(s, sid, kinds, out);
		return;
	}
	// The kinds only choose among the pieces placed (segment_furniture, merge_street and the knocked-out ones filter each piece
	// by itself), so the kept placement of every kind, filtered, is the placement of these kinds.
	// the ground its lots stand on: as far from the segment as street_gates looks for their gates
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	const float lots = s.roads.profile_of(sid).half() + s.catalog.traffic.kerb_gate_reach_m;
	const FurnitureKept::Reach reach{ float(g.minx) - lots, float(g.minz) - lots, float(g.maxx) + lots, float(g.maxz) + lots };
	const FurnitureKept::Spots all = s.furniture_kept.of(s.street_revision, sid, reach, [&](std::vector<FurnitureSpot> &made) {
		return place_street_furniture(s, sid, FURNITURE_ALL, made);
	});
	for (const FurnitureSpot &f : *all)
		if (kinds & furniture_bit(f.kind)) out.push_back(f);
}

const SidewalkFurniture &sidewalk_furniture(CityState &s, int seg) {
	auto &cache = s.traffic.furniture;
	auto it = cache.find(seg);
	if (it != cache.end()) return it->second;
	SidewalkFurniture &f = cache[seg];
	build(s, seg, f);
	s.traffic.furniture_serial++; // built again from the owner, as it places it now: what was placed clear of the old is placed again
	return f;
}

bool on_furniture(CityState &s, int seg, int side, float x, float z, float reach) {
	if (seg < 0 || side < 0 || side > 1) return false;
	for (const StreetObstacle &o : sidewalk_furniture(s, seg).side[side])
		if (distance_to(o, x, z) < reach) return true;
	return false;
}

bool way_clear(CityState &s, int seg, int side, const V2 &from, const V2 &to, float reach) {
	if (seg < 0 || side < 0 || side > 1) return true;
	for (const StreetObstacle &o : sidewalk_furniture(s, seg).side[size_t(side)])
		if (segment_distance_to(o, from, to) < reach) return false;
	return true;
}

bool clear_foot(CityState &s, int seg, int side, float &arc, float &u, const V2 &from, bool along, const V2 *also) {
	const TrafficParams &p = s.catalog.traffic;
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const float body = p.ped_body_m, half = walk_half(s, seg, side);
	// where a body just clears a piece near it: its band's edges across, its ends along (the places worth trying, none sampled)
	thread_local std::vector<float> arcs, across;
	arcs.assign(1, arc);
	across.assign(1, clampf(u, -half, half));
	for (const StreetObstacle &o : sidewalk_furniture(s, seg).side[size_t(side)]) {
		if (o.s1 + body < arc - p.ped_look_m || o.s0 - body > arc + p.ped_look_m) continue;
		if (along) {
			arcs.push_back(o.s0 - body - TOUCH_EPS_M);
			arcs.push_back(o.s1 + body + TOUCH_EPS_M);
		}
		across.push_back(clampf(o.u0 - body - TOUCH_EPS_M, -half, half));
		across.push_back(clampf(o.u1 + body + TOUCH_EPS_M, -half, half));
	}
	float best = BIG, best_arc = arc, best_u = u;
	for (const float a : arcs) {
		if (a < arc_lo(g) || a > arc_hi(g)) continue;
		for (const float c : across) {
			const float off = std::hypot(a - arc, c - u);
			if (off >= best) continue;
			Pose at;
			sidewalk_pose(s, seg, side, a, c, at);
			const V2 here(at.x, at.z);
			if (on_furniture(s, seg, side, at.x, at.z, body) || !way_clear(s, seg, side, from, here, body) || (also && !way_clear(s, seg, side, *also, here, body)))
				continue;
			best = off;
			best_arc = a;
			best_u = c;
		}
	}
	if (best >= BIG) return false;
	arc = best_arc;
	u = best_u;
	return true;
}

float clear_arc(CityState &s, int seg, int side, float at, float u) {
	const float body = s.catalog.traffic.ped_body_m;
	const std::vector<StreetObstacle> &pieces = sidewalk_furniture(s, seg).side[size_t(side)];
	for (bool moved = true; moved;) { // past one piece it may stand in the next: again until none covers it
		moved = false;
		for (const StreetObstacle &o : pieces) {
			if (o.s0 - body >= at) break; // by s0: none further on reaches back to it
			if (o.s1 + body <= at || u <= o.u0 - body || u >= o.u1 + body) continue;
			at = o.s1 + body;
			moved = true;
		}
	}
	return at;
}

void forget_furniture(CityState &s, int seg) { s.traffic.furniture.erase(seg); }

void prune_furniture(CityState &s, float cx, float cz, float radius) {
	auto &cache = s.traffic.furniture;
	for (auto it = cache.begin(); it != cache.end();) {
		const RoadSegment &g = s.roads.segs[size_t(it->first)];
		const float x = float((g.minx + g.maxx) * 0.5) - cx, z = float((g.minz + g.maxz) * 0.5) - cz;
		it = x * x + z * z > radius * radius ? cache.erase(it) : std::next(it);
	}
}

int furniture_in_gates(const CityState &s, bool signals, GateBlock *first) {
	int in = 0;
	std::vector<FurnitureSpot> spots;
	std::vector<ArcSpan> gates[2];
	const uint32_t kinds = signals ? furniture_bit(FurnitureKind::SIGNAL) : FURNITURE_ALL & ~furniture_bit(FurnitureKind::SIGNAL);
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		gates[0].clear();
		gates[1].clear();
		street_gates(s.roads, s.buildings, sid, 0, gates, 0); // the gates themselves, no margin
		if (gates[0].empty() && gates[1].empty()) continue;
		spots.clear();
		street_furniture_of(s, sid, kinds, spots);
		for (const FurnitureSpot &f : spots) {
			if (f.side < 0) continue;
			for (const ArcSpan &gate : gates[size_t(f.side)])
				if (f.v + f.half_v > gate.first + TOUCH_EPS_M && f.v - f.half_v < gate.second - TOUCH_EPS_M) {
					if (in++ == 0 && first) *first = GateBlock{ f, gate };
					break;
				}
		}
	}
	return in;
}

int stale_furniture(const CityState &s) {
	int stale = 0;
	for (const auto &[seg, kept] : s.traffic.furniture) {
		if (seg < 0 || seg >= int(s.roads.segs.size()) || !s.roads.segs[size_t(seg)].alive) continue;
		SidewalkFurniture now;
		build(s, seg, now);
		bool same = true;
		for (int side = 0; side < 2 && same; side++) {
			same = kept.side[side].size() == now.side[side].size();
			for (size_t k = 0; same && k < now.side[side].size(); k++) {
				const StreetObstacle &a = kept.side[side][k], &b = now.side[side][k];
				same = std::fabs(a.s0 - b.s0) < TOUCH_EPS_M && std::fabs(a.s1 - b.s1) < TOUCH_EPS_M && std::fabs(a.u0 - b.u0) < TOUCH_EPS_M &&
						std::fabs(a.u1 - b.u1) < TOUCH_EPS_M;
			}
		}
		stale += same ? 0 : 1;
	}
	return stale;
}

int walkers_in_furniture(const CityState &s, std::vector<std::pair<int, StreetObstacle>> *who) {
	// from the owner again, not the walkers' cache: the probe checks what they keep off as well as how
	std::unordered_map<int, SidewalkFurniture> seen;
	auto of = [&](int seg) -> const SidewalkFurniture & {
		auto it = seen.find(seg);
		if (it != seen.end()) return it->second;
		SidewalkFurniture &f = seen[seg];
		build(s, seg, f);
		return f;
	};
	const float body = s.catalog.traffic.ped_body_m;
	int inside = 0;
	for (size_t id = 0; id < s.traffic.peds.size(); id++) {
		const Pedestrian &w = s.traffic.peds[id];
		if (!w.alive) continue;
		bool hit = false;
		StreetObstacle piece;
		auto check = [&](int seg) {
			if (hit || seg < 0 || !s.roads.segs[size_t(seg)].alive) return;
			for (const auto &side : of(seg).side)
				for (const StreetObstacle &o : side)
					if (!hit && distance_to(o, w.cur.x, w.cur.z) < body - TOUCH_EPS_M) {
						hit = true;
						piece = o;
					}
		};
		check(w.seg);
		if (w.mode != PedMode::SIDEWALK && w.way_node >= 0)
			for (int seg : s.roads.nodes[size_t(w.way_node)].segs) check(seg);
		inside += hit ? 1 : 0;
		if (hit && who) who->emplace_back(int(id), piece);
	}
	return inside;
}

} // namespace traffic
} // namespace city
