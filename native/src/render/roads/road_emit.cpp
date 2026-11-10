// What every road mesher piece shares: the stations a segment is cut at (by its surface error), frames along it, a chunk's
// segments and nodes, and the textured quad.
#include "render/roads/road_mesh.h"

#include "render/owned_segments.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float DEGENERATE = 1e-12f; // a triangle of less area has no normal
// A stretch shorter than this is not split again: the surface merges stations closer than it (road_segment.cpp ROAD_MIN_SPAN).
constexpr double LEAST_STRETCH_M = 0.05;
constexpr int ERROR_PROBES = 32; // the points a stretch is measured at by stations_error, its own dense reading
// A curve of length L bending at most k stays within k L^2 / SAGITTA_SHARE of its chord: a circular arc's sagitta,
// R - sqrt(R^2 - (L/2)^2) <= (L/2)^2 / (2 R) = k L^2 / 8, and a curve bending less stays closer.
constexpr double SAGITTA_SHARE = 8;

// A point of the road's surface: `u` across from the centre line at arc `at`, at the deck's height.
Vector3 surface_at(const CityView &s,int sid, double at, double u) {
	const SegFrame f = seg_frame(s, sid, at);
	return f.c + f.left * float(u);
}

// How far the true surface at arc m stands off the mesh's straight stretch between arcs a and b, over the centre line and both
// edges of the section: the distance from the true point to the stretch's segment (the mesh's geometry, whatever share of the
// way along it the point falls).
double off_chord(const CityView &s,int sid, double half, double a, double b, double m) {
	double worst = 0;
	for (const double u : { -half, 0.0, half }) {
		const Vector3 pa = surface_at(s, sid, a, u), pb = surface_at(s, sid, b, u), p = surface_at(s, sid, m, u);
		const Vector3 run = pb - pa;
		const float run2 = run.length_squared();
		const float share = run2 > 0 ? std::clamp((p - pa).dot(run) / run2, 0.f, 1.f) : 0.f;
		worst = std::max(worst, double((p - (pa + run * share)).length()));
	}
	return worst;
}

// The stations of the deck's height profile strictly between a and b (it is linear between them: RoadGrade::at).
template <typename Fn>
void each_grade_station(const CityView &s,int sid, double a, double b, Fn &&fn) {
	if (!s.roads.grade.has(sid)) return; // linear between its nodes: no station inside
	const double step = double(s.roads.grade.prof[size_t(sid)].step);
	for (double at = (std::floor(a / step) + 1) * step; at < b; at += step) fn(at);
}

// How a quadratic Bezier bends between parameters ta and tb: the most curvature, |B' x B''| / |B'|^3, and the most the curvature
// changes a metre of arc. B' = B'(0) + t B'' with B'' constant, so the cross product is constant and the curvature peaks where the
// derivative is shortest: the curve's vertex, or the nearer end. Its change a metre is d/ds (C / |B'|^3) = -3 C (B' . B'') / |B'|^6,
// and B' . B'' is linear in t: largest at an end of the stretch.
struct Bend {
	double k = 0, dk = 0;
};

Bend bend_of(const Quad &q, double ta, double tb) {
	constexpr double CUBE_RULE = 3; // d(x^-3) = -3 x^-4 dx
	const double ddx = 2 * (q.x0 - 2 * q.cx + q.x1), ddz = 2 * (q.z0 - 2 * q.cz + q.z1);
	double dx0, dz0;
	q.deriv(0, dx0, dz0);
	const double dd = ddx * ddx + ddz * ddz;
	const double vertex = dd > 0 ? -(dx0 * ddx + dz0 * ddz) / dd : ta;
	double dx, dz;
	q.deriv(std::clamp(vertex, ta, tb), dx, dz);
	const double speed = std::hypot(dx, dz);
	if (!(speed > 0)) return { std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
	const double cross = std::fabs(dx0 * ddz - dz0 * ddx);
	double ax, az, bx, bz;
	q.deriv(ta, ax, az);
	q.deriv(tb, bx, bz);
	const double along = std::max(std::fabs(ax * ddx + az * ddz), std::fabs(bx * ddx + bz * ddz));
	const double speed3 = speed * speed * speed;
	return { cross / speed3, CUBE_RULE * cross * along / (speed3 * speed3) };
}

// The most the plan of the stretch [a, b] (arcs) can stand off its chord, over the centre line and both edges `half` across. A
// curve p(s) over a run L stays within L^2 / SAGITTA_SHARE x the most of |p''| of its chord (the error of a straight line through
// two points of a twice differentiable curve). An edge u across the centre line c(s) is p = c + u n, with n' = -k T: p'' =
// -u k' T + (1 - u k) k N, so |p''| <= h |k'| + (1 + k h) k on either side, folded or not (k h >= 1 on the inner one). The run is
// at most the control polygon's length (a Bezier lies in its control hull).
double plan_bound(const RoadSegment &g, double half, double a, double b) {
	const double ta = t_at_length(g.table, a), tb = t_at_length(g.table, b);
	const Quad piece = sub_quad(g.q, ta, tb);
	const double hull = std::hypot(piece.cx - piece.x0, piece.cz - piece.z0) + std::hypot(piece.x1 - piece.cx, piece.z1 - piece.cz);
	const Bend bend = bend_of(g.q, ta, tb);
	return hull * hull / SAGITTA_SHARE * (bend.k * (1 + bend.k * half) + bend.dk * half);
}

// How far the height of the stretch [a, b] stands off its chord at worst: the height is linear between the grade's stations, so
// the worst is at one of them, read exactly.
double height_off(const CityView &s,int sid, double a, double b) {
	const double ya = s.roads.seg_y(sid, a), yb = s.roads.seg_y(sid, b);
	double worst = 0;
	each_grade_station(s, sid, a, b, [&](double at) { worst = std::max(worst, std::fabs(s.roads.seg_y(sid, at) - (ya + (yb - ya) * (at - a) / (b - a)))); });
	return worst;
}

// Cuts [a, b] until the surface between its stations is within `error` of the true one, appending each right end: the plan's
// bound and the height's exact deviation together (a point is off its chord by at most the sum of the two). The plan's bound
// falls with the square of the run, so a stretch that is off by `off` is cut in ceil(sqrt(off / error)) equal parts at once: the
// fewest that can hold it, where halving again and again would land anywhere between that count and twice it.
void split(const CityView &s,int sid, double half, double error, double a, double b, std::vector<double> &out) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	const double off = b - a <= LEAST_STRETCH_M ? 0 : plan_bound(g, half, a, b) + height_off(s, sid, a, b);
	if (off <= error) {
		out.push_back(b);
		return;
	}
	const double most = std::max(2.0, std::floor((b - a) / LEAST_STRETCH_M)); // no part shorter than the least stretch
	const double want = std::isfinite(off) ? std::ceil(std::sqrt(off / error)) : 2;
	const int parts = int(std::clamp(want, 2.0, most));
	for (int i = 0; i < parts; i++) split(s, sid, half, error, a + (b - a) * i / parts, i + 1 == parts ? b : a + (b - a) * (i + 1) / parts, out);
}
} // namespace

std::vector<double> segment_stations(const CityView &s,const StreetLook &L, bool far, int sid, double v0, double v1) {
	std::vector<double> out{ v0 };
	if (v1 <= v0) return out;
	const double half = double(s.catalog.profile(s.roads.segs[size_t(sid)].profile).half());
	split(s, sid, half, double(far ? L.far_surface_error : L.surface_error), v0, v1, out);
	return out;
}

std::vector<double> segment_stations(const RoadEmit &e, int sid, double v0, double v1) {
	return segment_stations(e.s, e.L, e.far, sid, v0, v1);
}

double stations_error(const CityView &s,int sid, const std::vector<double> &at) {
	const double half = double(s.catalog.profile(s.roads.segs[size_t(sid)].profile).half());
	double worst = 0;
	for (size_t k = 0; k + 1 < at.size(); k++)
		for (int i = 1; i < ERROR_PROBES; i++)
			worst = std::max(worst, off_chord(s, sid, half, at[k], at[k + 1], at[k] + (at[k + 1] - at[k]) * i / ERROR_PROBES));
	return worst;
}

SegFrame seg_frame(const CityView &s,int sid, double at) {
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
