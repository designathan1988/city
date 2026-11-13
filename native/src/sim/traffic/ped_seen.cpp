// What one walker sees in a crowd step (ped_crowd.cpp): the bodies that can stand in its room, remapped into its way's frame (ped_stripes.h
// Seen): the walkers of its own sidewalk by arc, those filed with a junction near it in the world (SUMO's walking areas), its sidewalk's
// furniture by arc and, round a corner, the footprints of the furniture on the junction's arms. Somebody lying on the ground is the
// footprint of their body (ped_system.h lying: from their feet the way they went down, as long as the body), seen as furniture: the others go round
// it and never squeeze through, as people step round somebody lying (Cornes, Frank, Dorso 2017: the unconscious as inanimate bodies).
#include "sim/traffic/ped_stripes.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr int FOOTPRINT_CORNERS = 4;

// Somebody on the walker's own sidewalk, their feet `along` ahead of it: a walker, or the footprint of a body lying there, its middle
// half its length from the feet the way it lies, its extent ahead and across from that length and its thickness, in the walker's way
// (its heading where it stands; across, the sidewalk's own u: left of a -> b).
void see_on_sidewalk(const CrowdEntry &o, const CrowdEntry &w, float along, Seen &sn) {
	if (o.lie <= 0.f) {
		sn.walker(along, o.u, o.dir != w.dir && o.moving);
		return;
	}
	const V2 ahead(w.fx, w.fz), axis(o.lie_x, o.lie_z), u_way = left_of(w.dir == 0 ? ahead : -ahead);
	const float on = axis.dot(ahead), off = axis.dot(u_way), a = std::fabs(on), l = std::fabs(off);
	const float mid_along = along + on * o.lie, mid_u = o.u + off * o.lie;
	const float half_along = a * o.lie + l * LYING_R, half_across = l * o.lie + a * LYING_R;
	Body b;
	b.furniture = true;
	b.reach = sn.reach;
	b.along_lo = mid_along - half_along;
	b.along_hi = mid_along + half_along;
	b.lat_lo = mid_u - half_across;
	b.lat_hi = mid_u + half_across;
	sn.furniture(b);
}

// The footprint of somebody lying, in the world: its middle half its length from their feet the way they lie, along that way.
StreetObstacle lying_footprint(const CrowdEntry &o) {
	StreetObstacle f;
	f.x = o.x + o.lie_x * o.lie;
	f.z = o.z + o.lie_z * o.lie;
	f.fx = o.lie_x;
	f.fz = o.lie_z;
	f.half_along = o.lie;
	f.half_across = LYING_R;
	return f;
}
} // namespace

// In order of arc both ways from the walker, as far as anybody may stand in its room: ahead as far as it looks, behind as far as a body
// beside it (a walker within two radii, somebody lying within their body's half besides).
void see_sidewalk(const std::vector<CrowdEntry> &list, size_t k, Seen &sn) {
	const CrowdEntry &w = list[k];
	const float way = w.dir == 0 ? 1.f : -1.f; // ahead is up the arc or down it
	const float ahead = sn.look + sn.lie_most, behind = sn.touch + sn.lie_most;
	for (size_t j = k + 1; j < list.size() && !sn.full(); j++) {
		const float along = (list[j].s - w.s) * way;
		if (along > ahead || along <= -behind) break;
		see_on_sidewalk(list[j], w, along, sn);
	}
	for (size_t j = k; j-- > 0 && !sn.full();) {
		const float along = (list[j].s - w.s) * way;
		if (along > ahead || along <= -behind) break;
		see_on_sidewalk(list[j], w, along, sn);
	}
}

void see_junction(const TrafficState &t, int node, int self, int own_sidewalk, Seen &sn) {
	for (const CrowdEntry &o : t.node_walkers[size_t(node)]) {
		if (sn.full()) return;
		if (o.id == self || (own_sidewalk >= 0 && o.sidewalk == own_sidewalk)) continue;
		if (o.lie > 0.f) {
			see_footprint(lying_footprint(o), sn);
			continue;
		}
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
