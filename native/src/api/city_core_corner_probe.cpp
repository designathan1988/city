// The ways round the corners as the walkers would walk them, for the tests (corner_ways): every way round a corner with no
// crosswalk on it (sim/traffic/ped_routes.cpp aim_onto_sidewalk), from each place across the sidewalk it leaves, against the
// carriageway (sim/street/footway.h on_carriageway); a way that touches it is told with its node's arms and corners as drawn. And
// what opens onto the corners (lot_gates): the lots' bays read near the camera whose way reaches a lane through a gate whose foot
// lies in a corner or a crosswalk (sim/roads/parking_layout.h kerb_standing). And the step onto a sidewalk at the end of a way
// (onto_sidewalk_moves): a body put onto its sidewalk where it stands stays where it stood.
#include "api/city_probes.h"
#include "sim/roads/parking_layout.h"
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

namespace {
constexpr float LAID_EVERY_M = 5.f; // along a sidewalk, a body laid every this much
constexpr float BEYOND_ROOM_M = 0.2f; // and this far past its room's edge either side, as a line may end
} // namespace

Dictionary CityProbes::onto_sidewalk_moves() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	int laid = 0;
	float worst = 0;
	Array worst_case;
	for (int seg = 0; seg < int(s.roads.segs.size()); seg++) {
		if (!traffic::walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const Profile &p = s.roads.profile_of(seg);
		for (int side = 0; side < 2; side++) {
			if (!(side == 0 ? p.has_walk_right : p.has_walk_left)) continue;
			const float half = traffic::walk_half(s, seg, side);
			for (float arc = traffic::arc_lo(g); arc <= traffic::arc_hi(g); arc += LAID_EVERY_M)
				for (const float u : { -half - BEYOND_ROOM_M, -half, 0.f, half, half + BEYOND_ROOM_M }) {
					Pedestrian w; // a body standing there, put onto the sidewalk where it stands and posed again
					w.seg = seg;
					w.side = side;
					w.s = arc;
					w.u = u;
					traffic::stand_on_sidewalk(s, w);
					const V2 was(w.cur.x, w.cur.z);
					traffic::onto_sidewalk(s, w);
					traffic::stand_on_sidewalk(s, w);
					const float moved = (V2(w.cur.x, w.cur.z) - was).len();
					laid++;
					if (moved <= worst) continue;
					worst = moved;
					worst_case = Array::make(seg, side, arc, u, moved);
				}
		}
	}
	Dictionary d;
	d["laid"] = laid;
	d["worst_m"] = worst;
	d["worst_case"] = worst_case; // street, side, arc, place across, metres moved
	return d;
}

Dictionary CityProbes::lot_gates() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	int reached = 0, in_corner = 0;
	Array cases;
	for (size_t b = 0; b < s.traffic.park.bays.size(); b++) {
		const Bay &bay = s.traffic.park.bays[b];
		if (!bay.alive || bay.kerb || !bay.way_read || !bay.reached) continue;
		reached++;
		const DirSeg &d = s.traffic.g.ds[size_t(bay.lane.ds)];
		const int seg = bay.lane.ds / 2;
		const double v = double(d.at0) + double(d.sign) * double(bay.lane.foot); // the gate's foot on its street
		if (kerb_standing(s.roads, seg, v)) continue;
		in_corner++;
		if (cases.size() < CASES) cases.push_back(Array::make(int(b), bay.source, seg, v, Vector2(bay.lane.p.x, bay.lane.p.y)));
	}
	Dictionary d;
	d["reached"] = reached;
	d["in_corner"] = in_corner;
	d["cases"] = cases;
	return d;
}

} // namespace city
