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

Color hexc(const DataValue &v) { return rgba8_to_color(v.colour()); }

// A lump resting on the ground, one unit tall: the crown heart of a tree with no cards over it.
void lump(MeshBuilder &mb, const DataValue &k, uint32_t seed, Rng &r) {
	Shade sh{ Tones{ hexc(k["dark"]), hexc(k["lit"]) }, k["jitter"].f(), k["shade_bottom"].f(), 1.f, 1.f, BloomSpec{} };
	MeshBuilder none;
	CrownSpec c{ Vector3(0, HALF_F, 0), Vector3(HALF_F, HALF_F, HALF_F), k["rough"].f(), seed, k["subdiv"].whole(), 0, 0, 0 };
	static_cast<void>(tree_crown(mb, none, c, sh, r)); // its mean tone is for a tree's far card; ground pieces have none
}

// A column with `arms` bent arms part way up.
void cactus(MeshBuilder &mb, const DataValue &k, Rng &r) {
	const Tones tone{ Color(0, 0, 0, 1), hexc(k["lit"]) };
	float rad = k["radius"].f(), arm_from = k["arm_from"].f(), arm_reach = k["arm_reach"].f(), arm_rise = k["arm_rise"].f();
	int sides = k["ribs"].whole(), arms = k["arms"].whole();
	tree_tube(mb, Vector3(), Vector3(0, 1, 0), rad, rad * HALF_F, sides, tone, TreePart::BODY);
	for (int i = 0; i < arms; i++) {
		float a = TAU_F * (float(i) + r.f()) / float(arms), y = arm_from + r.f() * (1.f - arm_from) * HALF_F;
		Vector3 out(std::cos(a) * arm_reach, 0, std::sin(a) * arm_reach), root(0, y, 0), bend = root + out;
		tree_tube(mb, root, bend, rad * HALF_F, rad * HALF_F, sides, tone, TreePart::BODY);
		tree_tube(mb, bend, bend + Vector3(0, arm_rise, 0), rad * HALF_F, rad * HALF_F * HALF_F, sides, tone, TreePart::BODY);
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

void tuft(MeshBuilder &mb, const DataValue &k, Rng &r) {
	int blades = k["blades"].whole();
	float width = k["blade_width"].f(), lean = k["blade_lean"].f();
	Color foot = hexc(k["dark"]), top = hexc(k["lit"]);
	for (int i = 0; i < blades; i++) {
		float a = TAU_F * r.f(), reach = r.f() * HALF_F;
		Vector3 root(std::cos(a) * reach, 0, std::sin(a) * reach);
		blade(mb, root, root * (1.f + lean) + Vector3(0, 1.f - r.f() * HALF_F * HALF_F, 0), width, foot, top);
	}
}

void flower(MeshBuilder &mb, const DataValue &k, const Color &bloom, uint32_t seed, Rng &r) {
	float stem = k["stem_radius"].f(), head = k["head_radius"].f();
	tree_tube(mb, Vector3(), Vector3(0, 1.f - head, 0), stem, stem, STEM_SIDES, Tones{ Color(0, 0, 0, 1), hexc(k["dark"]) }, TreePart::BODY);
	Color shade = bloom * k["head_shade"].f();
	shade.a = 1;
	Shade sh{ Tones{ shade, bloom }, 0.f, 1.f, 1.f, 1.f, BloomSpec{} };
	MeshBuilder none;
	CrownSpec c{ Vector3(0, 1.f - head, 0), Vector3(head, head, head), 0.f, seed, HEAD_SUBDIV, 0, 0, 0 };
	static_cast<void>(tree_crown(mb, none, c, sh, r)); // its mean tone is for a tree's far card; ground pieces have none
}

Ref<ArrayMesh> variant(CoverPiece p, const DataValue &k, int i, Rng &r) {
	MeshBuilder mb;
	uint32_t seed = uint32_t(k["seed"].whole() + i);
	switch (p) {
		case CoverPiece::ROCKS:
		case CoverPiece::SCRUB: lump(mb, k, seed, r); break;
		case CoverPiece::CACTUS: cactus(mb, k, r); break;
		case CoverPiece::TUFTS: tuft(mb, k, r); break;
		default: flower(mb, k, hexc(k["blooms"][i]), seed, r); break;
	}
	return mb.commit();
}
} // namespace

bool CoverKit::build(const DataValue &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	reach_m = t["reach_m"].f(); fade_m = t["fade_m"].f(); tuft_share = t["tuft_share"].f();
	if (errors.size() != before) return false;
	for (int i = 0; i < COVER_PIECES; i++) {
		const DataValue k = t[PIECE_KEYS[i]];
		CoverLook &c = looks_[size_t(i)];
		const DataValue size = k["size_m"];
		size.list(2);
		c.spacing_m = k["spacing_m"].f(); c.least = k["least"].f(); c.size0 = size[0].f(); c.size1 = size[1].f(); c.tall = k["upright"].f(); c.sink = k["sink"].f();
		c.tint_vary = k["tint_vary"].f(); c.road_margin_m = k["road_margin_m"].f(); c.casts = k["casts"].flag(); c.sways = k["sways"].flag();
		Rng r{ uint32_t(k["seed"].whole()) };
		const int n = k["variants"].whole();
		c.variants.clear();
		if (errors.size() != before) continue; // nothing is built from a row that did not read
		for (int v = 0; v < n; v++) c.variants.push_back(variant(CoverPiece(i), k, v, r));
	}
	return errors.size() == before;
}

} // namespace city
