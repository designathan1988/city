// Small helpers every road mesher piece shares: frames along a segment, crossing nodes and the textured quad.
#include "render/roads/road_mesh.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float DEGENERATE = 1e-12f;
constexpr double ONE_GRADE_M = 0.02; // off the straight line between its mouths a road is on one grade under this
} // namespace

SegFrame seg_frame(const CityState &s, int sid, double at) {
	const RoadSegment &g = s.roads.segs[sid];
	double t = t_at_length(g.table, at), x, z, dx, dz;
	g.q.at(t, x, z);
	g.q.deriv(t, dx, dz);
	double l = std::hypot(dx, dz);
	if (l == 0) l = 1;
	SegFrame f;
	f.c = Vector3(float(x), float(s.roads.seg_y(sid, at)), float(z));
	f.fwd = Vector3(float(dx / l), 0, float(dz / l));
	f.left = Vector3(float(dz / l), 0, float(-dx / l));
	return f;
}

int surface_steps(const RoadEmit &e, int sid, double v0, double v1) {
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	const int fine = std::max(1, int(std::ceil((v1 - v0) / (e.far ? e.L.far_step : e.L.step))));
	if (!g.q.is_straight(STRAIGHT_TOL)) return fine;
	// A straight road on one grade is linear in position, height, texture and pool coordinates: its two mouths are enough.
	const double ya = e.s.roads.seg_y(sid, v0), yb = e.s.roads.seg_y(sid, v1);
	for (int k = 1; k < fine; k++) {
		const double share = double(k) / fine;
		if (std::fabs(e.s.roads.seg_y(sid, v0 + (v1 - v0) * share) - (ya + (yb - ya) * share)) > ONE_GRADE_M) return fine;
	}
	return 1;
}

double street_start(const RoadNetwork &net, int sid) {
	double run = 0;
	for (int at = sid, left = int(net.segs.size()); left > 0; left--) {
		const RoadNode &n = net.nodes[net.segs[at].a];
		if (!n.joint || n.segs.size() != 2) break;
		const int back = n.segs[0] == at ? n.segs[1] : n.segs[0];
		if (back == sid || net.segs[back].b != net.segs[at].a) break; // a ring, or a piece laid the other way
		run += net.segs[back].len;
		at = back;
	}
	return run;
}

bool crossing_node(const CityState &s, int node) { return s.roads.has_crosswalks(node); }

void emit_quad(MeshBuilder &mb, const Vector3 p[4], const Vector2 uv[4], const Vector2 uv2[4], const Color &col, const Vector3 &out) {
	const Color cols[4] = { col, col, col, col };
	emit_quad(mb, p, uv, uv2, cols, out);
}

void emit_quad(MeshBuilder &mb, const Vector3 p[4], const Vector2 uv[4], const Vector2 uv2[4], const Color cols[4], const Vector3 &out) {
	Vector3 nn = (p[1] - p[0]).cross(p[3] - p[0]);
	if (nn.length_squared() < DEGENERATE) nn = (p[2] - p[1]).cross(p[0] - p[1]);
	if (nn.length_squared() < DEGENERATE) return;
	nn = nn.normalized();
	if (nn.dot(out) < 0) nn = -nn;
	int i[4];
	for (int k = 0; k < 4; k++) i[k] = mb.add(p[k], nn, cols[k], uv[k], uv2[k]);
	mb.tri(i[0], i[1], i[2], nn);
	mb.tri(i[0], i[2], i[3], nn);
}

} // namespace city
