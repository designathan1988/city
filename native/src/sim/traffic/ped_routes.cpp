// Where walkers go (Roadcraft peds.ts spawnRoute/planCorner, routing.ts walking A*): a route of nodes over streets
// with a sidewalk (cost length / pace + a fixed extra, heuristic straight distance / pace), and the way round each
// node: from the corner it stands on, round the corners and over the crosswalks of the arms in between (the
// rotation with the fewest crossings) to the start of the next sidewalk. Crosswalks lie `cross_at_m` past the
// junction setback, from kerb to kerb.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/astar.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {


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
	float arc = a.at_a ? float(g.retreat_a) + tp.cross_at_m : float(g.len - g.retreat_b) - tp.cross_at_m;
	float u = side == 0 ? -(p.asph_right + tp.kerb_stand_m) : p.asph_left + tp.kerb_stand_m;
	Pose o;
	sidewalk_pose(s, a.seg, 0, arc, u - p.walk_right, o);
	return Waypoint{ o.x, o.y, o.z, -1 };
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
		for (int seg : net.nodes[k].segs) {
			if (!walkable(s, seg)) continue;
			const RoadSegment &g = net.segs[seg];
			relax(g.a == k ? g.b : g.a, float(g.len) / pace + tp.walk_cost_extra_s);
		}
	};
	return astar(sc, net.nodes.size(), from, goal, int(tp.ped_route_max_expanded), h, expand, out);
}

int plan_corner(const CityState &s, int n, int seg_in, int side_in, int seg_out, Waypoint *ways, int &side_out) {
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
		if (!has_side(po, side)) continue;
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
