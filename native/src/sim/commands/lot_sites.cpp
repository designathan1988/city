// The lots graded to their streets (lot_sites.h).
#include "sim/commands/lot_sites.h"

#include "sim/buildings/b40/record.h"
#include "sim/roads/parking_layout.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>

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
constexpr float SHUT_STEP_M = 0.5f; // a front is read for a junction's kerb this often where its ends and middle disagree
constexpr float SHUT_EDGE_M = 0.02f; // a shut stretch's ends are found to this, the stretch taken the larger
constexpr float BESIDE_M = 1.f; // a point this far past a street's band, or nearer, lies beside that street
constexpr float SIDE_SHARES[] = { 0.25f, 0.5f, 0.75f }; // where along a line, in shares of its length, a street beside it is looked for

// What the street outside a lot's front is at a point: the segment the point lies beside (the one whose band is nearest, -1 none), and
// whether a car's gate (open[CAR]: a car may stand at its kerb there) and a person's (open[WALK]: off a crosswalk's landing and its post)
// may open onto it; a point ahead of a street's end with no street beside it lies in a junction.
struct FrontRead {
	int seg = -1;
	bool open[b40::LotEdges::KINDS] = { true, true };
};
FrontRead front_read(const RoadNetwork &net, const V2 &p) {
	const double reach = footway_reach(net, BESIDE_M);
	FrontRead r;
	float nearest = BESIDE_M;
	double at = 0;
	bool ahead = false;
	net.for_segments_near(p.x - reach, p.y - reach, p.x + reach, p.y + reach, [&](int seg) {
		if (!net.segs[size_t(seg)].alive) return;
		double v;
		float u, past;
		segment_place(net, seg, p.x, p.y, v, u, past);
		const float off = std::fabs(u) - net.profile_of(seg).half(); // past the band's outer edge (< 0: on it)
		if (off > BESIDE_M) return;
		if (past > BESIDE_M) {
			ahead = true;
			return;
		}
		if (r.seg >= 0 && off >= nearest) return;
		nearest = off;
		r.seg = seg;
		at = v;
	});
	if (r.seg >= 0) {
		r.open[b40::LotEdges::CAR] = kerb_standing(net, r.seg, at);
		r.open[b40::LotEdges::WALK] = kerb_walkable(net, r.seg, at);
	} else {
		r.open[b40::LotEdges::CAR] = r.open[b40::LotEdges::WALK] = !ahead;
	}
	return r;
}

// The stretches of a front no gate of `kind` opens on, as lot x from its middle (`read`: the street outside the front at x): read every
// SHUT_STEP_M, each change found to SHUT_EDGE_M, the stretch taken the larger; more than SPANS stretches, the two nearest joined.
template <typename Read>
void front_shut(const Read &read, float w, int kind, b40::LotEdges &out) {
	const float a = -w * HALF, b = w * HALF;
	std::vector<std::pair<float, float>> shut;
	const int n = std::max(1, int(std::ceil(w / SHUT_STEP_M)));
	float x0 = a, from = a;
	bool open0 = read(a).open[kind];
	for (int k = 1; k <= n; k++) {
		const float x1 = a + w * float(k) / float(n);
		const bool open1 = read(x1).open[kind];
		if (open1 != open0) {
			float lo = x0, hi = x1; // the change lies between
			while (hi - lo > SHUT_EDGE_M) {
				const float mid = (lo + hi) * HALF;
				(read(mid).open[kind] == open0 ? lo : hi) = mid;
			}
			if (open0) from = lo; // a shut stretch starts, from the last open point
			else shut.push_back({ from, hi }); // it ends, at the first open point
		}
		x0 = x1;
		open0 = open1;
	}
	if (!open0) shut.push_back({ from, b });
	while (shut.size() > size_t(b40::LotEdges::SPANS)) { // a gate keeps off both and what lies between
		size_t join = 0;
		for (size_t k = 1; k + 1 < shut.size(); k++)
			if (shut[k + 1].first - shut[k].second < shut[join + 1].first - shut[join].second) join = k;
		shut[join].second = shut[join + 1].second;
		shut.erase(shut.begin() + std::ptrdiff_t(join + 1));
	}
	for (size_t k = 0; k < shut.size(); k++) {
		out.from[kind][k] = shut[k].first;
		out.to[kind][k] = shut[k].second;
	}
}

// The front's stretches of both kinds. Its ends and middle settle a front beside one street whose kerb is open along it all for both (each
// kind's open kerb is one stretch of each street, the front straight); else each kind is read along it.
void front_stretches(const RoadNetwork &net, const b40::LotFrame &f, float w, b40::LotEdges &out) {
	const auto read = [&](float x) { return front_read(net, f.at(x, -FRONT_LOOK_M)); };
	const FrontRead ra = read(-w * HALF), rm = read(0), rb = read(w * HALF);
	bool open = ra.seg == rm.seg && rm.seg == rb.seg;
	for (int kind = 0; kind < b40::LotEdges::KINDS; kind++) open = open && ra.open[kind] && rm.open[kind] && rb.open[kind];
	if (open) return;
	for (int kind = 0; kind < b40::LotEdges::KINDS; kind++) front_shut(read, w, kind, out);
}

// The lines of a lot, besides its front, a street runs along: a sidewalk, or a road's surface, just outside its left side, its
// right side or its back (the line across from the front: a lot that runs through its block).
uint8_t street_sides(const RoadNetwork &net, const b40::LotFrame &f, float w, float d) {
	const auto beside = [&](float x, float y) {
		const V2 p = f.at(x, y);
		const FootwayHit hit = footway_at(net, p.x, p.y, FOOTWAY_FIND_M);
		return (hit.ok && hit.side >= 0) || on_road(net, p.x, p.y);
	};
	const auto any = [&](const auto &at) { return std::any_of(std::begin(SIDE_SHARES), std::end(SIDE_SHARES), at); };
	const float out_x = w * HALF + FRONT_LOOK_M;
	uint8_t out = 0;
	if (any([&](float share) { return beside(-out_x, share * d); })) out |= b40::LotEdges::LEFT_ON_STREET;
	if (any([&](float share) { return beside(out_x, share * d); })) out |= b40::LotEdges::RIGHT_ON_STREET;
	if (any([&](float share) { return beside((share - HALF) * w, d + FRONT_LOOK_M); })) out |= b40::LotEdges::BACK_ON_STREET;
	return out;
}

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

b40::LotEdges lot_edges(const CityState &s, const Building &rec) {
	b40::LotEdges e;
	const b40::LotFrame f = b40::lot_frame(rec);
	front_stretches(s.roads, f, rec.w, e);
	e.street = street_sides(s.roads, f, rec.w, rec.d);
	return e;
}

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
		const Building &rec = s.buildings.items[size_t(id)];
		if (rec.alive) { // what its edges meet, as the streets now stand: its body is made to them
			const b40::LotEdges edges = lot_edges(s, rec);
			if (edges != rec.edges || !rec.edges_read) s.buildings.set_edges(id, edges); // a record read from a file reads its edges now
		}
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
