// The roads' heights carried to the ground and to everything that stands on a road (road_grade_sync.h).
#include "sim/commands/road_grade_sync.h"

#include "sim/commands/road_edit.h"
#include "sim/roads/junction.h"
#include "sim/state/city_state.h"
#include "sim/terrain/road_shape.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double WATER_STEP_M = 4; // a road is looked at for water every this far (the water grid's half spacing)

// The ground a road is laid over: the natural heights (the player's sculpting included, the roads' shaping not), the
// water of a river or a lake standing as a floor, and the sea's level.
GroundAt ground_of(const Heightfield &t) {
	return [&t](float x, float z) {
		const float g = std::max(t.base_height_at(x, z), t.water_level);
		return std::max(g, t.water_at(x, z));
	};
}

// How high a road's outermost strip stands over its deck: a raised footway or kerb a kerb's rise, anything else (a flush footway,
// a shoulder, a lane, a barrier's foot) the deck itself.
float paving_rise(const Strip &s) {
	const bool raised = (s.kind == StripKind::SIDEWALK || s.kind == StripKind::CURB) && !s.flush;
	return raised ? FOOTWAY_RISE_M : 0.f;
}

// The shape the ground takes under segment s: its stations along the curve at their solved heights.
RoadShape shape_of(const RoadNetwork &net, int s) {
	const RoadSegment &g = net.segs[size_t(s)];
	const GradeProfile &p = net.grade.prof[size_t(s)];
	RoadShape r;
	r.alive = true;
	r.half = p.half;
	const Profile &pr = net.profile_of(s);
	r.edge = pr.half();
	if (!pr.edges.empty()) {
		r.rise[0] = paving_rise(pr.edges.front().strip);
		r.rise[1] = paving_rise(pr.edges.back().strip);
	}
	for (int k = 0; k < 2; k++) { // a dead end's paving stops at the node, a bulb's goes round it to its footway's outer ring
		r.cut[k] = p.ends[k] == RoadEndKind::CUT;
		r.disc[k] = p.ends[k] == RoadEndKind::BULB ? float(BULB_R + BULB_WALK) : 0.f;
	}
	r.kind = uint8_t((is_raised(p.structure) ? SHAPE_RAISED : 0) | (is_sunken(p.structure) ? SHAPE_SUNKEN : 0) |
			(p.manual ? SHAPE_MANUAL : 0) | (p.walls ? SHAPE_WALLS : 0));
	r.minx = r.minz = BIG;
	r.maxx = r.maxz = -BIG;
	for (size_t i = 0; i < p.h.size(); i++) {
		double x, z;
		g.q.at(t_at_length(g.table, double(p.step) * double(i)), x, z);
		r.pts.insert(r.pts.end(), { float(x), p.h[i], float(z) });
		r.minx = std::min(r.minx, float(x)); r.maxx = std::max(r.maxx, float(x));
		r.minz = std::min(r.minz, float(z)); r.maxz = std::max(r.maxz, float(z));
	}
	const float reach = p.half + road_shape_reach();
	r.minx -= reach; r.minz -= reach; r.maxx += reach; r.maxz += reach;
	return r;
}
} // namespace

bool crosses_water(const CityState &s, const Quad &q) {
	const int n = std::max(1, int(std::ceil(quad_length(q) / WATER_STEP_M)));
	for (int i = 0; i <= n; i++) {
		double x, z;
		q.at(double(i) / n, x, z);
		if (s.terrain.is_water(float(x), float(z), 0.f)) return true;
	}
	return false;
}

void sync_grade(CityState &s) {
	RoadNetwork &net = s.roads;
	const std::vector<float> moved_ground = s.terrain.take_ground_moved();
	for (size_t k = 0; k + 3 < moved_ground.size(); k += 4) {
		const float *r = &moved_ground[k];
		net.for_segments_near(r[0], r[1], r[2], r[3], [&](int seg) { net.grade.ground_changed(seg); });
	}
	if (!net.grade.pending()) return;
	std::vector<int> moved, gone;
	net.grade.solve(net, ground_of(s.terrain), moved, gone, s.jobs.get());
	ComposeHold hold(s.terrain); // every road moved, shaped once where they overlap
	for (int seg : gone) s.terrain.drop_road_shape(seg);
	for (int seg : moved) {
		const RoadShape shape = shape_of(net, seg);
		mark_ground(s, shape.minx, shape.minz, shape.maxx, shape.maxz); // what stands on the ground there sits again
		s.terrain.put_road_shape(seg, shape);
		net.mark_heights(seg);
	}
}

} // namespace city
