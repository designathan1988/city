// What a body holds of the air past its ground walls (see body_air.h).
#include "sim/buildings/b40/body_air.h"

#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/roof_shape.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
bool pitched_out(Roof r) { return r == ROOF_GABLE || r == ROOF_HIP || r == ROOF_SHED; } // a sawtooth's teeth end on its walls
bool flat_top(Roof r) { return r == ROOF_FLAT || r == ROOF_TERRACE; }

// What a bay's component stands out of its wall at a storey, and how far under that storey's floor it hangs (under 0: from the
// floor up); `over` is the height over the floor below which nothing of it hangs (a ground opening's canopy and awning).
struct Stand {
	float out = 0, under = 0, over = 0;
};
Stand stand_of(uint8_t comp, int level) {
	switch (comp) {
		case BAY_BALCONY: return { BALCONY_OUT_M, BALCONY_SLAB_M, 0 };
		case BAY_GOURMET: return { DEEP_BALCONY_OUT_M, DEEP_BALCONY_UNDER_M, 0 };
		case BAY_BRISE: return { SUN_RACK_OUT_M, 0, 0 };
		case BAY_SHOPFRONT: return { BALCONY_OUT_M, 0, GROUND_HANG_UNDER_M };
		case BAY_DOOR: return level == 0 ? Stand{ DOOR_CANOPY_OUT_M, 0, GROUND_HANG_UNDER_M } : Stand{};
		case BAY_DOUBLE_DOOR: return level == 0 ? Stand{ WIDE_DOOR_CANOPY_OUT_M, 0, GROUND_HANG_UNDER_M } : Stand{};
		default: return {};
	}
}

// The box a stretch of a face stands out by: from t0 to t1 along it (metres from its start), `out` past the wall.
Overhang strip(const FaceRun &f, float t0, float t1, float out, float under) {
	const float ex = (f.b.x - f.a.x) / f.length, ey = (f.b.y - f.a.y) / f.length;
	const Point2 p[4] = { { f.a.x + ex * t0, f.a.y + ey * t0 }, { f.a.x + ex * t1, f.a.y + ey * t1 },
		{ f.a.x + ex * t1 + f.out.x * out, f.a.y + ey * t1 + f.out.y * out }, { f.a.x + ex * t0 + f.out.x * out, f.a.y + ey * t0 + f.out.y * out } };
	Overhang o{ p[0].x, p[0].y, p[0].x, p[0].y, under };
	for (const Point2 &q : p) {
		o.x0 = std::min(o.x0, q.x); o.x1 = std::max(o.x1, q.x);
		o.y0 = std::min(o.y0, q.y); o.y1 = std::max(o.y1, q.y);
	}
	return o;
}

// The parts a block's bays stand out of its walls, storey by storey, and its reliefs that project.
void bays_out(const Body &b, const Volume &v, const std::vector<FaceRun> &runs, std::vector<Overhang> &out) {
	for (const FaceRun &f : runs) {
		if (f.side >= MAX_FACES) continue;
		const int n = bays_of(v, f.side);
		const float bay = f.length / float(n);
		for (int k = 0; k < v.storeys; k++) {
			const float floor = b.block_z(v, v.base + k);
			for (int i = 0; i < n; i++) {
				const Stand s = stand_of(bay_at(v, k, f.side, i), v.base + k);
				if (s.out > 0) out.push_back(strip(f, bay * float(i), bay * float(i + 1), s.out, floor - s.under + s.over));
				const float proud = relief_depth(v, f.side, i, k);
				if (proud > 0) out.push_back(strip(f, bay * float(i), bay * float(i + 1), proud, floor));
			}
		}
	}
}

// A roof's reach past its block: a pitched roof's eaves, each side as far as it reaches, its edge that far down the slope and its
// body under it; a flat roof's crown bands along every side.
void roof_out(const Body &b, const Volume &v, const std::vector<FaceRun> &runs, std::vector<Overhang> &out) {
	const float eaves = b.block_z(v, v.top());
	if (pitched_out(v.roof) && v.outline.empty()) {
		const RoofReach r = roof_reach(b, v);
		const float reach[4] = { r.front, r.right, r.back, r.left }; // by side: face_runs' order of a rectangle
		for (const FaceRun &f : runs)
			if (reach[f.side] > 0) out.push_back(strip(f, -EAVES_OUT_M, f.length + EAVES_OUT_M, reach[f.side], eaves - reach[f.side] * roof_slope(v) - ROOF_BODY_M));
		return;
	}
	if (!flat_top(v.roof)) return;
	for (const FaceRun &f : runs) {
		const auto band = [&](float reach, float middle, float h) { out.push_back(strip(f, -reach, f.length + reach, reach, eaves + middle - h / 2)); };
		if (v.crown == CROWN_CORNICE) band(CORNICE_OUT_M, -CORNICE_DROP_M, CORNICE_H_M);
		else if (v.crown == CROWN_DECO) {
			band(STEP_OUT_M, -CORNICE_DROP_M, STEP_H_M);
			band(STEP_LOW_OUT_M, -STEP_LOW_DROP_M, STEP_LOW_H_M);
		} else if (v.crown == CROWN_SLAB) band(CROWN_SLAB_OUT_M, CROWN_SLAB_UP_M, CROWN_SLAB_H_M);
	}
}
} // namespace

std::vector<FaceRun> face_runs(const Volume &v) {
	std::vector<Point2> pts;
	if (v.outline.empty()) pts = { { v.x, v.y }, { v.x + v.w, v.y }, { v.x + v.w, v.y + v.d }, { v.x, v.y + v.d } };
	else
		for (const Point2 &q : v.outline) pts.push_back({ v.x + q.x * v.w, v.y + q.y * v.d });
	float area = 0;
	for (size_t i = 0; i < pts.size(); i++) {
		const Point2 &a = pts[i], &b = pts[(i + 1) % pts.size()];
		area += a.x * b.y - b.x * a.y;
	}
	std::vector<FaceRun> out;
	for (size_t i = 0; i < pts.size(); i++) {
		Point2 a = pts[i], b = pts[(i + 1) % pts.size()];
		if (area < 0) std::swap(a, b);
		const float len = std::hypot(b.x - a.x, b.y - a.y);
		if (len < EPS) continue;
		out.push_back({ int(i), a, b, Point2{ (b.y - a.y) / len, -(b.x - a.x) / len }, len });
	}
	return out;
}

RoofReach roof_reach(const Body &b, const Volume &v) {
	RoofReach r;
	auto room = [](float &side, float free) { side = std::clamp(free, 0.f, side); };
	if (b.plan.W > 0 && b.plan.D > 0) { // the lot's lines, where the body knows its lot
		room(r.left, v.x + b.plan.W / 2);
		room(r.right, b.plan.W / 2 - (v.x + v.w));
		room(r.front, v.y);
		room(r.back, b.plan.D - (v.y + v.d));
	}
	const float eave_z = b.block_z(v, v.top());
	for (const Volume &o : b.volumes) {
		if (&o == &v || b.block_z(o, o.top()) < eave_z) continue; // a lower block: the eave passes over it
		const bool beside_x = o.y < v.y + v.d && o.y + o.d > v.y, beside_y = o.x < v.x + v.w && o.x + o.w > v.x;
		if (beside_x && o.x + o.w <= v.x + EPS) room(r.left, v.x - (o.x + o.w));
		if (beside_x && o.x >= v.x + v.w - EPS) room(r.right, o.x - (v.x + v.w));
		if (beside_y && o.y + o.d <= v.y + EPS) room(r.front, v.y - (o.y + o.d));
		if (beside_y && o.y >= v.y + v.d - EPS) room(r.back, o.y - (v.y + v.d));
	}
	return r;
}

std::vector<Overhang> body_overhangs(const Body &b) {
	std::vector<Overhang> out;
	for (const Volume &v : b.volumes) {
		if (v.mode == BLOCK_VOID) continue; // it takes space away: it holds none
		if (v.base > 0) out.push_back({ v.x, v.y, v.x + v.w, v.y + v.d, b.block_z(v, v.base) });
		const std::vector<FaceRun> runs = face_runs(v);
		bays_out(b, v, runs, out);
		roof_out(b, v, runs, out);
	}
	// What is hung on the building among its elements (a canopy over an entrance, an awning, a slab on posts).
	for (const LotElement &e : b.elements) {
		if (e.kind != EL_CANOPY && e.kind != EL_AWNING && e.kind != EL_SLAB) continue;
		const float under = e.kind == EL_AWNING ? e.z - e.d / 2 : e.z; // an awning slopes down to half its depth under its hem
		out.push_back({ e.x - e.w / 2, e.y - e.d / 2, e.x + e.w / 2, e.y + e.d / 2, under });
	}
	return out;
}

} // namespace city::b40
