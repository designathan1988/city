// Where things are on the road (Roadcraft sim/lanes.ts): lane points between a segment's two mouths, points on a
// connector curve and on a sidewalk. "Left" of a direction d is (d.z, -d.x); a lane sits at curve point + left * u.
#include "sim/traffic/traffic_system.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr double DEGENERATE = 1e-9; // tangent length below which the curve direction falls back to +X
// Curve point, unit tangent (a -> b) and height of segment `seg` at arc length `arc` from node a.
void curve_at(const CityState &s, int seg, double arc, double &x, double &z, double &dx, double &dz, float &y) {
	const RoadSegment &g = s.roads.segs[seg];
	arc = std::max(0.0, std::min(g.len, arc));
	double t = t_at_length(g.table, arc);
	g.q.at(t, x, z);
	g.q.deriv(t, dx, dz);
	double l = std::hypot(dx, dz);
	if (l < DEGENERATE) { dx = 1; dz = 0; }
	else { dx /= l; dz /= l; }
	y = float(s.roads.seg_y(seg, arc));
}
} // namespace

void lane_pose(const CityState &s, int ds, int lane, float at, Pose &o) {
	const DirSeg &d = s.traffic.g.ds[ds];
	double arc = d.at0 + d.sign * std::max(0.f, std::min(d.len, at)), x, z, dx, dz;
	float y;
	curve_at(s, d.seg, arc, x, z, dx, dz, y);
	float u = d.u[std::max(0, std::min(d.lanes - 1, lane))];
	o.x = float(x + dz * u);
	o.z = float(z - dx * u);
	o.y = y;
	o.fx = float(dx * d.sign);
	o.fz = float(dz * d.sign);
}

void conn_pose(const Conn &c, float at, Pose &o) {
	float t = 1.f;
	for (int i = 0; i < CONN_STEPS; i++) {
		if (at > c.table[i + 1]) continue;
		float span = c.table[i + 1] - c.table[i];
		t = (float(i) + (span > 0 ? (at - c.table[i]) / span : 0.f)) / CONN_STEPS;
		break;
	}
	t = clampf(t, 0.f, 1.f);
	float r = 1 - t;
	o.x = r * r * c.x0 + 2 * r * t * c.cx + t * t * c.x1;
	o.z = r * r * c.z0 + 2 * r * t * c.cz + t * t * c.z1;
	o.y = lerpf(c.y0, c.y1, t);
	V2 d = V2(2 * r * (c.cx - c.x0) + 2 * t * (c.x1 - c.cx), 2 * r * (c.cz - c.z0) + 2 * t * (c.z1 - c.cz)).norm();
	o.fx = d.x;
	o.fz = d.y;
}

void sidewalk_pose(const CityState &s, int seg, int side, float arc, float track_u, Pose &o) {
	const Profile &p = s.roads.profile_of(seg);
	double x, z, dx, dz;
	float y;
	curve_at(s, seg, arc, x, z, dx, dz, y);
	float u = (side == 0 ? p.walk_right : p.walk_left) + track_u;
	o.x = float(x + dz * u);
	o.z = float(z - dx * u);
	o.y = y + s.catalog.furniture.sidewalk.height_m;
	o.fx = float(dx);
	o.fz = float(dz);
}

} // namespace traffic
} // namespace city
