// What one walker sees in a crowd step (ped_crowd.cpp): the bodies that can stand in its room, remapped into its way's frame (ped_stripes.h
// Seen): the walkers of its own sidewalk by arc, those filed with a junction near it in the world (SUMO's walking areas), its sidewalk's
// furniture by arc and, round a corner, the footprints of the furniture on the junction's arms.
#include "sim/traffic/ped_stripes.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
constexpr int FOOTPRINT_CORNERS = 4;
} // namespace

void see_sidewalk(const std::vector<CrowdEntry> &list, size_t k, Seen &sn) {
	const CrowdEntry &w = list[k];
	const float way = w.dir == 0 ? 1.f : -1.f; // ahead is up the arc or down it
	for (size_t j = k + 1; j < list.size() && !sn.full(); j++) {
		const float along = (list[j].s - w.s) * way;
		if (along > sn.look || along <= -sn.touch) break;
		sn.walker(along, list[j].u, list[j].dir != w.dir && list[j].moving);
	}
	for (size_t j = k; j-- > 0 && !sn.full();) {
		const float along = (list[j].s - w.s) * way;
		if (along > sn.look || along <= -sn.touch) break;
		sn.walker(along, list[j].u, list[j].dir != w.dir && list[j].moving);
	}
}

void see_junction(const TrafficState &t, int node, int self, int own_sidewalk, Seen &sn) {
	for (const CrowdEntry &o : t.node_walkers[size_t(node)]) {
		if (sn.full()) return;
		if (o.id == self || (own_sidewalk >= 0 && o.sidewalk == own_sidewalk)) continue;
		const V2 rel(o.x - sn.f.at.x, o.z - sn.f.at.y);
		const bool facing = o.fx * sn.f.fwd.x + o.fz * sn.f.fwd.y < 0; // heading against its way (SUMO's direction)
		sn.walker(rel.dot(sn.f.fwd), sn.f.u + rel.dot(sn.f.left), facing && o.moving);
	}
}

void see_sidewalk_furniture(const std::vector<StreetObstacle> &list, size_t from, const CrowdEntry &w, Seen &sn) {
	for (auto it = list.begin() + std::ptrdiff_t(from); it != list.end() && it->s0 <= w.s + sn.look && !sn.full(); ++it) {
		Body b;
		b.furniture = true;
		b.reach = sn.reach;
		b.along_lo = w.dir == 0 ? it->s0 - w.s : w.s - it->s1;
		b.along_hi = w.dir == 0 ? it->s1 - w.s : w.s - it->s0;
		b.lat_lo = it->u0;
		b.lat_hi = it->u1;
		sn.furniture(b);
	}
}

void junction_furniture(CityState &s, int n, float look, std::vector<const StreetObstacle *> &out) {
	out.clear();
	const float reach = s.catalog.furniture.crossing.zebra_to_m + look + s.catalog.traffic.ped_body_m;
	for (int seg : s.roads.nodes[size_t(n)].segs) {
		if (!walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const SidewalkFurniture &sf = sidewalk_furniture(s, seg);
		const float lo = g.a == n ? -BIG : float(g.len - g.retreat_b) - reach - sf.longest;
		const float hi = g.a == n ? float(g.retreat_a) + reach : BIG;
		for (const auto &side : sf.side)
			for (auto it = std::lower_bound(side.begin(), side.end(), lo, [](const StreetObstacle &o, float v) { return o.s0 < v; });
					it != side.end() && it->s0 <= hi; ++it)
				out.push_back(&*it);
	}
}

void see_footprint(const StreetObstacle &o, Seen &sn) {
	if (sn.full()) return;
	const V2 c = V2(o.x, o.z) - sn.f.at, a(o.fx, o.fz), l = left_of(a);
	// by its centre first: a piece wholly behind, beyond the look or off the room across is not projected
	const float spread = o.half_along + o.half_across, ahead = c.dot(sn.f.fwd), across = sn.f.u + c.dot(sn.f.left);
	if (ahead + spread < -sn.reach || ahead - spread > sn.look || across + spread < sn.f.lo - sn.reach || across - spread > sn.f.hi + sn.reach) return;
	Body b;
	b.furniture = true;
	b.reach = sn.reach;
	b.along_lo = b.lat_lo = BIG;
	b.along_hi = b.lat_hi = -BIG;
	for (int k = 0; k < FOOTPRINT_CORNERS; k++) {
		const V2 q = c + a * (k & 1 ? o.half_along : -o.half_along) + l * (k & 2 ? o.half_across : -o.half_across);
		const float along = q.dot(sn.f.fwd), lat = sn.f.u + q.dot(sn.f.left);
		b.along_lo = std::min(b.along_lo, along);
		b.along_hi = std::max(b.along_hi, along);
		b.lat_lo = std::min(b.lat_lo, lat);
		b.lat_hi = std::max(b.lat_hi, lat);
	}
	sn.furniture(b);
}

} // namespace traffic
} // namespace city
