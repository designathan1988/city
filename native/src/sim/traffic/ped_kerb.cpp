// Waiting at the kerb of a crosswalk. No place is handed out. Those about to cross wait on the right of the zebra's middle
// line as they will cross it, from a body clear of that line out to kerb_wait_half_m or the zebra's edge (the waiting span):
// the left is where those coming from the other kerb arrive, so the two ways never meet in a crowd. The way to the kerb has two
// legs: to an approach point back from the kerb along the way across (as far back as the crossed arm's footway leaves with an
// aisle behind it), reached at the place along the kerb the walker comes from when that lies inside the span, else at the span's
// nearer edge: the ways of those who come from past an edge converge on it, as people gather at a crosswalk, and their bodies are
// kept apart on the way by the crowd's one rule for the room between two walkers (ped_apart.h);
// then the queue: a leg begun parallel to the way across from where the walker stands. On the queue the room
// across is the span along the kerb (ped_stripes.cpp line_frame) and the leg ends at the kerb stand, so the crowd step stops a
// walker at the kerb or behind those already waiting, beside them where the span has room, and never on the asphalt. At the
// leg's end it asks for the crossing (pedestrians.cpp), and with the leave it crosses on a line parallel to the zebra's middle
// from where it stands, stepping into its own right half of the zebra as it goes.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr float ZERO_SPAN_M = 1e-4f; // a way across shorter than this has no direction

// The unit direction of the way across from `kerb` to `far`; false when it has none.
bool way_across(const Waypoint &kerb, const Waypoint &far, V2 &d) {
	d = V2(far.x - kerb.x, far.z - kerb.z);
	const float len = d.len();
	if (len < ZERO_SPAN_M) return false;
	d = d * (1.f / len);
	return true;
}

void move_along_kerb(Waypoint &kerb, Waypoint &far, const V2 &right, float by) {
	kerb.x += right.x * by;
	kerb.z += right.y * by;
	far.x += right.x * by;
	far.z += right.y * by;
}

// How far back from the kerb the queue reaches: what the crossed arm's footway leaves with an aisle behind it, a body at least.
float queue_depth(const CityState &s, const Waypoint &kerb, int arm) {
	const TrafficParams &p = s.catalog.traffic;
	const Profile &pf = s.roads.profile_of(arm);
	const StripEdge &e = kerb.side == 1 ? pf.edges.back() : pf.edges.front();
	const float aisle = 2 * p.ped_body_m;
	return std::max(p.ped_body_m, (e.u1 - e.u0) - p.ped_margin_m - p.kerb_stand_m - p.ped_body_m - aisle);
}

} // namespace

KerbSpan kerb_span(const CityState &s) {
	const TrafficParams &p = s.catalog.traffic;
	const float out = std::max(p.ped_body_m, std::min(p.kerb_wait_half_m, s.catalog.furniture.crossing.half() - p.ped_body_m));
	return { (p.ped_body_m + out) * 0.5f, (out - p.ped_body_m) * 0.5f };
}

bool toward_kerb(const Pedestrian &w) { return w.wi + 1 < w.wn && w.ways[w.wi].cross < 0 && w.ways[w.wi + 1].cross >= 0; }

int queue_before_kerb(const CityState &s, Waypoint *ways, int n) {
	Waypoint kerb = ways[n - 2], far = ways[n - 1];
	V2 d;
	if (!way_across(kerb, far, d)) return n;
	move_along_kerb(kerb, far, d.perp(), kerb_span(s).middle); // the right of the way across (left is (d.z, -d.x), lane_geometry.cpp)
	const float back = queue_depth(s, kerb, far.cross);
	Waypoint approach = kerb;
	approach.x -= d.x * back;
	approach.z -= d.y * back;
	ways[n - 2] = approach;
	ways[n - 1] = kerb;
	ways[n] = far;
	return n + 1;
}

bool toward_queue(const Pedestrian &w) {
	return w.wi + 2 < w.wn && w.ways[w.wi].cross < 0 && w.ways[w.wi + 1].cross < 0 && w.ways[w.wi + 2].cross >= 0;
}

void onto_approach(const CityState &s, Pedestrian &w) {
	Waypoint &approach = w.ways[w.wi], &kerb = w.ways[w.wi + 1], &far = w.ways[w.wi + 2];
	const KerbSpan span = kerb_span(s);
	V2 d;
	w.line_off = -span.middle;
	if (!way_across(kerb, far, d)) return;
	const V2 right = d.perp();
	const float aside = clampf((w.cur.x - approach.x) * right.x + (w.cur.z - approach.z) * right.y, -span.room, span.room);
	approach.x += right.x * aside;
	approach.z += right.y * aside;
	move_along_kerb(kerb, far, right, aside);
	w.line_off -= aside;
}

void onto_queue(const CityState &s, Pedestrian &w) {
	Waypoint &kerb = w.ways[w.wi], &far = w.ways[w.wi + 1];
	const KerbSpan span = kerb_span(s);
	V2 d;
	if (!way_across(kerb, far, d)) return;
	const V2 right = d.perp();
	const float held = -w.line_off - span.middle; // where along the span its way lies already, from the span's middle
	const float aside = clampf((w.cur.x - kerb.x) * right.x + (w.cur.z - kerb.z) * right.y, -span.room - held, span.room - held);
	move_along_kerb(kerb, far, right, aside);
	w.line_off -= aside;
}

void cross_from_here(Pedestrian &w) {
	Waypoint &kerb = w.ways[w.wi - 1], &far = w.ways[w.wi];
	V2 d;
	if (!way_across(kerb, far, d)) return;
	const V2 right = d.perp();
	const float aside = (w.cur.x - kerb.x) * right.x + (w.cur.z - kerb.z) * right.y; // where the queue left it, from its leg
	move_along_kerb(kerb, far, right, aside);
	w.line_off -= aside;
}

} // namespace traffic
} // namespace city
