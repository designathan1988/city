// Where walkers go (Roadcraft peds.ts spawnRoute/planCorner, routing.ts walking A*): a route of nodes over streets
// with a sidewalk (cost length / pace + a fixed extra, heuristic straight distance / pace), and the way round each
// node: from the corner it stands on, round the corners and over the crosswalks of the arms in between (the
// rotation with the fewest crossings, never over a highway or a ramp: the route asks for a way round each node it passes) to the start
// of the next sidewalk; round a corner with no crosswalk, along the kerb as it is
// drawn there, at the walker's own distance from it (the reference game's walking lines). A crosswalk is the middle line of the painted
// zebra past the junction setback (catalog street_furniture.json "crossing"), from kerb to kerb; each is walked on its
// right, by way of an approach point and a queue before its kerb (ped_kerb.cpp).
#include "sim/traffic/traffic_system.h"

#include "sim/street/footway.h"
#include "sim/traffic/astar.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {


constexpr size_t JUNCTION_ARMS = 2; // a node of more arms than this is a junction

V2 node_xz(const CityState &s, int n) { return V2(float(s.roads.nodes[n].x), float(s.roads.nodes[n].z)); }

struct Arm {
	int seg = -1;
	bool at_a = true; // the segment starts at the node (out-direction = a -> b)
	float angle = 0;
};

// Arms of node `n` with a sidewalk or a carriageway, by angle of their out-direction (increasing), into `arms`.
void arms_by_angle(const CityState &s, int n, std::vector<Arm> &arms) {
	arms.clear();
	for (int seg : s.roads.nodes[n].segs) {
		const RoadSegment &g = s.roads.segs[seg];
		if (!g.alive || s.roads.profile_of(seg).rail) continue;
		Arm a;
		a.seg = seg;
		a.at_a = g.a == n;
		Pose p;
		float arc = a.at_a ? float(g.retreat_a) : float(g.len - g.retreat_b);
		sidewalk_pose(s, seg, 0, arc, 0.f, p);
		a.angle = a.at_a ? std::atan2(p.fz, p.fx) : std::atan2(-p.fz, -p.fx);
		arms.push_back(a);
	}
	std::sort(arms.begin(), arms.end(), [](const Arm &x, const Arm &y) { return x.angle < y.angle; });
}

// The side (0 right / 1 left of a -> b) of arm `a` that faces the next arm by angle (the right of its out-direction).
int side_to_next(const Arm &a) { return a.at_a ? 0 : 1; }

// Whether the way round from corner `from`, `steps` corners in direction `dir` (+1 by increasing angle, -1 decreasing), crosses
// only arms walkers may cross (profiles.cpp foot_crossing): none goes over a highway or a ramp at grade.
bool open_round(const CityState &s, const std::vector<Arm> &arms, int from, int dir, int steps) {
	const int deg = int(arms.size());
	for (int k = 0, cur = from; k < steps; k++) {
		const int arm = dir > 0 ? (cur + 1) % deg : cur;
		if (!s.roads.profile_of(arms[size_t(arm)].seg).foot_crossing) return false;
		cur = dir > 0 ? (cur + 1) % deg : (cur - 1 + deg) % deg;
	}
	return true;
}

// The corner a walker on `side` of arm `a` (index `k` of `deg` arms) stands on: corner k lies between arm k and arm k + 1.
int standing_corner(const Arm &a, int k, int side, int deg) { return side == side_to_next(a) ? k : (k - 1 + deg) % deg; }

// A kerb point of the crosswalk over arm `a`, on `side`.
Waypoint kerb_point(const CityState &s, const Arm &a, int side) {
	const RoadSegment &g = s.roads.segs[a.seg];
	const Profile &p = s.roads.profile_of(a.seg);
	const TrafficParams &tp = s.catalog.traffic;
	const float at = s.catalog.furniture.crossing.middle();
	float arc = a.at_a ? float(g.retreat_a) + at : float(g.len - g.retreat_b) - at;
	float u = side == 0 ? -(p.asph_right + tp.kerb_stand_m) : p.asph_left + tp.kerb_stand_m;
	Pose o;
	sidewalk_pose(s, a.seg, 0, arc, u - p.walk_right, o);
	return Waypoint{ o.x, o.y, o.z, -1, side };
}

bool has_side(const Profile &p, int side) { return side == 0 ? p.has_walk_right : p.has_walk_left; }

// Whether a walker who came to node `n` from node `came` (-1: starts there) can go on along `seg_out` round the node, from either
// sidewalk to either, without crossing an arm closed to walkers. Most nodes have none and answer at once.
bool way_round(const CityState &s, int n, int came, int seg_out) {
	bool closed = false;
	for (int seg : s.roads.nodes[size_t(n)].segs) closed = closed || (s.roads.segs[size_t(seg)].alive && !s.roads.profile_of(seg).foot_crossing);
	const int seg_in = closed && came >= 0 ? seg_between(s, came, n) : -1;
	if (seg_in < 0) return true;
	static thread_local std::vector<Arm> arms;
	arms_by_angle(s, n, arms);
	const int deg = int(arms.size());
	int ai = -1, aj = -1;
	for (int k = 0; k < deg; k++) {
		if (arms[size_t(k)].seg == seg_in) ai = k;
		if (arms[size_t(k)].seg == seg_out) aj = k;
	}
	if (ai < 0 || aj < 0) return false;
	const Profile &pi = s.roads.profile_of(seg_in), &po = s.roads.profile_of(seg_out);
	for (int side_in = 0; side_in < 2; side_in++) {
		if (!has_side(pi, side_in)) continue;
		const int ci = standing_corner(arms[size_t(ai)], ai, side_in, deg);
		for (int side_out = 0; side_out < 2; side_out++) {
			if (!has_side(po, side_out)) continue;
			const int cj = standing_corner(arms[size_t(aj)], aj, side_out, deg);
			if (open_round(s, arms, ci, 1, (cj - ci + deg) % deg) || open_round(s, arms, ci, -1, (ci - cj + deg) % deg)) return true;
		}
	}
	return false;
}

constexpr float RETURN_STEP_RAD = PI_F / 12; // round a kerb return, a point every this much of its turn
constexpr int CURVE_POINTS = 5; // along the curve round any other corner
constexpr float CONTROL_REACH = 2; // two walking lines meeting further ahead than this many times the gap between their ends: near parallel
constexpr float SHORTEST_LEG_M = 0.05f; // a straight leg shorter than this is none

// One end of the way round a corner: where it is, its height, and the walking line's direction there (into the node at the
// sidewalk it leaves, out of it at the one it takes).
struct CornerEnd {
	V2 p, d;
	float y = 0;
};

// Round a kerb return about its centre `c`: straight on from where it leaves its sidewalk to the arc, along the arc (its
// distance from the centre blended from the one end's to the other's: the walker's own distance from the kerb at each), straight
// on from the arc to the next sidewalk's start. The points before that start, how many.
int round_return(const CornerEnd &from, const CornerEnd &to, const V2 &c, Waypoint *out, int room) {
	const float sa = std::max(0.f, (c - from.p).dot(from.d)), sb = std::max(0.f, (to.p - c).dot(to.d));
	const V2 fa = from.p + from.d * sa, fb = to.p - to.d * sb; // where each walking line touches the arc
	const V2 ra = fa - c, rb = fb - c;
	const float a0 = std::atan2(ra.y, ra.x), la = ra.len(), lb = rb.len();
	float sweep = std::atan2(rb.y, rb.x) - a0;
	sweep += sweep > PI_F ? -TAU_F : sweep < -PI_F ? TAU_F : 0.f;
	const int steps = std::clamp(int(std::ceil(std::fabs(sweep) / RETURN_STEP_RAD)), 1, room - 1);
	int n = 0;
	if (sa > SHORTEST_LEG_M) out[n++] = Waypoint{ fa.x, from.y, fa.y, -1 };
	for (int k = 1; k < steps && n < room; k++) {
		const float f = float(k) / float(steps), r = lerpf(la, lb, f), a = a0 + sweep * f;
		out[n++] = Waypoint{ c.x + std::cos(a) * r, lerpf(from.y, to.y, f), c.y + std::sin(a) * r, -1 };
	}
	if (sb > SHORTEST_LEG_M && n < room) out[n++] = Waypoint{ fb.x, to.y, fb.y, -1 };
	return n;
}

// The way round the corner of node `node` a walker comes off its sidewalk to, with no crosswalk on it, as the kerb is drawn there
// (render/roads/road_junction.cpp corner) at the walker's own distance from it: round a kerb return along its arc; round any other
// corner along the curve whose control is where its two walking lines meet, ahead of both ends; lines that meet behind, or near
// parallel, need nothing between their ends. The points before the next sidewalk's start, how many.
int round_corner(const CityState &s, int node, const CornerEnd &from, const CornerEnd &to, Waypoint *out, int room) {
	double cx = 0, cz = 0, sa = 0, sb = 0;
	if (JunctionKerbs(s.roads, node).kerb_return(from.p.x, from.p.y, to.p.x, to.p.y, cx, cz))
		return round_return(from, to, V2(float(cx), float(cz)), out, room);
	const float gap = (to.p - from.p).len();
	if (!line_line(from.p.x, from.p.y, from.d.x, from.d.y, to.p.x, to.p.y, to.d.x, to.d.y, sa, sb) || sa <= 0 || sb >= 0 ||
			sa > CONTROL_REACH * gap || -sb > CONTROL_REACH * gap)
		return 0;
	const V2 meet = from.p + from.d * float(sa);
	int n = 0;
	for (int k = 1; k <= CURVE_POINTS && n < room; k++) {
		const float f = float(k) / float(CURVE_POINTS + 1), g = 1 - f;
		const V2 q = from.p * (g * g) + meet * (2 * g * f) + to.p * (f * f);
		out[n++] = Waypoint{ q.x, lerpf(from.y, to.y, f), q.y, -1 };
	}
	return n;
}

} // namespace

int seg_between(const CityState &s, int a, int b) {
	for (int seg : s.roads.nodes[a].segs) {
		const RoadSegment &g = s.roads.segs[seg];
		if (g.alive && ((g.a == a && g.b == b) || (g.b == a && g.a == b)) && walkable(s, seg)) return seg;
	}
	return -1;
}

bool walkable(const CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	const Profile &p = s.roads.profile_of(seg);
	return g.alive && !p.rail && p.walk() && g.len - g.retreat_b > g.retreat_a;
}

bool find_walk_route(const CityState &s, int from, int goal, std::vector<int> &out) {
	static thread_local AStarScratch sc;
	const TrafficParams &tp = s.catalog.traffic;
	const RoadNetwork &net = s.roads;
	const float pace = tp.walk_speed_min;
	const V2 goal_p = node_xz(s, goal);
	auto h = [&](int k) { return (node_xz(s, k) - goal_p).len() / pace; };
	auto expand = [&](int k, auto relax) {
		const int came = k == from ? -1 : sc.parent[size_t(k)]; // k was reached from there: the way round k starts on that street
		for (int seg : net.nodes[k].segs) {
			if (!walkable(s, seg) || !way_round(s, k, came, seg)) continue;
			const RoadSegment &g = net.segs[seg];
			// the fixed extra is a junction's: a road cut in pieces, or with a crossing on it, is no longer than the parallel one
			relax(g.a == k ? g.b : g.a, float(g.len) / pace + (net.nodes[k].segs.size() > JUNCTION_ARMS ? tp.walk_cost_extra_s : 0.f));
		}
	};
	return astar(sc, net.nodes.size(), from, goal, int(tp.ped_route_max_expanded), h, expand, out);
}

int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, int want_side, Waypoint *ways, int &side_out) {
	static thread_local std::vector<Arm> arms;
	arms_by_angle(s, n, arms);
	const int deg = int(arms.size());
	int ai = -1, aj = -1;
	for (int k = 0; k < deg; k++) {
		if (arms[k].seg == seg_in) ai = k;
		if (arms[k].seg == seg_out) aj = k;
	}
	if (ai < 0 || aj < 0) return -1;
	// corner k lies between arm k and arm k + 1; a walker on arm i's side facing the next arm is on corner i
	const int ci = standing_corner(arms[ai], ai, side_in, deg);
	const Profile &po = s.roads.profile_of(seg_out);
	int best = -1, best_steps = deg + 1, best_dir = 1;
	for (int pick = 0; pick < 2; pick++) {
		int cj = pick == 0 ? aj : (aj - 1 + deg) % deg;
		int side = pick == 0 ? side_to_next(arms[aj]) : 1 - side_to_next(arms[aj]);
		if (!has_side(po, side) || (want_side >= 0 && has_side(po, want_side) && side != want_side)) continue;
		const int ccw = (cj - ci + deg) % deg, cw = (ci - cj + deg) % deg;
		// the shorter way round, unless it goes over a road walkers may not cross
		const bool up = open_round(s, arms, ci, 1, ccw), down = open_round(s, arms, ci, -1, cw);
		if (!up && !down) continue;
		const int steps = up && down ? std::min(ccw, cw) : up ? ccw : cw;
		if (steps < best_steps || (steps == best_steps && (n & 1) == 0)) {
			best = pick;
			best_steps = steps;
			best_dir = up && down ? (ccw <= cw ? 1 : -1) : up ? 1 : -1;
			side_out = side;
		}
	}
	if (best < 0) return -1;
	if (s.roads.mid_block(n) && best_steps > 0) { // over the arm that has the crosswalk
		const int over = arms[size_t((ci + 1) % deg)].seg == s.roads.crossing_arm(n) ? 1 : -1;
		if (open_round(s, arms, ci, over, best_steps)) best_dir = over;
	}
	int w = 0, cur = ci;
	for (int k = 0; k < best_steps && w + 4 <= MAX_WAYS; k++) { // a crossing's three points, and the next sidewalk's start after them
		// increasing: cross arm cur + 1 from its far side to the side facing the next arm; decreasing: arm cur back
		int arm = best_dir > 0 ? (cur + 1) % deg : cur;
		int to_next = side_to_next(arms[arm]);
		ways[w++] = kerb_point(s, arms[arm], best_dir > 0 ? 1 - to_next : to_next);
		Waypoint far = kerb_point(s, arms[arm], best_dir > 0 ? to_next : 1 - to_next);
		far.cross = arms[arm].seg;
		ways[w++] = far;
		w = queue_before_kerb(s, ways, w); // the way to that kerb: an approach point, then the queue (ped_kerb.cpp)
		cur = best_dir > 0 ? (cur + 1) % deg : (cur - 1 + deg) % deg;
	}
	return w;
}

void aim_onto_sidewalk(const CityState &s, Pedestrian &w) {
	Pose mid, start;
	sidewalk_pose(s, w.seg, w.side, w.s, 0.f, mid);
	const float room = walk_half(s, w.seg, w.side);
	const float across = clampf((w.cur.x - mid.x) * mid.fz - (w.cur.z - mid.z) * mid.fx, -room, room);
	sidewalk_pose(s, w.seg, w.side, w.s, across, start);
	const Waypoint at_start{ start.x, start.y, start.z, -1 };
	w.ways[w.wi] = at_start;
	if (w.wi > 0) return; // over a crosswalk first: from its far kerb, on the corner's footway
	// straight off its sidewalk round the corner: the way round it first
	const float out = w.dir == 0 ? 1.f : -1.f;
	const CornerEnd from{ V2(w.cur.x, w.cur.z), V2(w.way_x, w.way_z), w.cur.y }, to{ V2(start.x, start.z), V2(start.fx, start.fz) * out, start.y };
	const int n = round_corner(s, w.way_node, from, to, w.ways, MAX_WAYS - 1);
	w.ways[n] = at_start;
	w.wn = n + 1;
}

} // namespace traffic
} // namespace city
