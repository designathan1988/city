// A pitched roof over a plan that is not its box (the 4.0's emitPolygonRoof): the planes of the box's roof, each a convex patch, cut by the plan's triangles, and the walls
// filled up to the roof. A wall's top is exact: the wall is cut by the patches too, each piece straight under its plane.
#include "render/buildings/facade.h"
#include "render/buildings/roof_clip.h"

#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/roof_shape.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f, PROBE_M = 0.01f, SHORTEST_M = 1e-3f, LOWEST_M = 1e-4f; // a probe's step, the shortest edge and the lowest rise kept

// A convex piece of the plan and the height over the eaves there: gx * x + gy * y + h0.
struct Patch {
	Ring ring;
	float gx = 0, gy = 0, h0 = 0;
	float at(V2 p) const { return gx * p.x + gy * p.y + h0; }
};

Ring rect(float x0, float y0, float x1, float y1) { return { { x0, y0 }, { x1, y0 }, { x1, y1 }, { x0, y1 } }; }

void add(std::vector<Patch> &out, Ring ring, float gx, float gy, float h0) {
	if (ring_area(ring) < 0) std::reverse(ring.begin(), ring.end());
	out.push_back({ std::move(ring), gx, gy, h0 });
}

void hip_patches(const Volume &v, float slope, std::vector<Patch> &out) {
	const bool along_x = v.w >= v.d;
	const float x0 = v.x, y0 = v.y, x1 = v.x + v.w, y1 = v.y + v.d, h = std::min(v.w, v.d) * HALF;
	if (along_x) {
		add(out, { { x0, y0 }, { x0 + h, y0 + h }, { x0, y1 } }, slope, 0, -x0 * slope);
		add(out, { { x1, y0 }, { x1, y1 }, { x1 - h, y0 + h } }, -slope, 0, x1 * slope);
		add(out, { { x0, y0 }, { x1, y0 }, { x1 - h, y0 + h }, { x0 + h, y0 + h } }, 0, slope, -y0 * slope);
		add(out, { { x0 + h, y0 + h }, { x1 - h, y0 + h }, { x1, y1 }, { x0, y1 } }, 0, -slope, y1 * slope);
	} else {
		add(out, { { x0, y0 }, { x0 + h, y0 + h }, { x1, y0 } }, 0, slope, -y0 * slope);
		add(out, { { x0, y1 }, { x1, y1 }, { x0 + h, y1 - h } }, 0, -slope, y1 * slope);
		add(out, { { x0, y0 }, { x0, y1 }, { x0 + h, y1 - h }, { x0 + h, y0 + h } }, slope, 0, -x0 * slope);
		add(out, { { x0 + h, y0 + h }, { x0 + h, y1 - h }, { x1, y1 }, { x1, y0 } }, -slope, 0, x1 * slope);
	}
}

std::vector<Patch> patches_of(const RoofShape &K, const Volume &v) {
	std::vector<Patch> out;
	const float slope = roof_slope(K, v), x0 = v.x, y0 = v.y, x1 = v.x + v.w, y1 = v.y + v.d;
	switch (v.roof) {
		case ROOF_GABLE:
			if (ridge_along_x(v)) {
				const float mid = (y0 + y1) * HALF;
				add(out, rect(x0, y0, x1, mid), 0, slope, -y0 * slope);
				add(out, rect(x0, mid, x1, y1), 0, -slope, y1 * slope);
			} else {
				const float mid = (x0 + x1) * HALF;
				add(out, rect(x0, y0, mid, y1), slope, 0, -x0 * slope);
				add(out, rect(mid, y0, x1, y1), -slope, 0, x1 * slope);
			}
			break;
		case ROOF_HIP: hip_patches(v, slope, out); break;
		case ROOF_SHED:
			if (v.fall == 2) add(out, rect(x0, y0, x1, y1), 0, -slope, y1 * slope);
			else if (v.fall == 1) add(out, rect(x0, y0, x1, y1), -slope, 0, x1 * slope);
			else if (v.fall == 3) add(out, rect(x0, y0, x1, y1), slope, 0, -x0 * slope);
			else add(out, rect(x0, y0, x1, y1), 0, slope, -y0 * slope);
			break;
		default: { // sawtooth: a plan's walls leave no straight run for the glazing, so the teeth are ridges
			const float run = saw_step(K, v);
			for (float from = y0; from < y1 - SHORTEST_M; from += run) {
				const float to = std::min(y1, from + run), mid = (from + to) * HALF;
				add(out, rect(x0, from, x1, mid), 0, slope, -from * slope);
				add(out, rect(x0, mid, x1, to), 0, -slope, to * slope);
			}
		}
	}
	return out;
}

// The stretch of a -> b inside a convex ring, as shares of it; false when none.
bool inside_span(const Ring &ring, V2 a, V2 b, float &t0, float &t1) {
	t0 = 0;
	t1 = 1;
	for (size_t i = 0; i < ring.size(); i++) {
		const V2 p = ring[i], q = ring[(i + 1) % ring.size()];
		const float ex = q.x - p.x, ey = q.y - p.y;
		const float da = ex * (a.y - p.y) - ey * (a.x - p.x), db = ex * (b.y - p.y) - ey * (b.x - p.x);
		if (da < 0 && db < 0) return false;
		if (da < 0) t0 = std::max(t0, da / (da - db));
		else if (db < 0) t1 = std::min(t1, da / (da - db));
	}
	return t1 > t0;
}

struct Draw {
	Shell &s;
	const RoofHide *hide;
	void tri(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &out, const Lin &lin, int8_t fin) const {
		if (hide) hide->tri(s, a, b, c, out, lin, fin);
		else s.tri(a, b, c, out, lin, fin);
	}
	void quad(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, const Lin &lin, int8_t fin) const {
		if (hide) hide->quad(s, a, b, c, d, out, lin, fin);
		else s.quad(a, b, c, d, out, lin, fin);
	}
};
} // namespace

void emit_outline_roof(Shell &shell, const Placed &pl, const Volume &v, float eaves, const Lin &roof, int8_t roof_fin, int8_t wall_fin, const RoofHide *hide) {
	const Basis &lot = pl.lot;
	const Draw draw{ shell, hide };
	const Ring plan = ring_of(v);
	const std::vector<Ring> pieces = plan_triangles(plan);
	const std::vector<Patch> patches = patches_of(pl.data->shapes.roof, v);
	for (const Patch &patch : patches) {
		const Vector3 up = (lot.e2 - lot.e0 * patch.gx - lot.e1 * patch.gy).normalized();
		auto at = [&](V2 p) { return lot.at(p.x, p.y, eaves + patch.at(p)); };
		for (const Ring &piece : pieces) {
			const Ring cut = clip_to_convex(piece, patch.ring);
			for (size_t i = 2; i < cut.size(); i++) draw.tri(at(cut[0]), at(cut[i - 1]), at(cut[i]), up, roof, roof_fin);
		}
	}
	for (int side = 0; side < int(plan.size()); side++) {
		const V2 a = plan[size_t(side)], b = plan[(size_t(side) + 1) % plan.size()];
		const float length = std::hypot(b.x - a.x, b.y - a.y);
		if (length < SHORTEST_M) continue;
		const float nx = (b.y - a.y) / length, ny = (a.x - b.x) / length;
		const Vector3 out = (lot.e0 * nx + lot.e1 * ny).normalized();
		const Lin wall = wall_lin(pl, v, v.storeys - 1, side);
		for (const Patch &patch : patches) {
			float t0, t1;
			if (!inside_span(patch.ring, a, b, t0, t1) || (t1 - t0) * length < SHORTEST_M) continue;
			const V2 p{ a.x + (b.x - a.x) * t0, a.y + (b.y - a.y) * t0 }, q{ a.x + (b.x - a.x) * t1, a.y + (b.y - a.y) * t1 };
			// A wall along the seam of two planes is under the one on its inner side.
			const V2 in{ (p.x + q.x) * HALF - nx * PROBE_M, (p.y + q.y) * HALF - ny * PROBE_M };
			if (!contains_point(patch.ring, in)) continue;
			const float hp = patch.at(p), hq = patch.at(q);
			if (hp + hq < LOWEST_M) continue;
			const Vector3 p0 = lot.at(p.x, p.y, eaves), q0 = lot.at(q.x, q.y, eaves), p1 = lot.at(p.x, p.y, eaves + hp), q1 = lot.at(q.x, q.y, eaves + hq);
			if (hp < LOWEST_M) draw.tri(p0, q0, q1, out, wall, wall_fin); // a corner of the eaves is a triangle: a quad with two corners in one faces by its first three
			else if (hq < LOWEST_M) draw.tri(p0, q0, p1, out, wall, wall_fin);
			else draw.quad(p0, q0, q1, p1, out, wall, wall_fin);
		}
	}
}

} // namespace city::b40
