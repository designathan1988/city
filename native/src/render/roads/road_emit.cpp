// Small helpers every road mesher piece shares: frames along a segment, crossing nodes and the textured quad.
#include "render/roads/road_mesh.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float DEGENERATE = 1e-12f;
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
