// The parks' trees and the named roads' names of a scenic city, see scenic_finish.h.
#include "sim/citygen/scenic_finish.h"

#include "core/random.h"
#include "core/seeded.h"
#include "sim/citygen/scenic_rules.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"


namespace city {

namespace {
constexpr uint32_t PARK_SALT = 0x9a7c5U, STREAM_X = 1U, STREAM_Z = 2U; // the hash streams of a tree's two offsets
// Where along a named road its name is looked for, as shares of its line from the middle out: a share that falls on a junction
// or a bridge has no street of the road's own profile near it, and the next is tried.
constexpr float LOOK_SHARES[] = { 0.5f, 0.4f, 0.6f, 0.3f, 0.7f, 0.2f, 0.8f };
constexpr float SEEK_RADIUS_M = 3;

bool near_road(const CityState &s, V2 p, float clear) {
	const float reach = clear + s.catalog.widest_half();
	bool hit = false;
	s.roads.for_segments_near(p.x - reach, p.y - reach, p.x + reach, p.y + reach, [&](int seg) {
		if (hit) return;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		double t, d;
		closest_t(g.q, p.x, p.y, t, d);
		hit = d < s.catalog.profile(g.profile).half() + clear;
	});
	return hit;
}

bool on_building(const CityState &s, V2 p, float pad) {
	bool hit = false;
	s.buildings.query(p.x - pad, p.y - pad, p.x + pad, p.y + pad, [&](int id) { hit = hit || BuildingStore::covers(s.buildings.items[size_t(id)], p.x, p.y, pad); });
	return hit;
}

// The point a share of the way along the line.
V2 along(const Polyline &line, float share) {
	float left = polyline_length(line) * share;
	for (size_t k = 0; k + 1 < line.size(); k++) {
		const float seg = (line[k + 1] - line[k]).len();
		if (left <= seg && seg > 0) return line[k] + (line[k + 1] - line[k]) * (left / seg);
		left -= seg;
	}
	return line.back();
}

// The live segment of `profile` nearest p within `reach`, or -1.
int street_of(const CityState &s, V2 p, int profile, float reach) {
	int best = -1;
	double near = reach;
	s.roads.for_segments_near(p.x - reach, p.y - reach, p.x + reach, p.y + reach, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		if (g.profile != profile) return;
		double t, d;
		closest_t(g.q, p.x, p.y, t, d);
		if (d < near) {
			near = d;
			best = seg;
		}
	});
	return best;
}
} // namespace

int plant_parks(CityState &s, const ScenicCity &city, uint32_t seed) {
	const ScenicTrees &t = city.trees;
	const TreeBrushDef &def = s.catalog.nature.trees;
	const float mid = (t.low_m + t.high_m) / 2;
	const TreeBrush brush{ 0, mid, (t.high_m - t.low_m) / std::max(1.f, t.high_m + t.low_m), t.spacing_m };
	int planted = 0;
	for (size_t r = 0; r < city.parks.size(); r++) {
		const Polyline &ring = city.parks[r];
		V2 lo(BIG, BIG), hi(-BIG, -BIG);
		for (const V2 &p : ring) {
			lo = V2(std::min(lo.x, p.x), std::min(lo.y, p.y));
			hi = V2(std::max(hi.x, p.x), std::max(hi.y, p.y));
		}
		const uint32_t park = hash_combine(seed ^ PARK_SALT, uint32_t(r));
		for (int i = 0; lo.x + float(i) * t.spacing_m <= hi.x; i++)
			for (int j = 0; lo.y + float(j) * t.spacing_m <= hi.y; j++) {
				if (s.nature.planted.live_trees() >= def.max_planted) return planted;
				const uint32_t h = hash_combine(park, hash_combine(uint32_t(i), uint32_t(j)));
				const V2 p(lo.x + (float(i) + (hash_float(h + STREAM_X) - 0.5f) * t.jitter) * t.spacing_m,
						lo.y + (float(j) + (hash_float(h + STREAM_Z) - 0.5f) * t.jitter) * t.spacing_m);
				if (!ring_holds(ring, p) || value_noise(p.x / t.grove_cell_m, p.y / t.grove_cell_m) < t.grove_cut) continue;
				if (s.terrain.is_water(p.x, p.y, -t.road_clear_m) || near_road(s, p, t.road_clear_m) || on_building(s, p, t.road_clear_m)) continue;
				s.nature.planted.add(one_tree(TreeKind::MIXED, brush, p.x, p.y, h, def));
				planted++;
			}
	}
	return planted;
}

int name_scenic_roads(CityState &s, const ScenicCity &city) {
	int named = 0;
	for (const ScenicRoad &road : city.roads) {
		if (road.name.empty()) continue;
		const Polyline line = scenic_road_line(city, road);
		for (float share : LOOK_SHARES) {
			const V2 at = along(line, share);
			const int seg = street_of(s, at, road.profile, SEEK_RADIUS_M);
			if (seg < 0) continue;
			StreetItem it;
			double x, z, y;
			footway_point(s.roads, seg, s.roads.segs[size_t(seg)].len / 2, 0.f, x, z, y);
			it.kind = ItemKind::STREETNAME;
			it.x = float(x);
			it.z = float(z);
			it.text = road.name;
			s.street.add_item(it);
			named++;
			break;
		}
	}
	return named;
}

} // namespace city
