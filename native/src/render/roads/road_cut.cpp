// The place a road job cuts out of the city (road_cut.h).
#include "render/roads/road_cut.h"

#include "core/grid.h"
#include "render/owned_segments.h"
#include "render/roads/road_mesh.h"
#include "sim/roads/road_grade.h"
#include "sim/terrain/road_shape.h"

#include <algorithm>

namespace city {

float road_reach(const Catalog &catalog, const StreetLook &look) {
	const Tuning &tn = catalog.tune;
	float widest = 0;
	for (const Profile &p : catalog.profiles) widest = std::max(widest, p.half());
	const float walls = cut_wall_reach(); // road_walls.cpp held_ground
	const float gates = widest + catalog.traffic.kerb_gate_reach_m; // lot_access.cpp street_gates
	const float verge = look.verge_reach; // road_verge.cpp ground_at
	return CASING_BAND_M + std::max({ walls, stays_reach(), structure_reach(tn), gates, verge }) + tn.world_height_spacing_m;
}

CityCut cut_for_roads(const CityState &s, const StreetLook &look, int64_t key, float size, unsigned parts) {
	const float x0 = float(key_x(key)) * size, z0 = float(key_y(key)) * size;
	const auto box_of = [&](const CityState &live) {
		CutBox box{ x0, z0, x0 + size, z0 + size };
		live.roads.for_segments_near(x0, z0, x0 + size, z0 + size, [&](int sid) {
			const RoadSegment &g = live.roads.segs[size_t(sid)];
			if (segment_chunk(g, size) == key) box.take(g.minx, g.minz, g.maxx, g.maxz); // owned: drawn whole from here
		});
		return box.grown(road_reach(live.catalog, look));
	};
	// The car gates where kerbs drop, of each segment the job lowers kerbs on: those it owns and every arm of a node it draws (a
	// junction's corner meets its arms' footways at their mouths, kerb_drop.h), whose lots may stand past the box.
	const auto keep = [&](CityCut &cut, const CityState &live) {
		if (!(parts & CUT_GATES)) return;
		live.roads.for_segments_near(x0, z0, x0 + size, z0 + size, [&](int sid) {
			if (segment_chunk(live.roads.segs[size_t(sid)], size) == key) keep_gates(cut, live, sid);
		});
		live.roads.node_hash.query(x0, z0, x0 + size, z0 + size, [&](int n) {
			const RoadNode &nd = live.roads.nodes[size_t(n)];
			if (!nd.alive || nd.x < x0 || nd.z < z0 || nd.x >= x0 + size || nd.z >= z0 + size) return;
			for (int arm : nd.segs) keep_gates(cut, live, arm);
		});
	};
	return cut_city(s, parts, box_of, keep);
}

} // namespace city
