// Pitched roofs: gable and hip with 0.45 m eaves where the lot and the body's other blocks leave the room, a 0.18 m roof
// body (fascia trim x 0.92, soffit trim x 0.7), ridge cap (roof x 0.85), gable triangles in the wall paint; shed roofs
// with their tall back wall and triangular end walls; sawtooth (north-light) roofs of glazed teeth. Built in a ridge frame: u along the ridge, s across, z up.
#include "render/buildings/facade.h"
#include "render/buildings/roof_clip.h"

#include "sim/buildings/b40/roof_shape.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float EAVES = 0.45f, THICK = 0.18f, FASCIA = 0.92f, SOFFIT = 0.7f, CAP_W = 0.14f, CAP_LIFT = 0.07f, CAP_SHADE = 0.85f;
constexpr float HALF = 0.5f, MULLION = 0.08f;

// How far a roof may reach past each side of its block (lot frame: -x, +x, the front, the back): its eaves, but never past the
// lot's own line (a wall on it ends flush, as a party wall does) nor into another block of the body that stands as high.
struct Reach {
	float left = EAVES, right = EAVES, front = EAVES, back = EAVES;
};

Reach reach_of(const Body &b, const Volume &v) {
	Reach r;
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

struct Pitch {
	Shell &s;
	Basis R;
	float L, S, slope;
	Lin roof, fascia, soffit, wall;
	int8_t roof_fin, trim_fin, wall_fin;
	const RoofHide *hide = nullptr; // what a taller volume of a body cut open encloses is left out
	bool lean = false; // the far level: the planes and the walls under them alone (no underside, edge board or cap: none shows from there)
	float out_u0 = EAVES, out_u1 = EAVES, out_s0 = EAVES, out_s1 = EAVES; // the reach past each end of the ridge and each side across it
	Vector3 p(float u, float sv, float z) const { return R.at(u, sv, z); }
	void tri(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &out, const Lin &lin, int8_t fin) {
		if (hide) hide->tri(s, a, b, c, out, lin, fin);
		else s.tri(a, b, c, out, lin, fin);
	}
	void quad(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, const Lin &lin, int8_t fin) {
		if (hide) hide->quad(s, a, b, c, d, out, lin, fin);
		else s.quad(a, b, c, d, out, lin, fin);
	}
	// A box along the ridge frame's u axis (ridge cap, glazing mullion), minus the stretches that are enclosed.
	void along(float u0, float u1, float s0, float s1, float z0, float z1, const Lin &lin, int8_t fin) {
		if (lean) return;
		Basis f{ R.o, R.e0, R.e1, R.e2 };
		if (!hide) {
			s.box(f, u0, u1, s0, s1, z0, z1, lin, fin, SKIP_E2_NEG);
			return;
		}
		for (const auto &span : hide->spans(p(0, (s0 + s1) / 2, 0), R.e0, u0, u1)) s.box(f, span.first, span.second, s0, s1, z0, z1, lin, fin, SKIP_E2_NEG);
	}
	// A roof plane polygon (3 or 4 corners) with its underside THICK below.
	void plane(const Vector3 *q, int n, const Vector3 &out) {
		Vector3 down = -R.e2 * THICK;
		if (n == 4) {
			quad(q[0], q[1], q[2], q[3], out, roof, roof_fin);
			if (!lean) quad(q[0] + down, q[1] + down, q[2] + down, q[3] + down, -out, soffit, trim_fin);
		} else {
			tri(q[0], q[1], q[2], out, roof, roof_fin);
			if (!lean) tri(q[0] + down, q[1] + down, q[2] + down, -out, soffit, trim_fin);
		}
	}
	// The vertical face of the roof body under the edge a-b (eaves fascia, barge board); none on a side with no reach: the wall
	// under it closes the roof there, in the same plane.
	void edge(const Vector3 &a, const Vector3 &b, const Vector3 &out, float reach) {
		if (lean || reach <= 0) return;
		Vector3 down = -R.e2 * THICK;
		quad(a, b, b + down, a + down, out, fascia, trim_fin);
	}
	void gable_hip(bool hip);
	void shed();
	void sawtooth(float n_step, const Lin &glass, int8_t glass_fin);
};

void Pitch::gable_hip(bool hip) {
	// A hip's four planes meet at its corners only with one reach all round: the least room of its sides.
	const float even = std::min({ out_u0, out_u1, out_s0, out_s1 });
	const float u0 = hip ? even : out_u0, u1 = hip ? even : out_u1, s0 = hip ? even : out_s0, s1 = hip ? even : out_s1;
	float z0 = -s0 * slope, z1 = -s1 * slope, H = S / 2, rise = H * slope;
	float U0 = -u0, U1 = L + u1, S0 = -s0, S1 = S + s1;
	float r0 = hip ? U0 + (H + even) : U0, r1 = hip ? U1 - (H + even) : U1;
	if (r0 > r1) r0 = r1 = L / 2;
	Vector3 a = p(U0, S0, z0), b = p(U1, S0, z0), c = p(U1, S1, z1), d = p(U0, S1, z1), k0 = p(r0, H, rise), k1 = p(r1, H, rise);
	Vector3 front = (R.e2 - R.e1 * slope).normalized(), back = (R.e2 + R.e1 * slope).normalized();
	Vector3 q[4] = { a, b, k1, k0 };
	plane(q, 4, front);
	Vector3 q2[4] = { d, c, k1, k0 };
	plane(q2, 4, back);
	edge(a, b, -R.e1, s0);
	edge(d, c, R.e1, s1);
	if (hip) {
		Vector3 t0[3] = { a, d, k0 }, t1[3] = { b, c, k1 };
		plane(t0, 3, (R.e2 - R.e0 * slope).normalized());
		plane(t1, 3, (R.e2 + R.e0 * slope).normalized());
		edge(a, d, -R.e0, u0);
		edge(b, c, R.e0, u1);
	} else {
		tri(p(0, 0, 0), p(0, S, 0), p(0, H, rise), -R.e0, wall, wall_fin);
		tri(p(L, 0, 0), p(L, S, 0), p(L, H, rise), R.e0, wall, wall_fin);
		edge(a, k0, -R.e0, u0); edge(d, k0, -R.e0, u0); edge(b, k1, R.e0, u1); edge(c, k1, R.e0, u1);
	}
	if (r1 > r0) along(r0, r1, H - CAP_W, H + CAP_W, rise, rise + CAP_LIFT, roof * CAP_SHADE, roof_fin);
}

void Pitch::shed() {
	float rise = S * slope, hi = rise + out_s0 * slope, lo = -out_s1 * slope;
	float U0 = -out_u0, U1 = L + out_u1, S0 = -out_s0, S1 = S + out_s1;
	Vector3 a = p(U0, S0, hi), b = p(U1, S0, hi), c = p(U1, S1, lo), d = p(U0, S1, lo);
	Vector3 q[4] = { a, b, c, d };
	plane(q, 4, (R.e2 + R.e1 * slope).normalized());
	edge(a, b, -R.e1, out_s0); edge(d, c, R.e1, out_s1); edge(a, d, -R.e0, out_u0); edge(b, c, R.e0, out_u1);
	quad(p(0, 0, 0), p(L, 0, 0), p(L, 0, rise), p(0, 0, rise), -R.e1, wall, wall_fin);
	tri(p(0, 0, 0), p(0, S, 0), p(0, 0, rise), -R.e0, wall, wall_fin);
	tri(p(L, 0, 0), p(L, S, 0), p(L, 0, rise), R.e0, wall, wall_fin);
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
		quad(p(0, s1, 0), p(L, s1, 0), p(L, s1, rise), p(0, s1, rise), R.e1, glass, glass_fin);
		along(0, L, s1 - MULLION, s1, rise, rise + CAP_LIFT, fascia, trim_fin);
		tri(p(0, s0, 0), p(0, s1, 0), p(0, s1, rise), -R.e0, wall, wall_fin);
		tri(p(L, s0, 0), p(L, s1, 0), p(L, s1, rise), R.e0, wall, wall_fin);
	}
}
} // namespace

void emit_pitched(Shell &shell, const Placed &pl, const Volume &v, float eaves) {
	const Body &b = *pl.b;
	const Paint &rp = b.roof_of(v), &tp = b.paints[size_t(b.trim)], &wp = b.wall_of(v);
	Lin trim = paint_lin(tp);
	Pitch P{ shell, Basis{}, 0, 0, roof_slope(v), paint_lin(rp), trim * FASCIA, trim * SOFFIT, wall_lin(pl, v, v.storeys - 1, 0),
		rp.finish, tp.finish, wp.finish };
	P.lean = pl.lean;
	shell.grime_floor = -FAR_AWAY;
	RoofHide hidden(pl, v);
	if (hidden.any()) P.hide = &hidden;
	if (!v.outline.empty()) {
		emit_outline_roof(shell, pl, v, eaves, P.roof, P.roof_fin, P.wall_fin, P.hide);
		return;
	}
	const Basis &lot = pl.lot;
	const Reach room = reach_of(b, v);
	// The frame's ridge along the lot's x, s running to the back (`flip`: to the front); or along its y, s to the right (`flip`: to the left).
	auto frame_x = [&](bool flip) {
		P.R = Basis{ lot.at(v.x, flip ? v.y + v.d : v.y, eaves), lot.e0, flip ? -lot.e1 : lot.e1, lot.e2 };
		P.L = v.w; P.S = v.d;
		P.out_u0 = room.left; P.out_u1 = room.right;
		P.out_s0 = flip ? room.back : room.front; P.out_s1 = flip ? room.front : room.back;
	};
	auto frame_y = [&](bool flip) {
		P.R = Basis{ lot.at(flip ? v.x + v.w : v.x, v.y, eaves), lot.e1, flip ? -lot.e0 : lot.e0, lot.e2 };
		P.L = v.d; P.S = v.w;
		P.out_u0 = room.front; P.out_u1 = room.back;
		P.out_s0 = flip ? room.right : room.left; P.out_s1 = flip ? room.left : room.right;
	};
	if (v.roof == ROOF_SHED) {
		int fall = std::max(0, v.fall);
		if (fall % 2 == 0) frame_x(fall != 2); // falls toward the front (0) or back (2): ridge along x
		else frame_y(fall != 1);
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
	if (ridge_along_x(v)) frame_x(false);
	else frame_y(false);
	P.gable_hip(v.roof != ROOF_GABLE);
}

} // namespace city::b40
