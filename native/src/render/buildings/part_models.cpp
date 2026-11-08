// The facade part models, built straight into a mesh in the bay frame: unit models
// scaled by (sx, sy, sz) along / up / out and centred at (a, h, -depth). Railings with balusters, panelled
// doors, louvred shutters, brise fins, awnings, split AC condensers and porch lamps.
#include "render/buildings/facade_bay.h"
#include "core/shader_contracts/buildings.h"

#include "core/math.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;
constexpr uint32_t BRASS = 0xb08d57, LAMP_BODY = 0xf1ebdd; // door handles' brass, a wall lamp's body (the source game's)

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

void part_door(const PartModelsLook &K, Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m = centred(p);
	// the front faces of the leaf and its panel: the handle starts on the leaf's face
	const float leaf_z = (K.leaf_z + K.leaf_d / 2) * p.sz, panel_z = (K.panel_z + K.panel_d / 2) * p.sz;
	s.rect(m, -p.sx / 2, p.sx / 2, -p.sy / 2, p.sy / 2, leaf_z, lin, fin);
	for (float x : { -K.panel_x, K.panel_x }) {
		s.rect(m, (x - K.panel_w / 2) * p.sx, (x + K.panel_w / 2) * p.sx, (K.panel_y_hi - K.panel_tall / 2) * p.sy, (K.panel_y_hi + K.panel_tall / 2) * p.sy, panel_z, lin * K.panel_shade, fin);
		s.rect(m, (x - K.panel_w / 2) * p.sx, (x + K.panel_w / 2) * p.sx, (K.panel_y_lo - K.panel_short / 2) * p.sy, (K.panel_y_lo + K.panel_short / 2) * p.sy, panel_z, lin * K.panel_shade, fin);
	}
	unit_box(s, p, K.handle_x - K.door_handle_w / 2, K.handle_x + K.door_handle_w / 2, K.handle_y - p.look->part_handle_h_m / 2, K.handle_y + p.look->part_handle_h_m / 2, K.handle_z - K.handle_d / 2, K.handle_z + K.handle_d / 2, lin_of(BRASS), FIN_METAL);
}

// Louvred leaf: stiles, rails and nine slats as front faces, slats tilted out at the bottom.
void part_louvre(const PartModelsLook &K, Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m = centred(p);
	float hx = p.sx / 2, hy = p.sy / 2, hz = p.sz / 2, t = K.louvre_t * p.sx;
	s.rect(m, -hx, -hx + t, -hy, hy, hz, lin, fin);
	s.rect(m, hx - t, hx, -hy, hy, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, -hy, -hy + t, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, hy - t, hy, hz, lin, fin);
	s.rect(m, -hx + t, hx - t, -t / 2, t / 2, hz, lin, fin);
	float step = (p.sy - 2 * t) / float(K.slats), sh = step * K.slat_fill;
	for (int i = 0; i < K.slats; i++) {
		float y = -hy + t + (float(i) + HALF) * step;
		Vector3 a = m.at(-hx + t, y - sh / 2, hz), b = m.at(hx - t, y - sh / 2, hz), c = m.at(hx - t, y + sh / 2, 0), d = m.at(-hx + t, y + sh / 2, 0);
		s.quad(a, b, c, d, (m.e2 + m.e1).normalized(), lin * K.slat_shade, fin);
	}
}

void part_brise(Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	// Fins under a pixel are no geometry to draw, they beat against the pixels: one card, past where the blades end, and the fragment takes their share
	const Basis m = centred(p);
	s.card(m, -HALF * p.sx, HALF * p.sx, -HALF * p.sy, HALF * p.sy, HALF * p.sz, lin, CARD_BRISE, p.look->part_brise_fin_m / p.sx);
	if (s.veiled) return;
	s.slim = true;
	for (int i = 0; i < BRISE_FINS; i++) {
		float x = -HALF + (float(i) + HALF) / float(BRISE_FINS);
		unit_box(s, p, x - p.look->part_brise_fin_m / 2, x + p.look->part_brise_fin_m / 2, -HALF, HALF, -HALF, HALF, lin, fin, SKIP_E2_NEG | SKIP_E1_NEG);
	}
	s.slim = false;
}

// Awning: canvas falling from the wall (y 0) out to z 1 at slope 0.75, valance under its front edge.
void part_awning(const PartModelsLook &K, Shell &s, const Part &p, const Lin &lin, int8_t fin) {
	Basis m{ p.f->at(p.a, p.h, -p.depth), p.f->e0, p.f->e1, p.f->e2 };
	float drop = K.canvas_slope * p.sz;
	Vector3 a = m.at(-HALF * p.sx, 0, 0), b = m.at(HALF * p.sx, 0, 0), c = m.at(HALF * p.sx, -drop, p.sz), d = m.at(-HALF * p.sx, -drop, p.sz);
	Vector3 up = (p.f->e1 * p.sz + p.f->e2 * drop).normalized();
	s.quad(a, b, c, d, up, lin, fin);
	s.quad(a, b, c, d, -up, lin * HALF, fin);
	s.box(m, -HALF * p.sx, HALF * p.sx, -drop - K.valance_h * p.sy, -drop, p.sz - K.valance_t, p.sz, lin, fin, SKIP_NONE);
}

void part_lamp(Shell &s, const Part &p) {
	// All of it is the lamp: its five faces are lit, as a box of the Roadcraft's `lamp` material is.
	const Basis m = centred(p);
	const Vector3 c = m.at(0, 0, 0), x = m.e0 * (p.sx / 2), y = m.e1 * (p.sy / 2), z = m.e2 * (p.sz / 2);
	const Lin body = lin_of(LAMP_BODY);
	const auto face = [&](const Vector3 &mid, const Vector3 &u, const Vector3 &v, const Vector3 &out) {
		s.quad(mid - u - v, mid + u - v, mid + u + v, mid - u + v, out, body, FIN_NONE, SK_LAMP);
	};
	face(c + z, x, y, m.e2);
	face(c + x, y, z, m.e0);
	face(c - x, y, z, -m.e0);
	face(c + y, x, z, m.e1);
	face(c - y, x, z, -m.e1);
}

} // namespace city::b40
