// The kerbed edge round a junction's corner or a turning bulb: the kerb along its line, the footway as one polygon cut into triangles
// (core/polygon_ops.h ring_triangles: whatever its shape, no triangle over another), and the face down from the footway's back.
#include "render/roads/road_mesh.h"

#include "core/polygon_ops.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

constexpr float HALF = 0.5f;

V2 plan(const Vector3 &p) { return V2(p.x, p.z); }

// The point of the line through `pts` nearest `p`, in plan.
V2 nearest_on(const V2 &p, const std::vector<Vector3> &pts) {
	V2 best = plan(pts[0]);
	for (size_t k = 0; k + 1 < pts.size(); k++) {
		const V2 a = plan(pts[k]), b = plan(pts[k + 1]);
		float t = 0;
		dist_point_segment(p, a, b, &t);
		const V2 q = a + (b - a) * t;
		if ((q - p).len2() < (best - p).len2()) best = q;
	}
	return best;
}

float distance_to(const V2 &p, const std::vector<Vector3> &pts) { return (nearest_on(p, pts) - p).len(); }

// A vertical face between two points of a line, from `lo` up to `hi` over each, facing `out`; `run` how far along the line each lies.
void face(const RoadEmit &e, const Vector3 &a, const Vector3 &b, float lo, float hi, float run_a, float run_b, const Color &col, const Vector3 &out) {
	if (hi <= lo) return;
	const Vector3 p[4] = { a + Vector3(0, lo, 0), b + Vector3(0, lo, 0), b + Vector3(0, hi, 0), a + Vector3(0, hi, 0) };
	const Vector2 uv[4] = { Vector2(run_a, lo), Vector2(run_b, lo), Vector2(run_b, hi), Vector2(run_a, hi) };
	emit_quad(e.mb, p, uv, road_color(col, RoadMat::SKIRT), out);
}
} // namespace

void kerb_band(const RoadEmit &e, const std::vector<Vector3> &in, const std::vector<Vector3> &stone, float top, float walk, bool has_stone) {
	const StreetLook &L = e.L;
	float run = 0;
	for (size_t k = 1; k < in.size(); k++) {
		const Vector3 &a = in[k - 1], &b = in[k], &sa = stone[k - 1], &sb = stone[k];
		Vector3 towards_road = (a + b) - (sa + sb); // from the stone's back to the kerb line
		towards_road.y = 0;
		towards_road = towards_road.length() > 0 ? towards_road.normalized() : UP;
		const float step = (b - a).length();
		face(e, a, b, 0, top, run, run + step, L.curb, towards_road);
		if (has_stone) {
			const Vector3 up(0, top, 0);
			const Vector3 p[4] = { a + up, b + up, sb + up, sa + up };
			const float wa = (sa - a).length(), wb = (sb - b).length();
			const Vector2 uv[4] = { Vector2(0, run), Vector2(0, run + step), Vector2(wb, run + step), Vector2(wa, run) };
			emit_quad(e.mb, p, uv, road_color(L.curb, RoadMat::KERB), UP);
			face(e, sa, sb, walk, top, run, run + step, L.curb, -towards_road);
		}
		run += step;
	}
}

void footway_patch(const RoadEmit &e, const std::vector<Vector3> &ring, const std::vector<Vector3> &inner, const std::vector<Vector3> &back) {
	if (ring.size() < 3) return;
	std::vector<double> x, z;
	for (const Vector3 &p : ring) {
		x.push_back(p.x);
		z.push_back(p.z);
	}
	std::vector<int> ids;
	for (const Vector3 &p : ring) {
		const V2 at = plan(p);
		const Color worn = wear_color(distance_to(at, inner), distance_to(at, back), 0, RoadMat::FOOTWAY, 0);
		ids.push_back(e.mb.add(p, UP, worn, Vector2(p.x, p.z + p.y), Vector2()));
	}
	const std::vector<int> tris = ring_triangles(x, z);
	for (size_t k = 0; k + 2 < tris.size(); k += 3) e.mb.tri(ids[size_t(tris[k])], ids[size_t(tris[k + 1])], ids[size_t(tris[k + 2])], UP);
}

void footway_skirt(const RoadEmit &e, const std::vector<Vector3> &back, const std::vector<Vector3> &inner, const Color &walk) {
	float run = 0;
	for (size_t k = 1; k < back.size(); k++) {
		const Vector3 &a = back[k - 1], &b = back[k];
		const float step = Vector2(b.x - a.x, b.z - a.z).length();
		if (step <= 0) continue;
		Vector3 out(b.z - a.z, 0, a.x - b.x); // square to the side, then turned away from the footway's inner side
		const V2 mid = plan((a + b) * HALF);
		if ((mid - nearest_on(mid, inner)).dot(V2(out.x, out.z)) < 0) out = -out;
		const float top = std::min(a.y, b.y), drop = top - e.L.skirt;
		const Vector3 p[4] = { Vector3(a.x, a.y - e.L.skirt, a.z), Vector3(b.x, b.y - e.L.skirt, b.z), b, a };
		const Vector2 uv[4] = { Vector2(run, drop), Vector2(run + step, drop), Vector2(run + step, top), Vector2(run, top) };
		emit_quad(e.mb, p, uv, road_color(walk, RoadMat::SKIRT), out.normalized());
		run += step;
	}
}

} // namespace city
