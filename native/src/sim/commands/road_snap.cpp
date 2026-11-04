// Where a road being drawn lands (Roadcraft editor/snap.ts): what is under the pointer, and the end the pointer asks for
// once the angles and the grid have had their say. Radii are screen pixels, so a click means the same at any zoom. The
// result is only a position: the plan (road_plan.cpp) validates it like any other point.
#include "sim/commands/commands.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double PI = 3.14159265358979323846;
constexpr double TAU = 2 * PI;
constexpr double DEG_TO_RAD = PI / 180;
constexpr int GRID_HEADINGS = 8; // the grid's two lines both ways, then the diagonals between them
constexpr double ON_HEADING = 1e-9;
constexpr double LOOK_MIN_M = 0.4, LOOK_MAX_M = 4.8, LOOK_SHARE = 0.2; // an arm's direction is read this far out of its node
constexpr double RAY_PAD_M = 1; // the snapped ray reaches past twice the pointer's distance by this
constexpr double HALF = 0.5;

struct Heading {
	double angle = 0;
	SnapGuide guide = SnapGuide::NONE;
};

double wrap(double a) {
	a = std::fmod(a + PI, TAU);
	return (a < 0 ? a + TAU : a) - PI;
}

// The direction a segment leaves `node` in.
double arm_heading(const RoadNetwork &net, int seg, int node) {
	const RoadSegment &g = net.segs[seg];
	double look = std::min(LOOK_MAX_M, std::max(LOOK_MIN_M, g.len * LOOK_SHARE)), x, z;
	bool from_a = g.a == node;
	g.q.at(t_at_length(g.table, from_a ? look : g.len - look), x, z);
	return std::atan2(z - net.nodes[node].z, x - net.nodes[node].x);
}

// The heading a drag may take and why: inside the cone of a candidate, that candidate; else the pointer's own. Landing
// on a road the angles that matter are the ones to that road; on open ground, the grid's and every step from them.
Heading snap_heading(const CityState &s, const Anchor &start, double heading, bool landing, double land) {
	double step = s.catalog.tune.road_snap_step_deg * DEG_TO_RAD, diff = step / 2;
	Heading best{ heading, SnapGuide::NONE };
	auto offer = [&](double a, SnapGuide g) {
		double d = std::fabs(wrap(a - heading));
		if (d < diff - ON_HEADING) { // a tie stays with the first: rounding must not turn a right angle into a plain step
			diff = d;
			best = { a, g };
		}
	};
	if (start.node >= 0)
		for (int seg : s.roads.nodes[start.node].segs) {
			double out = arm_heading(s.roads, seg, start.node);
			offer(out + PI, SnapGuide::CONTINUE);
			offer(out + PI / 2, SnapGuide::PERPENDICULAR);
			offer(out - PI / 2, SnapGuide::PERPENDICULAR);
		}
	int turns = int(std::lround(TAU / step));
	if (landing) {
		offer(land + PI / 2, SnapGuide::PERPENDICULAR);
		offer(land - PI / 2, SnapGuide::PERPENDICULAR);
		for (int i = 0; i < turns; i++) offer(land + i * step, SnapGuide::ANGLE);
	} else {
		for (int i = 0; i < GRID_HEADINGS; i++) offer(i * TAU / GRID_HEADINGS, SnapGuide::ORTHOGONAL);
		for (int i = 0; i < turns; i++) offer(i * step, SnapGuide::ANGLE);
	}
	return best;
}

double tangent_at(const Quad &q, double x, double z) {
	double t, d, dx, dz;
	closest_t(q, x, z, t, d);
	q.deriv(t, dx, dz);
	return std::atan2(dz, dx);
}

// Where a ray from the start first crosses the curve.
bool ray_hits(const Quad &q, double fx, double fz, double angle, double reach, double &hx, double &hz) {
	double tx = fx + std::cos(angle) * reach, tz = fz + std::sin(angle) * reach, best = 2, ax, az;
	q.at(0, ax, az);
	for (int i = 1; i <= ROAD_ARC_STEPS; i++) {
		double bx, bz, u, v;
		q.at(double(i) / ROAD_ARC_STEPS, bx, bz);
		if (seg_seg(fx, fz, tx, tz, ax, az, bx, bz, u, v) && u < best) {
			best = u;
			hx = fx + (tx - fx) * u;
			hz = fz + (tz - fz) * u;
		}
		ax = bx;
		az = bz;
	}
	return best <= 1;
}

double on_grid(double v, double step) { return std::round(v / step) * step; }

// The end moved onto the grid: along an axis or diagonal from a start on the grid it stays on that heading.
void onto_grid(const Anchor &start, double angle, double length, double step, double &x, double &z) {
	bool start_on = std::fabs(start.x - on_grid(start.x, step)) < ON_HEADING && std::fabs(start.z - on_grid(start.z, step)) < ON_HEADING;
	for (int k = 0; start_on && k < GRID_HEADINGS; k++) {
		double h = k * TAU / GRID_HEADINGS;
		if (std::fabs(wrap(angle - h)) >= ON_HEADING) continue;
		double unit = k % 2 ? step * std::sqrt(2.0) : step, steps = std::max(1.0, std::round(length / unit));
		x = start.x + std::cos(h) * steps * unit;
		z = start.z + std::sin(h) * steps * unit;
		break;
	}
	x = on_grid(x, step);
	z = on_grid(z, step);
}

SnappedEnd end_at(const Anchor &start, double x, double z, SnapGuide guide) {
	return { x, z, guide, std::atan2(z - start.z, x - start.x), std::hypot(x - start.x, z - start.z) };
}
} // namespace

Anchor find_anchor(const CityState &s, double x, double z, double px) {
	const RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	Anchor a;
	a.x = x;
	a.z = z;
	int n = net.find_node_near(x, z, tn.road_snap_node_px * px);
	if (n >= 0) {
		a.node = n;
		a.x = net.nodes[n].x;
		a.z = net.nodes[n].z;
		return a;
	}
	// A road is picked anywhere on its surface, however small the pointer's own radius is at this zoom.
	double pick = tn.road_snap_pick_px * px, widest = 0, best = std::numeric_limits<double>::max(), best_t = 0;
	for (const Profile &p : s.catalog.profiles) widest = std::max(widest, double(p.half()));
	double reach = std::max(pick, widest + tn.road_verge_m);
	int seg = -1;
	net.for_segments_near(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		double t, d;
		closest_t(net.segs[id].q, x, z, t, d);
		if (d > std::max(pick, double(net.profile_of(id).half()) + tn.road_verge_m)) return;
		if (d < best || (d == best && id < seg)) {
			best = d;
			best_t = t;
			seg = id;
		}
	});
	if (seg < 0) return a;
	const RoadSegment &g = net.segs[seg];
	double end = tn.road_snap_end_m;
	if (best_t * g.len < end || (1 - best_t) * g.len < end) {
		a.node = best_t < HALF ? g.a : g.b;
		a.x = net.nodes[a.node].x;
		a.z = net.nodes[a.node].z;
		return a;
	}
	a.seg = seg;
	a.t = best_t;
	g.q.at(best_t, a.x, a.z);
	return a;
}

Anchor snap_road_start(const CityState &s, const Anchor &a, const SnapOptions &o) {
	if (a.node >= 0 || a.seg >= 0 || !o.on || !o.grid) return a;
	Anchor free;
	free.x = on_grid(a.x, s.catalog.tune.road_grid_m);
	free.z = on_grid(a.z, s.catalog.tune.road_grid_m);
	return free;
}

SnappedEnd snap_road_end(const CityState &s, const Anchor &start, double x, double z, const SnapOptions &o) {
	Anchor hit = find_anchor(s, x, z, o.px);
	bool free = hit.node < 0 && hit.seg < 0;
	if (!o.on || (!o.angles && !o.grid)) return end_at(start, hit.x, hit.z, free ? SnapGuide::NONE : SnapGuide::NETWORK);
	// Landing on a node takes the node, exactly: no angle can be negotiated.
	if (hit.node >= 0) return end_at(start, hit.x, hit.z, SnapGuide::NETWORK);
	double heading = std::atan2(z - start.z, x - start.x), length = std::hypot(x - start.x, z - start.z);
	if (hit.seg >= 0) {
		if (!o.angles) return end_at(start, hit.x, hit.z, SnapGuide::NETWORK);
		const Quad &q = s.roads.segs[hit.seg].q;
		double reach = length * 2 + RAY_PAD_M, rx = 0, rz = 0, sx, sz;
		Heading h = snap_heading(s, start, heading, true, tangent_at(q, hit.x, hit.z));
		bool got = ray_hits(q, start.x, start.z, h.angle, reach, rx, rz);
		// The road's direction where the ray really meets it may offer a better angle than under the pointer.
		if (got) {
			Heading again = snap_heading(s, start, heading, true, tangent_at(q, rx, rz));
			if (ray_hits(q, start.x, start.z, again.angle, reach, sx, sz)) {
				h = again;
				rx = sx;
				rz = sz;
			}
		}
		// The snapped landing has to be near the pointer, or it is a different road.
		if (got && std::hypot(rx - hit.x, rz - hit.z) <= s.catalog.tune.road_snap_land_px * o.px)
			return { rx, rz, h.guide == SnapGuide::NONE ? SnapGuide::NETWORK : h.guide, wrap(h.angle), std::hypot(rx - start.x, rz - start.z) };
		return end_at(start, hit.x, hit.z, SnapGuide::NETWORK);
	}
	Heading h{ heading, SnapGuide::NONE };
	if (o.angles) h = snap_heading(s, start, heading, false, 0);
	double ex = start.x + std::cos(h.angle) * length, ez = start.z + std::sin(h.angle) * length;
	if (o.grid) onto_grid(start, h.angle, length, s.catalog.tune.road_grid_m, ex, ez);
	return end_at(start, ex, ez, h.guide);
}

} // namespace city
