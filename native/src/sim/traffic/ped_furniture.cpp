// The street furniture as the walkers meet it: the pieces sim/roads/street_furniture.cpp places (its ends' signal posts
// where junction control signals them) become bodies standing on their sidewalks, footprints along the arc and across
// from the sidewalk's middle, and in the world. A segment's bodies are built when a walker first meets it, dropped when
// an edit touches the segment or one of its nodes (lane_graph_edit.cpp, reset_traffic) and when the walkers are far.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace city {
namespace traffic {

namespace {

// Distance from (x, z) to a footprint (0 inside it).
float distance_to(const StreetObstacle &o, float x, float z) {
	const float rx = x - o.x, rz = z - o.z;
	const float along = std::fabs(rx * o.fx + rz * o.fz), across = std::fabs(rx * o.fz - rz * o.fx);
	return std::hypot(std::max(0.f, along - o.half_along), std::max(0.f, across - o.half_across));
}

void build(const CityState &s, int seg, SidewalkFurniture &out) {
	std::vector<FurnitureSpot> spots;
	street_furniture_of(s, seg, FURNITURE_ALL, spots);
	const Profile &p = s.roads.profile_of(seg);
	for (const FurnitureSpot &f : spots) {
		if (f.side < 0) continue; // not on a sidewalk: no walker meets it
		const float across = f.u - (f.side == 0 ? p.walk_right : p.walk_left);
		Pose at;
		sidewalk_pose(s, seg, f.side, float(f.v), across, at);
		StreetObstacle o;
		o.s0 = float(f.v) - f.half_v;
		o.s1 = float(f.v) + f.half_v;
		o.u0 = across - f.half_u;
		o.u1 = across + f.half_u;
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

} // namespace

void street_furniture_of(const CityState &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	if (!g.alive) return;
	const bool signalled[2] = { control_of(s, g.a) == Control::SIGNAL, control_of(s, g.b) == Control::SIGNAL };
	segment_furniture(s.roads, sid, signalled, kinds, out);
}

const SidewalkFurniture &sidewalk_furniture(CityState &s, int seg) {
	auto &cache = s.traffic.furniture;
	auto it = cache.find(seg);
	if (it != cache.end()) return it->second;
	SidewalkFurniture &f = cache[seg];
	build(s, seg, f);
	return f;
}

bool on_furniture(CityState &s, int seg, int side, float x, float z, float reach) {
	if (seg < 0 || side < 0 || side > 1) return false;
	for (const StreetObstacle &o : sidewalk_furniture(s, seg).side[side])
		if (distance_to(o, x, z) < reach) return true;
	return false;
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

int walkers_in_furniture(const CityState &s) {
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
	for (const Pedestrian &w : s.traffic.peds) {
		if (!w.alive) continue;
		bool hit = false;
		auto check = [&](int seg) {
			if (hit || seg < 0 || !s.roads.segs[size_t(seg)].alive) return;
			for (const auto &side : of(seg).side)
				for (const StreetObstacle &o : side) hit = hit || distance_to(o, w.cur.x, w.cur.z) < body;
		};
		check(w.seg);
		if (w.mode != PedMode::SIDEWALK && w.way_node >= 0)
			for (int seg : s.roads.nodes[size_t(w.way_node)].segs) check(seg);
		inside += hit ? 1 : 0;
	}
	return inside;
}

} // namespace traffic
} // namespace city
