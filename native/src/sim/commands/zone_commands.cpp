// Zone brush (Roadcraft src/features/zones/zoneTool.ts dab + clearSide): paints the zone on the road sides whose
// frontage the brush touches; a building of another base zone facing a repainted side is knocked down.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int SIDE_LEFT = 0, SIDE_RIGHT = 1;
constexpr double FRONT_REACH_M = 80; // buildings this far behind a side still face it (deepest downtown lots)

bool zonable(const Profile &p) { return !p.rail && p.walk(); }

// Does the brush disc touch the frontage line of this side (zoneTool.ts: a point every zone_sample_m)?
bool side_hit(const CityState &s, int seg, int side, double x, double z, double radius) {
	const RoadSegment &g = s.roads.segs[seg];
	const Profile &p = s.catalog.profile(g.profile);
	const Tuning &tn = s.catalog.tune;
	double off = side == SIDE_LEFT ? p.left + tn.zone_side_offset_m : -(p.right + tn.zone_side_offset_m);
	int n = std::max(2, int(std::ceil(g.len / tn.zone_sample_m)));
	for (int i = 0; i <= n; i++) {
		double t = double(i) / n, px, pz, dx, dz;
		g.q.at(t, px, pz);
		g.q.deriv(t, dx, dz);
		double l = std::hypot(dx, dz);
		if (l <= 0) continue;
		px += dz / l * off;
		pz -= dx / l * off;
		if ((px - x) * (px - x) + (pz - z) * (pz - z) <= radius * radius) return true;
	}
	return false;
}

// Knocks down the buildings on this side of the segment (nearest street = this one) whose base zone differs.
int clear_side(CityState &s, int seg, int side, int keep_zone) {
	const RoadSegment &g = s.roads.segs[seg];
	std::vector<int> doomed;
	float r = float(FRONT_REACH_M);
	s.buildings.query(g.minx - r, g.minz - r, g.maxx + r, g.maxz + r, [&](int id) {
		const Building &b = s.buildings.items[id];
		if (b.zone == 0 || (keep_zone != 0 && base_zone(b.zone) == base_zone(keep_zone))) return;
		double t, d, nt, nd;
		closest_t(g.q, b.x, b.z, t, d);
		if (d > FRONT_REACH_M || s.roads.find_segment_near(b.x, b.z, d, nt, nd) != seg) return;
		double px, pz, dx, dz;
		g.q.at(t, px, pz);
		g.q.deriv(t, dx, dz);
		bool left = (b.x - px) * dz - (b.z - pz) * dx > 0;
		if (left == (side == SIDE_LEFT)) doomed.push_back(id);
	});
	for (int id : doomed) j_remove_building(s, id);
	return int(doomed.size());
}
} // namespace

EditResult paint_zone(CityState &s, double x, double z, double radius, int zone) {
	EditResult r;
	if (zone < 0 || zone >= ZONE_CODES) {
		r.error = "tool.failed";
		return r;
	}
	RoadNetwork &net = s.roads;
	double reach = radius + s.catalog.tune.zone_side_offset_m + s.catalog.widest_half();
	std::vector<std::pair<int, int>> hits;
	net.for_segments_near(x - reach, z - reach, x + reach, z + reach, [&](int sg) {
		if (!zonable(net.profile_of(sg))) return;
		for (int side : { SIDE_LEFT, SIDE_RIGHT })
			if (side_hit(s, sg, side, x, z, radius)) hits.push_back({ sg, side });
	});
	for (auto &h : hits) {
		const RoadSegment &g = net.segs[h.first];
		int cur = h.second == SIDE_LEFT ? g.zl : g.zr;
		if (cur == zone) continue;
		if (cur != 0) r.buildings_removed += clear_side(s, h.first, h.second, zone);
		j_set_zone(s, h.first, h.second, zone);
		r.zoned_sides++;
	}
	if (r.zoned_sides == 0) {
		r.error = hits.empty() ? "tools.toast.zoneNoRoad" : "tools.toast.zoneSame";
		return r;
	}
	finish_edit(s, x - reach, z - reach, x + reach, z + reach);
	r.ok = true;
	return r;
}

} // namespace city
