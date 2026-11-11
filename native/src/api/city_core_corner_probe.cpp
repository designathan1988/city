// The ways round the corners as the walkers would walk them, for the tests (corner_ways): every way round a corner with no
// crosswalk on it (sim/traffic/ped_routes.cpp aim_onto_sidewalk), from each place across the sidewalk it leaves, against the
// carriageway (sim/street/footway.h on_carriageway); a way that touches it is told with its node's arms and corners as drawn.
#include "api/city_probes.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float PROBE_STEP_M = 0.25f; // across a sidewalk's room and along a way, a point every this much
constexpr float ROUNDING_M = 1e-3f;
constexpr int CASES = 8;

// Whether the run from `at` through the walker's ways touches a carriageway, and where first.
bool run_off_way(const CityState &s, const Pedestrian &w, V2 at, V2 &where) {
	for (int k = 0; k < w.wn; k++) {
		const V2 to(w.ways[k].x, w.ways[k].z), leg = to - at;
		const float len = leg.len();
		for (float d = 0; d <= len + ROUNDING_M; d += PROBE_STEP_M) {
			where = at + leg * (len > 0 ? std::min(1.f, d / len) : 0.f);
			if (on_carriageway(s.roads, where.x, where.y, 0.f)) return true;
		}
		at = to;
	}
	return false;
}

// A corner way that touches a carriageway, as the test prints it: the node, the street it comes from, the side, the street it
// goes to, its place across, where it touched; the run from where it stood through its points; the node's arms as the kerbs
// are drawn (direction, asphalt half widths, setback) and its corners (mode, trims, radius, centre from the node).
Array corner_case(const CityState &s, int n, int in, int side, int out, float u, const V2 &where, const Pedestrian &w) {
	PackedVector2Array run;
	run.push_back(Vector2(w.cur.x, w.cur.z));
	for (int k = 0; k < w.wn; k++) run.push_back(Vector2(w.ways[k].x, w.ways[k].z));
	const JunctionKerbs kerbs(s.roads, n);
	Array arms, corners;
	for (size_t k = 0; k < kerbs.arms.size(); k++) {
		const Arm &a = kerbs.arms[k];
		const Corner &c = kerbs.corners[k];
		arms.push_back(Array::make(a.dx, a.dz, a.asph_left, a.asph_right, kerbs.mouths[k]));
		corners.push_back(Array::make(int(c.mode), c.trim_a, c.trim_b, c.r, c.cx, c.cz));
	}
	return Array::make(n, in, side, out, u, Vector2(where.x, where.y), run, Vector2(float(kerbs.x), float(kerbs.z)), arms, corners);
}
} // namespace

Dictionary CityProbes::corner_ways() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	int ways = 0, off = 0;
	Array cases;
	for (int n = 0; n < int(s.roads.nodes.size()); n++) {
		const RoadNode &nd = s.roads.nodes[size_t(n)];
		if (!nd.alive || nd.segs.size() < 2) continue;
		for (int in : nd.segs)
			for (int side = 0; side < 2 && traffic::walkable(s, in); side++) {
				const Profile &pi = s.roads.profile_of(in);
				if (!(side == 0 ? pi.has_walk_right : pi.has_walk_left)) continue;
				for (int out : nd.segs) {
					Waypoint plan[MAX_WAYS];
					int side_out = 0;
					if (out == in || !traffic::walkable(s, out) || traffic::plan_corner(s, n, in, side, out, -1, plan, side_out) != 0) continue;
					const RoadSegment &gi = s.roads.segs[size_t(in)], &go = s.roads.segs[size_t(out)];
					const float half = traffic::walk_half(s, in, side);
					for (float u = -half; u <= half + ROUNDING_M; u += PROBE_STEP_M) {
						Pedestrian w; // at the end of its sidewalk, at this place across, as at_node finds it (pedestrians.cpp)
						w.seg = in;
						w.side = side;
						w.dir = gi.b == n ? 0 : 1;
						w.s = w.dir == 0 ? traffic::arc_hi(gi) : traffic::arc_lo(gi);
						w.u = u;
						traffic::stand_on_sidewalk(s, w);
						w.seg = out;
						w.side = side_out;
						w.dir = go.a == n ? 0 : 1;
						w.s = w.dir == 0 ? traffic::arc_lo(go) : traffic::arc_hi(go);
						w.way_node = n;
						w.wi = 0;
						w.wn = 1;
						traffic::aim_onto_sidewalk(s, w);
						ways++;
						V2 where;
						if (!run_off_way(s, w, V2(w.cur.x, w.cur.z), where)) continue;
						off++;
						if (cases.size() < CASES) cases.push_back(corner_case(s, n, in, side, out, u, where, w));
					}
				}
			}
	}
	Dictionary d;
	d["ways"] = ways;
	d["off"] = off;
	d["cases"] = cases;
	return d;
}

} // namespace city
