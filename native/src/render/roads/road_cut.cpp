// The place a road job cuts out of the city (road_cut.h).
#include "render/roads/road_cut.h"

#include "core/grid.h"
#include "sim/roads/road_grade.h"
#include "sim/terrain/road_shape.h"

#include <algorithm>

namespace city {

float road_reach(const Catalog &catalog, const StreetLook &look) {
	const Tuning &tn = catalog.tune;
	const StructureShape &K = look.shapes.road_structures;
	float widest = 0;
	for (const Profile &p : catalog.profiles) widest = std::max(widest, p.half());
	const float walls = cut_wall_reach(); // road_walls.cpp held_ground
	const float legs = look.shapes.road_stays.leg_out_m; // road_stays.cpp tower
	const float wings = K.jamb_m + tn.structure_portal_wing_m + tn.structure_portal_thickness_m + K.portal_reach_m; // road_structures.cpp portal
	const float piers = std::max(tn.structure_pier_radius_bridge_m, tn.structure_pier_radius_elevated_m) * tn.structure_pier_max_width * K.pier_flare; // on_ground_road
	const float gates = widest + catalog.traffic.kerb_gate_reach_m; // lot_access.cpp street_gates
	return CASING_BAND_M + std::max({ walls, legs, wings, piers, gates }) + tn.world_height_spacing_m;
}

CityCut cut_for_roads(const CityState &s, const StreetLook &look, int64_t key, float size, unsigned parts) {
	return cut_city(s, parts, [&](const CityState &live) {
		const float x0 = float(key_x(key)) * size, z0 = float(key_y(key)) * size;
		CutBox box{ x0, z0, x0 + size, z0 + size };
		live.roads.for_segments_near(x0, z0, x0 + size, z0 + size, [&](int sid) {
			const RoadSegment &g = live.roads.segs[size_t(sid)];
			double mx, mz;
			g.q.at(0.5, mx, mz);
			if (chunk_of(float(mx), float(mz), size) == key) box.take(g.minx, g.minz, g.maxx, g.maxz); // owned: drawn whole from here
		});
		return box.grown(road_reach(live.catalog, look));
	});
}

} // namespace city
