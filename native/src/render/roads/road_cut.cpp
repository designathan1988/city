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
	return cut_city(s, parts, [&](const CityState &live) {
		const float x0 = float(key_x(key)) * size, z0 = float(key_y(key)) * size;
		CutBox box{ x0, z0, x0 + size, z0 + size };
		live.roads.for_segments_near(x0, z0, x0 + size, z0 + size, [&](int sid) {
			const RoadSegment &g = live.roads.segs[size_t(sid)];
			if (segment_chunk(g, size) == key) box.take(g.minx, g.minz, g.maxx, g.maxz); // owned: drawn whole from here
		});
		return box.grown(road_reach(live.catalog, look));
	});
}

} // namespace city
