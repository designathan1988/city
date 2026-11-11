// Every lot's front against the footway it faces (CityProbes::lot_fronts): the lot's covers as they are drawn there (the ground
// 0.3 m inside its front at a quarter, the middle and three quarters of its frontage, plus the lot's plate: render/buildings
// lot_emit.cpp) held against the footway's top 0.3 m outside (sim/street/footway.h footway_at: the road's level there, plus the
// sidewalk's height where it is raised). A lot meets the footway it faces at the footway's top: no step down and no bank between.
#include "api/city_probes.h"

#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cmath>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float FRONT_STEP_M = 0.3f; // the two heights are read this far inside and outside the lot's front line
constexpr float FRONT_SHARES[] = { -0.25f, 0.f, 0.25f }; // along the frontage from its middle, in frontages
constexpr double FOOTWAY_FIND_M = 0.25; // how far off a sidewalk's band the outside point may lie
constexpr float LEVEL_EPS_M = 0.01f; // a lot this close to the footway's top meets it
} // namespace

Dictionary CityProbes::lot_fronts() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Tuning &tn = s.catalog.tune;
	int fronts = 0, off = 0;
	float worst = 0;
	Array first;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const b40::LotFrame f = b40::lot_frame(rec);
		for (float share : FRONT_SHARES) {
			const V2 out = f.at(share * rec.w, -FRONT_STEP_M), in = f.at(share * rec.w, FRONT_STEP_M);
			const FootwayHit hit = footway_at(s.roads, out.x, out.y, FOOTWAY_FIND_M);
			if (!hit.ok || hit.side < 0) continue; // no sidewalk in front of it there (a corner's mouth, a median)
			const Profile &p = s.roads.profile_of(hit.seg);
			const Strip &edge = hit.side == 0 ? p.edges.front().strip : p.edges.back().strip;
			double x, z, y;
			footway_point(s.roads, hit.seg, hit.v, hit.u, x, z, y);
			const float top = float(y) + (edge.flush ? 0.f : tn.street_sidewalk_m);
			const float lot = s.terrain.height_at(in.x, in.y) + tn.lot_plate_m;
			fronts++;
			const float gap = lot - top;
			if (std::fabs(gap) <= LEVEL_EPS_M) continue;
			off++;
			if (std::fabs(gap) > std::fabs(worst)) {
				worst = gap;
				first = Array::make(id, in.x, in.y, gap);
			}
		}
	}
	Dictionary d;
	d["fronts"] = fronts; // points of lot fronts with a sidewalk outside them
	d["off"] = off; // of them, those whose lot does not meet the footway's top
	d["worst"] = first; // building, x, z, the lot's surface less the footway's top (m)
	return d;
}

} // namespace city
