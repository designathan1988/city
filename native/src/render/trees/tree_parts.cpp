// Tree emitters (see tree_parts.h). Cards stand on a golden-angle spiral of directions over the crown (an even cover
// without clumps, each nudged by the crown's own noise), every card's four corners at its centre: the vertex shader
// spreads them across the view.
#include "render/trees/tree_parts.h"

#include "core/math.h"
#include "core/random.h"
#include "render/trees/tree_kit.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float HALF_F = 0.5f;
constexpr float GOLDEN = 1.61803398875f;
constexpr float GOLDEN_ANGLE = 2.39996322973f; // radians between successive spiral points (pi * (3 - sqrt 5))
constexpr int ICO_VERTS = 12, ICO_FACES = 20;
enum IcoVertex : int { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11 };
constexpr int ICO[ICO_FACES][3] = { { V0, V11, V5 }, { V0, V5, V1 }, { V0, V1, V7 }, { V0, V7, V10 }, { V0, V10, V11 }, { V1, V5, V9 }, { V5, V11, V4 },
	{ V11, V10, V2 }, { V10, V7, V6 }, { V7, V1, V8 }, { V3, V9, V4 }, { V3, V4, V2 }, { V3, V2, V6 }, { V3, V6, V8 }, { V3, V8, V9 }, { V4, V9, V5 },
	{ V2, V4, V11 }, { V6, V2, V10 }, { V8, V6, V7 }, { V9, V8, V1 } };
// A face's light between the dark and lit tones: hemisphere lighting, a point on the equator takes half from the sky
// (Rost, OpenGL Shading Language, 3rd ed., 12.1).
constexpr float FACE_BASE = 0.5f, FACE_UP = 0.5f;
constexpr float NOISE_FREQ = 1.f; // one noise cell across a crown radius
// A card's size varies by this share either way; it stands this share of its size out of the heart (min, span).
constexpr float CARD_VARY = 0.25f, CARD_OUT_MIN = 0.1f, CARD_OUT_SPAN = 0.2f;
// A card's tone: from this share of the lit tone low in the crown to all of it at the top, varied by this share.
constexpr float CARD_LOW_TONE = 0.45f, CARD_TONE_VARY = 0.12f;
// A card's normal is lifted towards the sky by this share, so the canopy's top takes the sky light.
constexpr float CARD_SKY_LIFT = 0.2f;
// A frond row's tone: its share of the lit tone at a level row, plus this much per unit of its up axis.
constexpr float FROND_ROW_BASE = 0.5f, FROND_TONE_UP = 0.3f;
constexpr int FROND_COLS = 3; // left edge, rachis, right edge
const Vector3 UP(0, 1, 0);

using Tri = std::array<Vector3, 3>;

std::vector<Tri> icosphere(int subdiv) {
	const Vector3 v[ICO_VERTS] = { Vector3(-1, GOLDEN, 0), Vector3(1, GOLDEN, 0), Vector3(-1, -GOLDEN, 0), Vector3(1, -GOLDEN, 0),
		Vector3(0, -1, GOLDEN), Vector3(0, 1, GOLDEN), Vector3(0, -1, -GOLDEN), Vector3(0, 1, -GOLDEN), Vector3(GOLDEN, 0, -1),
		Vector3(GOLDEN, 0, 1), Vector3(-GOLDEN, 0, -1), Vector3(-GOLDEN, 0, 1) };
	std::vector<Tri> tris;
	for (const auto &f : ICO) tris.push_back({ v[f[0]].normalized(), v[f[1]].normalized(), v[f[2]].normalized() });
	for (int s = 0; s < subdiv; s++) {
		std::vector<Tri> next;
		for (const Tri &t : tris) {
			Vector3 a = (t[0] + t[1]).normalized(), b = (t[1] + t[2]).normalized(), c = (t[2] + t[0]).normalized();
			next.push_back({ t[0], a, c });
			next.push_back({ t[1], b, a });
			next.push_back({ t[2], c, b });
			next.push_back({ a, b, c });
		}
		tris.swap(next);
	}
	return tris;
}

Color with_alpha(Color c, float a) {
	c.a = a;
	return c;
}

void flat_tri(MeshBuilder &mb, const Vector3 p[3], const Vector3 &out, const Color &col) {
	const Vector2 tag(float(int(TreePart::BODY)), 0);
	int a = mb.add(p[0], out, col, Vector2(), tag), b = mb.add(p[1], out, col, Vector2(), tag), c = mb.add(p[2], out, col, Vector2(), tag);
	mb.tri(a, b, c, out);
}

float face_light(const Vector3 &n) { return clampf(FACE_BASE + FACE_UP * n.y, 0.f, 1.f); }

// One card at `c` (all four corners there), `half` its half size, `n` its outward normal.
void card(MeshBuilder &mb, const Vector3 &c, const Vector3 &n, const Color &tone, float half, float roll) {
	const Vector2 tag(float(int(TreePart::CARD)), half);
	const Vector2 corner[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
	Vector3 lifted = Vector3(n.x, n.y * (1.f - CARD_SKY_LIFT) + CARD_SKY_LIFT, n.z).normalized();
	int base = -1;
	for (int k = 0; k < 4; k++) {
		int i = mb.add(c, lifted, with_alpha(tone, roll), corner[k], tag);
		if (k == 0) base = i;
	}
	// The corners coincide here, so the winding is the shader's to make (the cards are drawn two-sided).
	for (int k : { 0, 1, 2, 0, 2, 3 }) mb.idx.push_back(base + k);
}
} // namespace

void tree_tube(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, float r0, float r1, int sides, const Color &col) {
	Vector3 axis = (b - a).normalized();
	Vector3 side = std::fabs(axis.y) < (1.f - CARD_SKY_LIFT) ? axis.cross(UP).normalized() : Vector3(1, 0, 0);
	Vector3 fwd = axis.cross(side).normalized();
	for (int i = 0; i < sides; i++) {
		float a0 = TAU_F * i / sides, a1 = TAU_F * (i + 1) / sides;
		Vector3 d0 = side * std::cos(a0) + fwd * std::sin(a0), d1 = side * std::cos(a1) + fwd * std::sin(a1);
		Vector3 p[4] = { a + d0 * r0, a + d1 * r0, b + d1 * r1, b + d0 * r1 };
		Vector3 out = (d0 + d1).normalized();
		Color c = col * face_light(out);
		c.a = 1;
		const Vector3 t0[3] = { p[0], p[1], p[2] }, t1[3] = { p[0], p[2], p[3] };
		flat_tri(mb, t0, out, c);
		flat_tri(mb, t1, out, c);
	}
}

Color tree_crown(MeshBuilder &body, MeshBuilder &cards, const CrownSpec &c, const Shade &sh, Rng &r) {
	auto displace = [&](const Vector3 &d) {
		float k = 1.f + vnoise(d.x * NOISE_FREQ + d.y, d.z * NOISE_FREQ - d.y, c.seed) * c.rough;
		return Vector3(d.x * c.radius.x, d.y * c.radius.y, d.z * c.radius.z) * k;
	};
	auto tones_at = [&](float y) { return sh.bloom.on && y > sh.bloom.above ? sh.bloom.tones : sh.leaf; };
	auto height01 = [&](float y) { return clampf((y + c.radius.y) / (2.f * c.radius.y), 0.f, 1.f); };
	bool covered = c.cards > 0;
	float shrink = covered ? sh.heart_shrink : 1.f, dim = covered ? sh.heart_shade : 1.f;
	// The far card takes the mean of what is seen of the crown from afar: its cards, or its faces when it has none.
	Color faces, leaves;
	int nf = 0, nl = 0;
	for (const Tri &t : icosphere(c.subdiv)) {
		Vector3 p[3] = { c.centre + displace(t[0]) * shrink, c.centre + displace(t[1]) * shrink, c.centre + displace(t[2]) * shrink };
		Vector3 nn = (p[1] - p[0]).cross(p[2] - p[0]).normalized();
		Vector3 mid = (p[0] + p[1] + p[2]) / 3.f;
		if (nn.dot(mid - c.centre) < 0) nn = -nn;
		float k = clampf(face_light(nn) + (r.f() - HALF_F) * sh.jitter, 0.f, 1.f);
		float low = sh.shade_bottom + (1.f - sh.shade_bottom) * height01(mid.y - c.centre.y);
		Color col = tones_at(mid.y).tone(k) * (low * dim);
		flat_tri(body, p, nn, with_alpha(col, 1));
		faces += col;
		nf++;
	}
	for (int i = 0; i < c.cards; i++) {
		// The golden-angle spiral: z from top to bottom of the sphere, the azimuth turning by the golden angle each step.
		float z = 1.f - (2.f * i + 1.f) / c.cards, ring = std::sqrt(std::max(0.f, 1.f - z * z)), az = GOLDEN_ANGLE * i;
		Vector3 d = Vector3(std::cos(az) * ring, z, std::sin(az) * ring);
		Vector3 surf = c.centre + displace(d), out = (surf - c.centre).normalized();
		float h = height01(surf.y - c.centre.y);
		float size = c.card * (1.f + (r.f() * 2.f - 1.f) * CARD_VARY) * (1.f - c.taper * h);
		Vector3 at = surf + out * size * (CARD_OUT_MIN + CARD_OUT_SPAN * r.f());
		float k = clampf((CARD_LOW_TONE + (1.f - CARD_LOW_TONE) * h) * (1.f + (r.f() * 2.f - 1.f) * CARD_TONE_VARY), 0.f, 1.f);
		Color col = tones_at(at.y).tone(k);
		card(cards, at, out, col, size * HALF_F, r.f());
		leaves += col;
		nl++;
	}
	if (nl > 0) return with_alpha(leaves / float(nl), 1);
	return nf > 0 ? with_alpha(faces / float(nf), 1) : sh.leaf.lit;
}

void tree_fronds(MeshBuilder &cards, const Vector3 &top, const FrondSpec &f, const Tones &tones, Rng &r) {
	const Vector2 tag(float(int(TreePart::FROND)), 0);
	for (int k = 0; k < f.count; k++) {
		bool young = k >= f.count - f.young;
		float az = TAU_F * (k + r.f()) / f.count;
		Vector3 out(std::cos(az), 0, std::sin(az)), side(-std::sin(az), 0, std::cos(az));
		float len = young ? f.young_len : r.range(f.len0, f.len1);
		float rise = young ? f.young_rise : r.range(f.rise0, f.rise1), droop = young ? f.young_droop : r.range(f.droop0, f.droop1);
		Vector3 ctrl = top + out * (len * HALF_F) + UP * (len * rise), tip = top + out * len + UP * (len * droop);
		int first = int(cards.v.size());
		for (int i = 0; i <= f.steps; i++) {
			float t = float(i) / f.steps, u = 1.f - t;
			Vector3 p = top * (u * u) + ctrl * (2.f * u * t) + tip * (t * t);
			Vector3 along = ((ctrl - top) * u + (tip - ctrl) * t).normalized();
			Vector3 up = side.cross(along).normalized();
			if (up.y < 0) up = -up;
			Color col = with_alpha(tones.tone(clampf(FROND_ROW_BASE + FROND_TONE_UP * up.y, 0.f, 1.f)), 1);
			Vector3 drop = UP * (-f.width * f.fold);
			const Vector3 row[FROND_COLS] = { p - side * f.width + drop, p, p + side * f.width + drop };
			for (int c = 0; c < FROND_COLS; c++)
				cards.add(row[c], up, col, Vector2(c * HALF_F, 1.f - t), tag); // the frond's base is the atlas region's bottom row
		}
		for (int i = 0; i < f.steps; i++)
			for (int c = 0; c + 1 < FROND_COLS; c++) {
				int a = first + i * FROND_COLS + c, b = a + 1, d = a + FROND_COLS, e = d + 1;
				for (int q : { a, b, e, a, e, d }) cards.idx.push_back(q);
			}
	}
}

} // namespace city
