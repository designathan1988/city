// The facade part models, built straight into a mesh in the bay frame: unit models
// scaled by (sx, sy, sz) along / up / out and centred at (a, h, -depth). Railings with balusters, panelled
// doors, louvred shutters, brise fins, awnings, split AC condensers and porch lamps.
#include "render/buildings/facade_bay.h"

#include "core/math.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float LEAF_Z = -0.2f, LEAF_D = 0.6f, PANEL_W = 0.36f, PANEL_TALL = 0.5f, PANEL_SHORT = 0.3f, PANEL_Y_HI = 0.2f;
constexpr float PANEL_Y_LO = -0.3f, PANEL_X = 0.24f, PANEL_Z = 0.25f, PANEL_D = 0.35f, HANDLE_X = 0.4f, HANDLE_Y = -0.04f;
constexpr float DOOR_HANDLE_W = 0.1f, HANDLE_D = 1.6f, HANDLE_Z = 0.9f, PANEL_SHADE = 0.92f;
constexpr float LOUVRE_T = 0.09f, SLAT_FILL = 0.67f, SLAT_SHADE = 0.85f;
constexpr int SLATS = 9;
constexpr int BRISE_FINS = 6;
constexpr float CANVAS_SLOPE = 0.75f, VALANCE_H = 0.28f, VALANCE_T = 0.025f;
constexpr float HALF = 0.5f;
constexpr uint32_t BRASS = 0xb08d57, LAMP_WARM = 0xffefcf;

Basis centred(const Part &p) { return Basis{ p.f->at(p.a, p.h, -p.depth), p.f->e0, p.f->e1, p.f->e2 }; }
// Box in unit model coordinates (centre-relative, before scaling).
void unit_box(Shell &s, const Part &p, float x0, float x1, float y0, float y1, float z0, float z1, const Lin &lin, int8_t fin,
		uint8_t skip = SKIP_E2_NEG) {
	Basis m = centred(p);
	s.box(m, x0 * p.sx, x1 * p.sx, y0 * p.sy, y1 * p.sy, z0 * p.sz, z1 * p.sz, lin, fin, skip);
}
} // namespace

void part_box(Shell &s, const Part &p, const Lin &lin, int8_t fin) { unit_box(s, p, -HALF, HALF, -HALF, HALF, -HALF, HALF, lin, fin); }

// Railing: y from 0 to 1 (bottom at the part's h), z from the wall (0) out to 1; the bottom rail and the balusters
// are single front faces (a thin box seen from the street shows only its front).
void part_railing(Shell &s, const Part &p, const Lin &lin, int8_t) {
	Basis m{ p.f->at(p.a, p.h, -p.depth), p.f->e0, p.f->e1, p.f->e2 };
	const float hx = p.sx / 2, z1 = p.sz;
	s.card(m, -hx, hx, 0, p.sy, z1, lin, CARD_RAILING_FRONT);
	Basis left{ m.at(-hx, 0, 0), m.e2, m.e1, -m.e0 }, right{ m.at(hx, 0, 0), m.e2, m.e1, m.e0 };
	s.card(left, 0, z1, 0, p.sy, 0, lin, CARD_RAILING_SIDE);
	s.card(right, 0, z1, 0, p.sy, 0, lin, CARD_RAILING_SIDE);
}

void part_door(Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m = centred(p);
	// the front faces of the leaf and its panel: the handle starts on the leaf's face
	const float leaf_z = (LEAF_Z + LEAF_D / 2) * p.sz, panel_z = (PANEL_Z + PANEL_D / 2) * p.sz;
	s.rect(m, -p.sx / 2, p.sx / 2, -p.sy / 2, p.sy / 2, leaf_z, lin, fin);
	for (float x : { -PANEL_X, PANEL_X }) {
		s.rect(m, (x - PANEL_W / 2) * p.sx, (x + PANEL_W / 2) * p.sx, (PANEL_Y_HI - PANEL_TALL / 2) * p.sy, (PANEL_Y_HI + PANEL_TALL / 2) * p.sy, panel_z, lin * PANEL_SHADE, fin);
		s.rect(m, (x - PANEL_W / 2) * p.sx, (x + PANEL_W / 2) * p.sx, (PANEL_Y_LO - PANEL_SHORT / 2) * p.sy, (PANEL_Y_LO + PANEL_SHORT / 2) * p.sy, panel_z, lin * PANEL_SHADE, fin);
	}
	unit_box(s, p, HANDLE_X - DOOR_HANDLE_W / 2, HANDLE_X + DOOR_HANDLE_W / 2, HANDLE_Y - p.look->part_handle_h_m / 2, HANDLE_Y + p.look->part_handle_h_m / 2, HANDLE_Z - HANDLE_D / 2, HANDLE_Z + HANDLE_D / 2, lin_of(BRASS), FIN_METAL);
}

// Louvred leaf: stiles, rails and nine slats as front faces, slats tilted out at the bottom.
void part_louvre(Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m = centred(p);
	float hx = p.sx / 2, hy = p.sy / 2, hz = p.sz / 2, t = LOUVRE_T * p.sx;
	s.rect(m, -hx, -hx + t, -hy, hy, hz, lin, fin);
	s.rect(m, hx - t, hx, -hy, hy, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, -hy, -hy + t, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, hy - t, hy, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, -t / 2, t / 2, hz, lin, fin);
	float step = (p.sy - 2 * t) / float(SLATS), sh = step * SLAT_FILL;
	for (int i = 0; i < SLATS; i++) {
		float y = -hy + t + (float(i) + HALF) * step;
		Vector3 a = m.at(-hx + t, y - sh / 2, hz), b = m.at(hx - t, y - sh / 2, hz), c = m.at(hx - t, y + sh / 2, 0), d = m.at(-hx + t, y + sh / 2, 0);
		s.quad(a, b, c, d, (m.e2 + m.e1).normalized(), lin * SLAT_SHADE, fin);
	}
}

void part_brise(Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	for (int i = 0; i < BRISE_FINS; i++) {
		float x = -HALF + (float(i) + HALF) / float(BRISE_FINS);
		unit_box(s, p, x - p.look->part_brise_fin_m / 2, x + p.look->part_brise_fin_m / 2, -HALF, HALF, -HALF, HALF, lin, fin, SKIP_E2_NEG | SKIP_E1_NEG);
	}
}

// Awning: canvas falling from the wall (y 0) out to z 1 at slope 0.75, valance under its front edge.
void part_awning(Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m{ p.f->at(p.a, p.h, -p.depth), p.f->e0, p.f->e1, p.f->e2 };
	float drop = CANVAS_SLOPE * p.sz;
	Vector3 a = m.at(-HALF * p.sx, 0, 0), b = m.at(HALF * p.sx, 0, 0), c = m.at(HALF * p.sx, -drop, p.sz), d = m.at(-HALF * p.sx, -drop, p.sz);
	Vector3 up = (p.f->e1 * p.sz + p.f->e2 * drop).normalized();
	s.quad(a, b, c, d, up, lin, fin);
	s.quad(a, b, c, d, -up, lin * HALF, fin);
	s.box(m, -HALF * p.sx, HALF * p.sx, -drop - VALANCE_H * p.sy, -drop, p.sz - VALANCE_T, p.sz, lin, fin, SKIP_NONE);
}

void part_lamp(Shell &s, const Part &p) {
	Basis m = centred(p);
	Vector3 c = m.at(0, 0, 0);
	Vector3 q[4] = { c + m.e0 * (-p.sx / 2) + m.e1 * (-p.sy / 2) + m.e2 * (p.sz / 2), c + m.e0 * (p.sx / 2) + m.e1 * (-p.sy / 2) + m.e2 * (p.sz / 2),
		c + m.e0 * (p.sx / 2) + m.e1 * (p.sy / 2) + m.e2 * (p.sz / 2), c + m.e0 * (-p.sx / 2) + m.e1 * (p.sy / 2) + m.e2 * (p.sz / 2) };
	Lin warm = lin_of(LAMP_WARM);
	s.quad(q[0], q[1], q[2], q[3], m.e2, warm, FIN_NONE, SK_LAMP);
	part_box(s, Part{ p.f, p.a, p.h, p.depth - p.sz / 2, p.sx, p.sy, p.sz / 2 }, warm * HALF, FIN_METAL);
}

} // namespace city::b40
