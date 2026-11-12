// Small helpers every road mesher piece shares: frames along a segment, crossing nodes and the textured quad.
#include "render/roads/road_mesh.h"

#include "render/owned_segments.h"
#include "sim/roads/centre_line.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float DEGENERATE = 1e-12f;
constexpr double ONE_GRADE_M = 0.02; // off the straight line between its mouths a road is on one grade under this
} // namespace

SegFrame seg_frame(const CityView &s, int sid, double at) {
	double x, z, fx, fz;
	centre_at(s.roads, sid, at, x, z, fx, fz);
	SegFrame f;
	f.c = Vector3(float(x), float(s.roads.seg_y(sid, at)), float(z));
	f.fwd = Vector3(float(fx), 0, float(fz));
	f.left = Vector3(float(fz), 0, float(-fx));
	return f;
}

int surface_steps(const RoadEmit &e, int sid, double v0, double v1) {
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	const int fine = std::max(1, int(std::ceil((v1 - v0) / (e.far ? e.L.far_step : e.L.step))));
	if (!g.q.is_straight(STRAIGHT_TOL)) return fine;
	// A straight road on one grade is linear in position, height and texture coordinates: its two mouths are enough.
	const double ya = e.s.roads.seg_y(sid, v0), yb = e.s.roads.seg_y(sid, v1);
	for (int k = 1; k < fine; k++) {
		const double share = double(k) / fine;
		if (std::fabs(e.s.roads.seg_y(sid, v0 + (v1 - v0) * share) - (ya + (yb - ya) * share)) > ONE_GRADE_M) return fine;
	}
	return 1;
}

bool crossing_node(const CityView &s, int node) { return s.roads.has_crosswalks(node); }
bool crosswalk_at(const CityView &s, int node, int seg) { return s.roads.crosswalk_on(node, seg); }

void emit_chunk(const RoadEmit &e, int64_t chunk) {
	const float x0 = key_x(chunk) * CHUNK, z0 = key_y(chunk) * CHUNK;
	each_owned_segment(e.s, chunk, [&](int sid) {
		emit_segment(e, sid);
		emit_markings(e, sid);
	});
	e.s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = e.s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == chunk) emit_node(e, n);
	});
}

void emit_quad(MeshBuilder &mb, const Vector3 p[4], const Vector2 uv[4], const Color &col, const Vector3 &out) {
	const Color cols[4] = { col, col, col, col };
	emit_quad(mb, p, uv, cols, out);
}

void emit_quad(MeshBuilder &mb, const Vector3 p[4], const Vector2 uv[4], const Color cols[4], const Vector3 &out, const Vector2 wear[4]) {
	Vector3 nn = (p[1] - p[0]).cross(p[3] - p[0]);
	if (nn.length_squared() < DEGENERATE) nn = (p[2] - p[1]).cross(p[0] - p[1]);
	if (nn.length_squared() < DEGENERATE) return;
	nn = nn.normalized();
	if (nn.dot(out) < 0) nn = -nn;
	int i[4];
	for (int k = 0; k < 4; k++) i[k] = mb.add(p[k], nn, cols[k], uv[k], wear ? wear[k] : Vector2());
	mb.tri(i[0], i[1], i[2], nn);
	mb.tri(i[0], i[2], i[3], nn);
}

} // namespace city
