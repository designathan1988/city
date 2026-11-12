// Where walkers go (Roadcraft peds.ts spawnRoute/planCorner, routing.ts walking A*): a route of nodes over streets
// with a sidewalk (cost length / pace + a fixed extra, heuristic straight distance / pace), and the way round each
// node: from the corner it stands on, round the corners and over the crosswalks of the arms in between (the
// rotation with the fewest crossings) to the start of the next sidewalk. A crosswalk is the middle line of the painted
// zebra past the junction setback (catalog street_furniture.json "crossing"), from kerb to kerb.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/route_search.h"

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

// Arms of node `n` with a sidewalk or a carriageway, by angle of their out-direction (increasing).
std::vector<Arm> arms_by_angle(const CityState &s, int n) {
	std::vector<Arm> arms;
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
	return arms;
}

// The side (0 right / 1 left of a -> b) of arm `a` that faces the next arm by angle (the right of its out-direction).
int side_to_next(const Arm &a) { return a.at_a ? 0 : 1; }

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

} // namespace

int seg_between(const CityState &s, int a, int b) {
	for (int seg : s.roads.nodes[a].segs) {
		const RoadSegment &g = s.roads.segs[seg];
		if (g.alive && ((g.a == a && g.b == b) || (g.b == a && g.a == b)) && walkable(s, seg)) return seg;
	}
	return -1;
}

bool walkable(const CityState &s, int seg) {
	if (seg < 0 || size_t(seg) >= s.roads.segs.size() || !s.roads.segs[size_t(seg)].alive) return false;
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const Profile &p = s.roads.profile_of(seg);
	return g.alive && !p.rail && p.walk() && g.len - g.retreat_b > g.retreat_a;
}

void RoadRouteSearch::begin_walk(const CityState &s, int from, int goal, int max_expanded) {
	counts_ = {}; walking_ = true; micro_ = false; search_.begin(0, -1, -1, 0); result_ = SearchStatus::INVALIDATED;
	city_ = s.generation; graph_ = s.traffic.g.routing_revision; roads_ = uint64_t(s.roads.version);
	const RoadNetwork &net = s.roads;
	if (from < 0 || goal < 0 || size_t(from) >= net.nodes.size() || size_t(goal) >= net.nodes.size()) return;
	goal_ = goal;
	search_.begin(net.nodes.size(), from, goal, max_expanded < 0 ? int(s.catalog.traffic.ped_route_max_expanded) : max_expanded);
	result_ = SearchStatus::PENDING;
}
SearchStatus RoadRouteSearch::step_walk(const CityState &s, WorkBudget &work) {
	if (!source_current(s)) { result_ = SearchStatus::INVALIDATED; return result_; }
	if (result_ != SearchStatus::PENDING) return result_;
	const TrafficParams &tp = s.catalog.traffic; const RoadNetwork &net = s.roads;
	const float pace = tp.walk_speed_min; const V2 goal_p = node_xz(s, goal_);
	auto h = [&](int k) { return (node_xz(s, k) - goal_p).len() / pace; };
	auto next = [&](int k, RouteConnCursor &cursor, int &to, float &cost) {
		const auto &edges = net.nodes[k].segs;
		if (cursor.at >= edges.size()) return EdgeStep::END;
		const int seg = edges[cursor.at++];
		if (!walkable(s, seg)) return EdgeStep::SKIP;
		const RoadSegment &g = net.segs[seg];
		to = g.a == k ? g.b : g.a;
		// the fixed extra is a junction's: cutting a road into pieces does not add a cost to its through joints
		cost = float(g.len) / pace + (edges.size() > JUNCTION_ARMS ? tp.walk_cost_extra_s : 0.f);
		return EdgeStep::EDGE;
	};
	result_ = search_.advance([&work] { return work.take(); }, h, next); return result_;
}
int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, int want_side, Waypoint *ways, int &side_out) {
	std::vector<Arm> arms = arms_by_angle(s, n);
	const int deg = int(arms.size());
	int ai = -1, aj = -1;
	for (int k = 0; k < deg; k++) {
		if (arms[k].seg == seg_in) ai = k;
		if (arms[k].seg == seg_out) aj = k;
	}
	if (ai < 0 || aj < 0) return -1;
	// corner k lies between arm k and arm k + 1; a walker on arm i's side facing the next arm is on corner i
	const int ci = side_in == side_to_next(arms[ai]) ? ai : (ai - 1 + deg) % deg;
	const Profile &po = s.roads.profile_of(seg_out);
	int best = -1, best_steps = deg + 1, best_dir = 1;
	for (int pick = 0; pick < 2; pick++) {
		int cj = pick == 0 ? aj : (aj - 1 + deg) % deg;
		int side = pick == 0 ? side_to_next(arms[aj]) : 1 - side_to_next(arms[aj]);
		if (!has_side(po, side) || (want_side >= 0 && has_side(po, want_side) && side != want_side)) continue;
		int ccw = (cj - ci + deg) % deg, cw = (ci - cj + deg) % deg;
		int steps = std::min(ccw, cw);
		if (steps < best_steps || (steps == best_steps && (n & 1) == 0)) {
			best = pick;
			best_steps = steps;
			best_dir = ccw <= cw ? 1 : -1;
			side_out = side;
		}
	}
	if (best < 0) return -1;
	if (s.roads.mid_block(n) && best_steps > 0) best_dir = arms[size_t((ci + 1) % deg)].seg == s.roads.crossing_arm(n) ? 1 : -1; // over the arm that has the crosswalk
	int w = 0, cur = ci;
	for (int k = 0; k < best_steps && w + 3 <= MAX_WAYS; k++) {
		// increasing: cross arm cur + 1 from its far side to the side facing the next arm; decreasing: arm cur back
		int arm = best_dir > 0 ? (cur + 1) % deg : cur;
		int to_next = side_to_next(arms[arm]);
		ways[w++] = kerb_point(s, arms[arm], best_dir > 0 ? 1 - to_next : to_next);
		Waypoint far = kerb_point(s, arms[arm], best_dir > 0 ? to_next : 1 - to_next);
		far.cross = arms[arm].seg;
		ways[w++] = far;
		cur = best_dir > 0 ? (cur + 1) % deg : (cur - 1 + deg) % deg;
	}
	return w;
}

} // namespace traffic
} // namespace city
