// Roundabouts (the lab's editor/roundabout.ts commitRoundabout, roundaboutRefusal): on vacant ground only, a single-lane ring of
// ring_nodes nodes round the centre, one-way anticlockwise with the ring profile, every other node an entry with a two-way
// approach of approach_m out from it (priority at the entries, the circulating stream not stopped), the nodes between plain; a
// road anywhere near refuses it rather than being buried or left crossing the ring unconnected, and so does water.
#include "core/units.h"
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"

#include "sim/roads/junction.h"
#include "sim/state/city_state.h"
#include "sim/terrain/terrain_gen.h"

#include <cmath>

namespace city {

namespace {
// The ring's points: anticlockwise seen from above (the map's z runs down, as arm_angle reads it).
XZ ring_point(double cx, double cz, double angle, double r) { return { cx + std::cos(angle) * r, cz - std::sin(angle) * r }; }
} // namespace

std::string roundabout_refusal(const CityState &s, double cx, double cz, double radius) {
	const int ring = s.catalog.ring_profile();
	const Tuning &tn = s.catalog.tune;
	if (ring < 0 || !std::isfinite(radius) || radius < tn.roundabout_radius_min_m || radius > tn.roundabout_radius_max_m) return "tool.failed";
	const double reach = radius + tn.roundabout_approach_m;
	if (!s.terrain.in_bounds(float(cx), float(cz), float(reach))) return "road.move.outside";
	// No road within the ring, its approaches and both their casings: the ring is laid on vacant ground (roundaboutRefusal).
	const double width = s.catalog.profile(ring).half(), near = reach + width;
	bool occupied = false;
	s.roads.for_segments_near(cx - near, cz - near, cx + near, cz + near, [&](int sg) {
		double t, d;
		closest_t(s.roads.segs[size_t(sg)].q, cx, cz, t, d);
		if (d < near + s.roads.profile_of(sg).half()) occupied = true;
	});
	if (occupied) return "road.ring.occupied";
	// The game's ground holds standing water, the lab's none: a ring at grade stands on dry ground.
	for (double r : { radius - width, radius, radius + width })
		for (int k = 0; k < tn.roundabout_water_samples; k++) {
			const XZ p = ring_point(cx, cz, TAU_D * k / tn.roundabout_water_samples, r);
			const float ground = s.terrain.base_height_at(float(p.x), float(p.z));
			if (ground < TerrainGen::OCEAN_Y + tn.roundabout_water_clear_m || s.terrain.water_at(float(p.x), float(p.z)) > ground) return "road.ring.water";
		}
	return "";
}

EditResult build_roundabout(CityState &s, double cx, double cz, double radius) {
	const Tuning &tn = s.catalog.tune;
	EditResult r;
	r.error = roundabout_refusal(s, cx, cz, radius);
	if (!r.error.empty()) return r;
	const int ring = s.catalog.ring_profile(), count = tn.roundabout_ring_nodes;
	const double step = TAU_D / count;
	std::vector<int> nodes, made;
	for (int i = 0; i < count; i++) {
		const XZ p = ring_point(cx, cz, i * step, radius);
		const int n = j_add_node(s, p.x, p.z, s.terrain.base_height_at(float(p.x), float(p.z)));
		if (i % 2 != 0) j_set_plain(s, n, true); // between the entries the ring only bends
		else j_set_control(s, n, CTL_PRIORITY); // a yield on the node would stop the circulating stream as well
		nodes.push_back(n);
	}
	for (int i = 0; i < count; i++) { // each arc's control on its bisector at radius / cos(half its span): tangent to the circle at both ends
		const int a = nodes[size_t(i)], b = nodes[size_t((i + 1) % count)];
		const XZ c = ring_point(cx, cz, (i + 0.5) * step, radius / std::cos(step / 2));
		const Quad q{ s.roads.nodes[size_t(a)].x, s.roads.nodes[size_t(a)].z, c.x, c.z, s.roads.nodes[size_t(b)].x, s.roads.nodes[size_t(b)].z };
		made.push_back(j_add_segment(s, a, b, q, ring, 0, 0, 0));
	}
	for (int i = 0; i < count; i += 2) { // the approaches: two-way streets out from every other node
		const XZ far = ring_point(cx, cz, i * step, radius + tn.roundabout_approach_m);
		const int entry = nodes[size_t(i)];
		const double ex = s.roads.nodes[size_t(entry)].x, ez = s.roads.nodes[size_t(entry)].z;
		const int out = j_add_node(s, far.x, far.z, s.terrain.base_height_at(float(far.x), float(far.z)));
		made.push_back(j_add_segment(s, out, entry, Quad::straight(far.x, far.z, ex, ez), 0, 0, 0, 0));
	}
	if (!settle_road_edit(s, r)) return r; // the caller rolls the journal back
	for (int sg : made) r.buildings_removed += clear_under(s, sg);
	settle_network(s);
	// A ring node whose arms meet too sharply is a geometry the network cannot solve (roundabout.ts: workNet.impossible).
	const float least = float(tn.inspect_min_leg_deg) * DEG_TO_RAD;
	for (int n : nodes)
		if (node_gap(s.roads, n) < least) {
			r.error = "road.ring.geometry";
			return r;
		}
	r.ok = true;
	r.segments = int(made.size());
	return r;
}

} // namespace city
