// Reading a point against the segments near it, see footway.h.
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float BROADEST_M = 48.f; // no profile reaches farther from its centre line (a query's pad)
constexpr int ARMS_WITH_CROSSWALK = 3;
constexpr float BESIDE_M = 0.05f; // the closest-point search's own error: under this a point is beside the segment

double arc_of(const RoadSegment &g, double t) {
	if (g.table.size() < 2) return g.len * t;
	const double f = std::clamp(t, 0.0, 1.0) * double(g.table.size() - 1);
	const size_t i = std::min(size_t(f), g.table.size() - 2);
	return g.table[i] + (g.table[i + 1] - g.table[i]) * (f - double(i));
}

bool strip_at(const Profile &p, float u, StripKind kind, float &centre) {
	for (const StripEdge &e : p.edges)
		if (e.strip.kind == kind && u >= e.u0 && u <= e.u1) {
			centre = (e.u0 + e.u1) * 0.5f;
			return true;
		}
	return false;
}

// Whether (x, z) lies along a segment at an offset `band` accepts.
template <typename F>
bool on_band(const RoadNetwork &net, double x, double z, F &&band) {
	bool on = false;
	net.for_segments_near(x - BROADEST_M, z - BROADEST_M, x + BROADEST_M, z + BROADEST_M, [&](int sid) {
		const Profile &p = net.profile_of(sid);
		if (on || p.edges.empty()) return;
		double v;
		float u, past;
		segment_place(net, sid, x, z, v, u, past);
		on = past < BESIDE_M && band(p, u);
	});
	return on;
}
} // namespace

void segment_frame(const RoadNetwork &net, int seg, double v, double &fx, double &fz, double &lx, double &lz) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const double t = t_at_length(g.table, v);
	double dx, dz;
	g.q.deriv(t, dx, dz);
	double l = std::hypot(dx, dz);
	if (l == 0) l = 1;
	fx = dx / l;
	fz = dz / l;
	lx = dz / l;
	lz = -dx / l;
}

void footway_point(const RoadNetwork &net, int seg, double v, float u, double &x, double &z, double &y) {
	const RoadSegment &g = net.segs[size_t(seg)];
	double fx, fz, lx, lz;
	segment_frame(net, seg, v, fx, fz, lx, lz);
	g.q.at(t_at_length(g.table, v), x, z);
	x += lx * u;
	z += lz * u;
	y = net.seg_y(seg, v);
}

void segment_place(const RoadNetwork &net, int seg, double x, double z, double &v, float &u, float &past) {
	const RoadSegment &g = net.segs[size_t(seg)];
	double t, d, cx, cz, fx, fz, lx, lz;
	closest_t(g.q, x, z, t, d);
	v = arc_of(g, t);
	g.q.at(t, cx, cz);
	segment_frame(net, seg, v, fx, fz, lx, lz);
	u = float((x - cx) * lx + (z - cz) * lz);
	past = float(std::fabs((x - cx) * fx + (z - cz) * fz));
}

FootwayHit footway_at(const RoadNetwork &net, double x, double z, double reach) {
	FootwayHit best;
	best.away = std::numeric_limits<float>::max();
	const double r = reach + BROADEST_M;
	net.for_segments_near(x - r, z - r, x + r, z + r, [&](int sid) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (p.edges.empty() || g.a == g.b) return;
		double v;
		float u, past;
		segment_place(net, sid, x, z, v, u, past);
		if (past > reach || v < g.retreat_a - reach || v > g.len - g.retreat_b + reach) return;
		for (int side = 0; side < 2; side++) {
			const StripEdge &e = side == 0 ? p.edges.front() : p.edges.back();
			if (e.strip.kind != StripKind::SIDEWALK) continue;
			const float away = std::max({ e.u0 - u, u - e.u1, past });
			if (away > reach || away >= best.away) continue;
			double fx, fz, lx, lz;
			segment_frame(net, sid, v, fx, fz, lx, lz);
			const float toward = side == 0 ? 1.f : -1.f; // +u runs from the right sidewalk to the road
			best.ok = true;
			best.seg = sid;
			best.side = side;
			best.v = std::clamp(v, g.retreat_a, g.len - g.retreat_b);
			best.u = std::clamp(u, e.u0, e.u1);
			best.away = away;
			best.kx = float(lx) * toward;
			best.kz = float(lz) * toward;
		}
	});
	return best;
}

FootwayHit median_at(const RoadNetwork &net, double x, double z, double reach) {
	FootwayHit hit;
	net.for_segments_near(x - BROADEST_M, z - BROADEST_M, x + BROADEST_M, z + BROADEST_M, [&](int sid) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		double v;
		float u, centre, past;
		segment_place(net, sid, x, z, v, u, past);
		if (hit.ok || past > BESIDE_M || v < g.retreat_a || v > g.len - g.retreat_b || !strip_at(p, u, StripKind::MEDIAN, centre)) return;
		if (std::fabs(u - centre) > reach) return;
		hit.ok = true;
		hit.seg = sid;
		hit.v = v;
		hit.u = centre;
	});
	return hit;
}

bool on_road(const RoadNetwork &net, double x, double z) {
	return on_band(net, x, z, [](const Profile &p, float u) { return u >= p.edges.front().u0 && u <= p.edges.back().u1; });
}

bool on_carriageway(const RoadNetwork &net, double x, double z, float keep) {
	return on_band(net, x, z, [keep](const Profile &p, float u) {
		const StripEdge &r = p.edges.front(), &l = p.edges.back();
		const float lo = r.strip.kind == StripKind::SIDEWALK ? r.u1 : r.u0, hi = l.strip.kind == StripKind::SIDEWALK ? l.u0 : l.u1;
		return u > lo - keep && u < hi + keep;
	});
}

bool on_landing(const RoadNetwork &net, const FootwayHit &hit, float radius, float clear) {
	if (!hit.ok || hit.side < 0) return false;
	const RoadSegment &g = net.segs[size_t(hit.seg)];
	const CrossingDef &c = net.cat->furniture.crossing;
	const float pad = radius + clear;
	const bool at_a = int(net.nodes[size_t(g.a)].segs.size()) >= ARMS_WITH_CROSSWALK, at_b = int(net.nodes[size_t(g.b)].segs.size()) >= ARMS_WITH_CROSSWALK;
	const double from_a = hit.v - g.retreat_a, from_b = g.len - g.retreat_b - hit.v;
	return (at_a && from_a > c.zebra_from_m - pad && from_a < c.zebra_to_m + pad) || (at_b && from_b > c.zebra_from_m - pad && from_b < c.zebra_to_m + pad);
}

} // namespace city
