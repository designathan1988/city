// Carriageway and footway point queries (Roadcraft 4.0 world/carriageway.ts, world/footwayPath.ts:35-61). A segment's
// lateral offset u is measured along its left normal (d.z, -d.x): the profile's right edges are at negative u.
#include "sim/roads/road_query.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace roads {

namespace {
constexpr double DEGENERATE = 1e-9;

// Arc length at parameter t (the segment's table holds it at t = i / (size - 1)).
double arc_at(const RoadSegment &g, double t) {
	const size_t n = g.table.size();
	if (n < 2) return g.len * t;
	const double f = std::max(0.0, std::min(1.0, t)) * double(n - 1);
	const size_t i = std::min(n - 2, size_t(f));
	return g.table[i] + (g.table[i + 1] - g.table[i]) * (f - double(i));
}

// The curve point, unit tangent and the point's lateral offset at the closest parameter to (x, z).
struct Foot {
	double t = 0, cx = 0, cz = 0, dx = 1, dz = 0, u = 0;
};
Foot foot_of(const RoadSegment &g, double x, double z) {
	Foot f;
	double d;
	closest_t(g.q, x, z, f.t, d);
	g.q.at(f.t, f.cx, f.cz);
	g.q.deriv(f.t, f.dx, f.dz);
	const double l = std::hypot(f.dx, f.dz);
	if (l < DEGENERATE) { f.dx = 1; f.dz = 0; }
	else { f.dx /= l; f.dz /= l; }
	f.u = (x - f.cx) * f.dz - (z - f.cz) * f.dx;
	return f;
}
} // namespace

bool on_carriageway(const RoadNetwork &net, double x, double z, double *y) {
	bool hit = false;
	net.for_segments_near(x, z, x, z, [&](int s) {
		if (hit) return;
		const RoadSegment &g = net.segs[size_t(s)];
		const Profile &p = net.profile_of(s);
		const Foot f = foot_of(g, x, z);
		// beyond an end the closest point is the end itself: that is a junction's plate, below
		if (f.t <= 0 || f.t >= 1 || f.u < -p.asph_right || f.u > p.asph_left) return;
		hit = true;
		if (y) *y = net.seg_y(s, arc_at(g, f.t));
	});
	if (hit) return true;
	// A junction's plate: within the widest asphalt half width of its arms from the node.
	double plate = 0;
	for (const Profile &p : net.cat->profiles) plate = std::max(plate, double(std::max(p.asph_right, p.asph_left)));
	net.node_hash.query(float(x - plate), float(z - plate), float(x + plate), float(z + plate), [&](int n) {
		if (hit || n < 0 || size_t(n) >= net.nodes.size() || !net.nodes[size_t(n)].alive) return;
		const RoadNode &nd = net.nodes[size_t(n)];
		double half = 0;
		for (int s : nd.segs) half = std::max(half, double(std::max(net.profile_of(s).asph_right, net.profile_of(s).asph_left)));
		if (std::hypot(x - nd.x, z - nd.z) > half) return;
		hit = true;
		if (y) *y = nd.y;
	});
	return hit;
}

FootwayHit nearest_footway(const RoadNetwork &net, double x, double z, double reach, double step,
		const std::function<bool(double, double)> &rejects) {
	FootwayHit best;
	net.for_segments_near(x - reach, z - reach, x + reach, z + reach, [&](int s) {
		const RoadSegment &g = net.segs[size_t(s)];
		const Profile &p = net.profile_of(s);
		for (int side = 0; side < 2; side++) {
			if (!(side == 0 ? p.has_walk_right : p.has_walk_left)) continue;
			const double u = side == 0 ? p.walk_right : p.walk_left;
			const double lo = g.retreat_a, hi = std::max(lo, g.len - g.retreat_b);
			auto point_at = [&](double arc, double &px, double &pz) {
				const double t = t_at_length(g.table, std::max(lo, std::min(hi, arc)));
				double cx, cz, dx, dz;
				g.q.at(t, cx, cz);
				g.q.deriv(t, dx, dz);
				const double l = std::max(DEGENERATE, std::hypot(dx, dz));
				px = cx + dz / l * u;
				pz = cz - dx / l * u;
			};
			double px, pz;
			double arc = std::max(lo, std::min(hi, arc_at(g, foot_of(g, x, z).t)));
			point_at(arc, px, pz);
			double d = std::hypot(px - x, pz - z);
			if (!(d < reach) || (best.ok && d >= best.d)) continue;
			if (rejects && rejects(px, pz)) {
				bool found = false;
				for (double a = lo; a <= hi; a += step) {
					double qx, qz;
					point_at(a, qx, qz);
					const double dq = std::hypot(qx - x, qz - z);
					if (dq < reach && (!found || dq < d) && !rejects(qx, qz)) {
						found = true;
						px = qx; pz = qz; d = dq; arc = a;
					}
				}
				if (!found || (best.ok && d >= best.d)) continue;
			}
			best = { true, float(px), float(pz), float(d), float(arc), s, side };
		}
	});
	return best;
}

} // namespace roads
} // namespace city
