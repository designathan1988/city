// Builds the cover's meshes (see cover_kit.h) from data/render/cover.json.
#include "render/cover/cover_kit.h"

#include "core/color.h"
#include "core/math.h"
#include "core/random.h"
#include "render/trees/tree_kit.h"
#include "render/trees/tree_parts.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF_F = 0.5f;
constexpr int HEAD_SUBDIV = 1, STEM_SIDES = 3;
const char *const PIECE_KEYS[COVER_PIECES] = { "rocks", "scrub", "cactus", "tufts", "flowers" };

Color hexc(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

// A lump resting on the ground, one unit tall: the crown heart of a tree with no cards over it.
void lump(MeshBuilder &mb, const Dictionary &k, uint32_t seed, Rng &r) {
	Shade sh{ Tones{ hexc(k["dark"]), hexc(k["lit"]) }, float(k["jitter"]), float(k["shade_bottom"]), 1.f, 1.f, BloomSpec{} };
	MeshBuilder none;
	CrownSpec c{ Vector3(0, HALF_F, 0), Vector3(HALF_F, HALF_F, HALF_F), float(k["rough"]), seed, int(k["subdiv"]), 0, 0, 0 };
	tree_crown(mb, none, c, sh, r);
}

// A column with `arms` bent arms part way up.
void cactus(MeshBuilder &mb, const Dictionary &k, Rng &r) {
	Color tone = hexc(k["lit"]);
	float rad = k["radius"], arm_from = k["arm_from"], arm_reach = k["arm_reach"], arm_rise = k["arm_rise"];
	int sides = k["ribs"], arms = k["arms"];
	tree_tube(mb, Vector3(), Vector3(0, 1, 0), rad, rad * HALF_F, sides, tone);
	for (int i = 0; i < arms; i++) {
		float a = TAU_F * (float(i) + r.f()) / float(arms), y = arm_from + r.f() * (1.f - arm_from) * HALF_F;
		Vector3 out(std::cos(a) * arm_reach, 0, std::sin(a) * arm_reach), root(0, y, 0), bend = root + out;
		tree_tube(mb, root, bend, rad * HALF_F, rad * HALF_F, sides, tone);
		tree_tube(mb, bend, bend + Vector3(0, arm_rise, 0), rad * HALF_F, rad * HALF_F * HALF_F, sides, tone);
	}
}

// A blade from the root to its tip, seen from both sides.
void blade(MeshBuilder &mb, const Vector3 &root, const Vector3 &tip, float width, const Color &foot, const Color &top) {
	const Vector2 tag(float(int(TreePart::BODY)), 0);
	Vector3 side = (tip - root).cross(Vector3(0, 0, 1)).normalized() * (width * HALF_F);
	Vector3 n = side.cross(tip - root).normalized();
	if (n.y < 0) n = -n;
	int a = mb.add(root - side, n, foot, Vector2(), tag), b = mb.add(root + side, n, foot, Vector2(), tag), c = mb.add(tip, n, top, Vector2(), tag);
	for (int i : { a, b, c, a, c, b }) mb.idx.push_back(i);
}

void tuft(MeshBuilder &mb, const Dictionary &k, Rng &r) {
	int blades = k["blades"];
	float width = k["blade_width"], lean = k["blade_lean"];
	Color foot = hexc(k["dark"]), top = hexc(k["lit"]);
	for (int i = 0; i < blades; i++) {
		float a = TAU_F * r.f(), reach = r.f() * HALF_F;
		Vector3 root(std::cos(a) * reach, 0, std::sin(a) * reach);
		blade(mb, root, root * (1.f + lean) + Vector3(0, 1.f - r.f() * HALF_F * HALF_F, 0), width, foot, top);
	}
}

void flower(MeshBuilder &mb, const Dictionary &k, const Color &bloom, uint32_t seed, Rng &r) {
	float stem = k["stem_radius"], head = k["head_radius"];
	tree_tube(mb, Vector3(), Vector3(0, 1.f - head, 0), stem, stem, STEM_SIDES, hexc(k["dark"]));
	Color shade = bloom * float(k["head_shade"]);
	shade.a = 1;
	Shade sh{ Tones{ shade, bloom }, 0.f, 1.f, 1.f, 1.f, BloomSpec{} };
	MeshBuilder none;
	CrownSpec c{ Vector3(0, 1.f - head, 0), Vector3(head, head, head), 0.f, seed, HEAD_SUBDIV, 0, 0, 0 };
	tree_crown(mb, none, c, sh, r);
}

Ref<ArrayMesh> variant(CoverPiece p, const Dictionary &k, int i, Rng &r) {
	MeshBuilder mb;
	uint32_t seed = uint32_t(int(k["seed"]) + i);
	switch (p) {
		case CoverPiece::ROCKS:
		case CoverPiece::SCRUB: lump(mb, k, seed, r); break;
		case CoverPiece::CACTUS: cactus(mb, k, r); break;
		case CoverPiece::TUFTS: tuft(mb, k, r); break;
		default: flower(mb, k, hexc(Array(k["blooms"])[i]), seed, r); break;
	}
	return mb.commit();
}
} // namespace

bool CoverKit::build(const Dictionary &t, PackedStringArray &errors) {
	static const char *const TOP[] = { "reach_m", "fade_m", "pop_m", "tuft_share" };
	for (const char *key : TOP)
		if (!t.has(key)) errors.push_back(String("cover.json: falta ") + key);
	if (!errors.is_empty()) return false;
	reach_m = t["reach_m"]; fade_m = t["fade_m"]; pop_m = t["pop_m"]; tuft_share = t["tuft_share"];
	for (int i = 0; i < COVER_PIECES; i++) {
		if (!t.has(PIECE_KEYS[i])) {
			errors.push_back(String("cover.json: falta ") + PIECE_KEYS[i]);
			continue;
		}
		Dictionary k = t[PIECE_KEYS[i]];
		CoverLook &c = looks_[size_t(i)];
		Array size = k["size_m"];
		c.spacing_m = k["spacing_m"]; c.least = k["least"]; c.size0 = size[0]; c.size1 = size[1]; c.tall = k["upright"]; c.sink = k["sink"];
		c.tint_vary = k["tint_vary"]; c.road_margin_m = k["road_margin_m"]; c.casts = k["casts"]; c.sways = k["sways"];
		Rng r{ uint32_t(int(k["seed"])) };
		int n = k["variants"];
		c.variants.clear();
		for (int v = 0; v < n; v++) c.variants.push_back(variant(CoverPiece(i), k, v, r));
	}
	return errors.is_empty();
}

} // namespace city
