// Pitched roofs: gable and hip with 0.45 m eaves all round, a 0.18 m roof
// body (fascia trim x 0.92, soffit trim x 0.7), ridge cap (roof x 0.85), gable triangles in the wall paint; shed roofs
// with their tall back wall and triangular end walls; sawtooth (north-light) roofs of glazed teeth. Built in a ridge frame: u along the ridge, s across, z up.
#include "render/buildings/facade.h"

#include "sim/buildings/b40/roof_shape.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float EAVES = 0.45f, THICK = 0.18f, FASCIA = 0.92f, SOFFIT = 0.7f, CAP_W = 0.14f, CAP_LIFT = 0.07f, CAP_SHADE = 0.85f;
constexpr float HALF = 0.5f, MULLION = 0.08f;

struct Pitch {
	Shell &s;
	Basis R;
	float L, S, slope;
	Lin roof, fascia, soffit, wall;
	int8_t roof_fin, trim_fin, wall_fin;
	Vector3 p(float u, float sv, float z) const { return R.at(u, sv, z); }
	// A roof plane polygon (3 or 4 corners) with its underside THICK below.
	void plane(const Vector3 *q, int n, const Vector3 &out) {
		Vector3 down = -R.e2 * THICK;
		if (n == 4) {
			s.quad(q[0], q[1], q[2], q[3], out, roof, roof_fin);
			s.quad(q[0] + down, q[1] + down, q[2] + down, q[3] + down, -out, soffit, trim_fin);
		} else {
			s.tri(q[0], q[1], q[2], out, roof, roof_fin);
			s.tri(q[0] + down, q[1] + down, q[2] + down, -out, soffit, trim_fin);
		}
	}
	// The vertical face of the roof body under the edge a-b (eaves fascia, barge board).
	void edge(const Vector3 &a, const Vector3 &b, const Vector3 &out) {
		Vector3 down = -R.e2 * THICK;
		s.quad(a, b, b + down, a + down, out, fascia, trim_fin);
	}
	void gable_hip(bool hip);
	void shed();
	void sawtooth(float n_step, const Lin &glass, int8_t glass_fin);
};

void Pitch::gable_hip(bool hip) {
	float ze = -EAVES * slope, H = S / 2, rise = H * slope;
	float U0 = -EAVES, U1 = L + EAVES, S0 = -EAVES, S1 = S + EAVES;
	float r0 = hip ? U0 + (H + EAVES) : U0, r1 = hip ? U1 - (H + EAVES) : U1;
	if (r0 > r1) r0 = r1 = L / 2;
	Vector3 a = p(U0, S0, ze), b = p(U1, S0, ze), c = p(U1, S1, ze), d = p(U0, S1, ze), k0 = p(r0, H, rise), k1 = p(r1, H, rise);
	Vector3 front = (R.e2 - R.e1 * slope).normalized(), back = (R.e2 + R.e1 * slope).normalized();
	Vector3 q[4] = { a, b, k1, k0 };
	plane(q, 4, front);
	Vector3 q2[4] = { d, c, k1, k0 };
	plane(q2, 4, back);
	edge(a, b, -R.e1);
	edge(d, c, R.e1);
	if (hip) {
		Vector3 t0[3] = { a, d, k0 }, t1[3] = { b, c, k1 };
		plane(t0, 3, (R.e2 - R.e0 * slope).normalized());
		plane(t1, 3, (R.e2 + R.e0 * slope).normalized());
		edge(a, d, -R.e0);
		edge(b, c, R.e0);
	} else {
		s.tri(p(0, 0, 0), p(0, S, 0), p(0, H, rise), -R.e0, wall, wall_fin);
		s.tri(p(L, 0, 0), p(L, S, 0), p(L, H, rise), R.e0, wall, wall_fin);
		edge(a, k0, -R.e0); edge(d, k0, -R.e0); edge(b, k1, R.e0); edge(c, k1, R.e0);
	}
	if (r1 > r0) s.box(Basis{ R.o, R.e0, R.e1, R.e2 }, r0, r1, H - CAP_W, H + CAP_W, rise, rise + CAP_LIFT, roof * CAP_SHADE, roof_fin, SKIP_E2_NEG);
}

void Pitch::shed() {
	float rise = S * slope, hi = rise + EAVES * slope, lo = -EAVES * slope;
	float U0 = -EAVES, U1 = L + EAVES, S0 = -EAVES, S1 = S + EAVES;
	Vector3 a = p(U0, S0, hi), b = p(U1, S0, hi), c = p(U1, S1, lo), d = p(U0, S1, lo);
	Vector3 q[4] = { a, b, c, d };
	plane(q, 4, (R.e2 + R.e1 * slope).normalized());
	edge(a, b, -R.e1); edge(d, c, R.e1); edge(a, d, -R.e0); edge(b, c, R.e0);
	s.quad(p(0, 0, 0), p(L, 0, 0), p(L, 0, rise), p(0, 0, rise), -R.e1, wall, wall_fin);
	s.tri(p(0, 0, 0), p(0, S, 0), p(0, 0, rise), -R.e0, wall, wall_fin);
	s.tri(p(L, 0, 0), p(L, S, 0), p(L, 0, rise), R.e0, wall, wall_fin);
}
// Teeth across the roof every min(6 m, depth): a slope rising toward the back, then a vertical north-light glazing
// down to the eaves, with end triangles in the wall paint.
void Pitch::sawtooth(float n_step, const Lin &glass, int8_t glass_fin) {
	float step = n_step, rise = step * HALF * slope;
	int n = std::max(1, int(std::round(S / step)));
	Vector3 up = (R.e2 - R.e1 * slope * HALF).normalized();
	for (int i = 0; i < n; i++) {
		float s0 = step * float(i), s1 = s0 + step;
		Vector3 q[4] = { p(0, s0, 0), p(L, s0, 0), p(L, s1, rise), p(0, s1, rise) };
		plane(q, 4, up);
		s.quad(p(0, s1, 0), p(L, s1, 0), p(L, s1, rise), p(0, s1, rise), R.e1, glass, glass_fin);
		s.box(Basis{ R.o, R.e0, R.e1, R.e2 }, 0, L, s1 - MULLION, s1, rise, rise + CAP_LIFT, fascia, trim_fin, SKIP_E2_NEG);
		s.tri(p(0, s0, 0), p(0, s1, 0), p(0, s1, rise), -R.e0, wall, wall_fin);
		s.tri(p(L, s0, 0), p(L, s1, 0), p(L, s1, rise), R.e0, wall, wall_fin);
	}
}
} // namespace

void emit_pitched(Shell &shell, const Placed &pl, const Volume &v, float eaves) {
	const Body &b = *pl.b;
	const Paint &rp = b.roof_of(v), &tp = b.paints[size_t(b.trim)], &wp = b.wall_of(v);
	Lin trim = paint_lin(tp);
	Pitch P{ shell, Basis{}, 0, 0, roof_slope(v), paint_lin(rp), trim * FASCIA, trim * SOFFIT, wall_lin(pl, v, v.storeys - 1, 0),
		rp.finish, tp.finish, wp.finish };
	shell.grime_floor = -FAR_AWAY;
	const Basis &lot = pl.lot;
	if (v.roof == ROOF_SHED) {
		int fall = std::max(0, v.fall);
		bool along_y = fall % 2 == 0; // falls toward the front (0) or back (2): ridge along x
		if (along_y) {
			bool to_back = fall == 2;
			P.R = Basis{ lot.at(v.x, to_back ? v.y : v.y + v.d, eaves), lot.e0, to_back ? lot.e1 : -lot.e1, lot.e2 };
			P.L = v.w; P.S = v.d;
		} else {
			bool to_right = fall == 1;
			P.R = Basis{ lot.at(to_right ? v.x : v.x + v.w, v.y, eaves), lot.e1, to_right ? lot.e0 : -lot.e0, lot.e2 };
			P.L = v.d; P.S = v.w;
		}
		P.shed();
		return;
	}
	if (v.roof == ROOF_SAWTOOTH) {
		P.R = Basis{ lot.at(v.x, v.y, eaves), lot.e0, lot.e1, lot.e2 };
		P.L = v.w;
		P.S = v.d;
		const Paint &g = pl.data->color("sawtooth_glass");
		P.sawtooth(saw_step(v), paint_lin(g), g.finish);
		return;
	}
	bool along_x = ridge_along_x(v);
	P.R = along_x ? Basis{ lot.at(v.x, v.y, eaves), lot.e0, lot.e1, lot.e2 } : Basis{ lot.at(v.x, v.y, eaves), lot.e1, lot.e0, lot.e2 };
	P.L = along_x ? v.w : v.d;
	P.S = along_x ? v.d : v.w;
	P.gable_hip(v.roof != ROOF_GABLE);
}

} // namespace city::b40
