// The lots graded to their streets (lot_sites.h).
#include "sim/commands/lot_sites.h"

#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float FRONT_LOOK_M = 0.3f; // the footway is looked for this far outside the lot's front line
constexpr double FOOTWAY_FIND_M = 0.25; // how far off a sidewalk's band that point may lie
constexpr float HALF = 0.5f;
constexpr float LEVEL_STEP_M = 2.f; // a lot's level is taken from the footway this often along its front (the road's profile is smooth)
// Where along its front, in frontages from its middle, a lot looks for the sidewalk it faces: the middle first, then out to its corners.
constexpr float FRONT_SHARES[] = { 0.f, -0.25f, 0.25f, -0.45f, 0.45f };
// A lot's middle stands at most this far from the street its front meets: no lot is deeper than twice it.
constexpr float FRONT_REACH_M = 100.f;

// The graded ground of building `id`'s lot, or false: no building there, or no sidewalk in front of it (a lot on a corner's mouth or
// off every street keeps the land).
bool site_of(const CityState &s, int id, LotShape &out) {
	const Building &rec = s.buildings.items[size_t(id)];
	if (!rec.alive) return false;
	const b40::LotFrame f = b40::lot_frame(rec);
	// The sidewalk outside the front: from its middle out to its corners (a lot's front may meet a corner's mouth in part).
	FootwayHit hit;
	for (float share : FRONT_SHARES) {
		const V2 front = f.at(share * rec.w, -FRONT_LOOK_M);
		hit = footway_at(s.roads, front.x, front.y, FOOTWAY_FIND_M);
		if (hit.ok && hit.side >= 0) break;
	}
	if (!hit.ok || hit.side < 0) return false;
	const Tuning &tn = s.catalog.tune;
	// The top of the footway `x` metres along the lot's front, outside it, less the plate the lot's covers stand on: the sidewalk
	// found there, else the place on the street found above.
	const auto level_at = [&](float x) {
		const V2 q = f.at(x, -FRONT_LOOK_M);
		FootwayHit at = footway_at(s.roads, q.x, q.y, FOOTWAY_FIND_M);
		if (!at.ok || at.side < 0) {
			float past;
			at = hit;
			segment_place(s.roads, hit.seg, q.x, q.y, at.v, at.u, past);
		}
		const Profile &p = s.roads.profile_of(at.seg);
		const StripEdge &walk = at.side == 0 ? p.edges.front() : p.edges.back();
		double wx, wz, y;
		footway_point(s.roads, at.seg, at.v, at.u, wx, wz, y);
		return float(y) + (walk.strip.flush ? 0.f : tn.street_sidewalk_m) - tn.lot_plate_m;
	};
	LotShape r;
	r.ox = f.o.x;
	r.oz = f.o.y;
	r.ax = f.e0.x;
	r.az = f.e0.y;
	r.bx = f.e1.x;
	r.bz = f.e1.y;
	r.w = rec.w;
	r.d = rec.d;
	// the footway's profile along the frontage, a level every LEVEL_STEP_M (two at least): the lot follows its street
	const int steps = std::max(1, int(std::ceil(rec.w / LEVEL_STEP_M)));
	for (int k = 0; k <= steps; k++) r.levels.push_back(level_at(-rec.w * HALF + rec.w * float(k) / float(steps)));
	r.minx = r.minz = r.rx0 = r.rz0 = std::numeric_limits<float>::max();
	r.maxx = r.maxz = r.rx1 = r.rz1 = std::numeric_limits<float>::lowest();
	for (float x : { -rec.w * HALF, rec.w * HALF })
		for (float y : { 0.f, rec.d }) { // the lot's own rectangle, from its front: the street in front is the road's ground
			const V2 c = f.at(x, y);
			r.minx = std::min(r.minx, c.x);
			r.maxx = std::max(r.maxx, c.x);
			r.minz = std::min(r.minz, c.y);
			r.maxz = std::max(r.maxz, c.y);
			r.rx0 = std::min(r.rx0, c.x);
			r.rx1 = std::max(r.rx1, c.x);
			r.rz0 = std::min(r.rz0, c.y);
			r.rz1 = std::max(r.rz1, c.y);
		}
	const float grow = tn.lot_apron_m + tn.lot_bank_reach_m;
	r.minx -= grow;
	r.minz -= grow;
	r.maxx += grow;
	r.maxz += grow;
	out = r;
	return true;
}

// What stands on the ground in the box sits again: the buildings (their floors), the roads' verges and the trees and ground cover.
void mark_stands(CityState &s, float minx, float minz, float maxx, float maxz) {
	for_chunks(minx, minz, maxx, maxz, [&](int64_t k) {
		s.log.building_chunks.push_back(k);
		s.log.road_chunks.push_back(k);
		s.log.nature_chunks.push_back(k);
	});
}
} // namespace

void settle_sites(CityState &s, const std::vector<int> &streets) {
	std::vector<int> due;
	due.swap(s.buildings.sites_due);
	for (int seg : streets) {
		if (seg < 0 || seg >= int(s.roads.segs.size())) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)]; // a road gone keeps its box: the lots that faced it keep the land now
		s.buildings.query(g.minx - FRONT_REACH_M, g.minz - FRONT_REACH_M, g.maxx + FRONT_REACH_M, g.maxz + FRONT_REACH_M, [&](int id) { due.push_back(id); });
	}
	if (due.empty()) return;
	ChangeLog::dedup(due);
	ComposeHold hold(s.terrain); // every lot graded, composed once where they overlap
	for (int id : due) {
		if (id < 0 || id >= int(s.buildings.items.size())) continue;
		const std::vector<LotShape> &shapes = s.terrain.lots.shapes;
		const LotShape *was = id < int(shapes.size()) && shapes[size_t(id)].alive ? &shapes[size_t(id)] : nullptr;
		if (was) mark_stands(s, was->minx, was->minz, was->maxx, was->maxz);
		LotShape r;
		if (!site_of(s, id, r)) {
			s.terrain.drop_lot_shape(id);
			continue;
		}
		mark_stands(s, r.minx, r.minz, r.maxx, r.maxz);
		s.terrain.put_lot_shape(id, r);
	}
}

} // namespace city
