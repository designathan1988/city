// Builds the tree species meshes (see tree_kit.h).
#include "render/trees/tree_kit.h"

#include "core/color.h"
#include "core/random.h"
#include "render/props/prop_kit.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <array>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float GOLDEN = 1.61803398875f;
constexpr int ICO_VERTS = 12, ICO_FACES = 20;
enum IcoVertex : int { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11 };
constexpr int ICO[ICO_FACES][3] = { { V0, V11, V5 }, { V0, V5, V1 }, { V0, V1, V7 }, { V0, V7, V10 }, { V0, V10, V11 }, { V1, V5, V9 }, { V5, V11, V4 },
	{ V11, V10, V2 }, { V10, V7, V6 }, { V7, V1, V8 }, { V3, V9, V4 }, { V3, V4, V2 }, { V3, V2, V6 }, { V3, V6, V8 }, { V3, V8, V9 }, { V4, V9, V5 },
	{ V2, V4, V11 }, { V6, V2, V10 }, { V8, V6, V7 }, { V9, V8, V1 } };
constexpr float UNLIT_TONE = 0.42f, LIT_UP = 0.45f, NOISE_FREQ = 1.1f;
constexpr int LOD_SUBDIV[TREE_LODS] = { 1, 0 };
constexpr int LOD_TRUNK_SIDES[TREE_LODS] = { 6, 4 };

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

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

Color hexc(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

struct Tones {
	Color dark, lit;
	float jitter = 0, shade_bottom = 0;
};

// One crown mass: displaced icosphere, flat shaded, darker toward its bottom.
void crown(MeshBuilder &mb, const Dictionary &c, const Tones &tn, int subdiv) {
	Vector3 centre = vec3(c["center"]), rad = vec3(c["radius"]);
	float rough = float(c["rough"]);
	uint32_t seed = uint32_t(int(c["seed"]));
	auto displace = [&](const Vector3 &d) {
		float k = 1.f + vnoise(d.x * NOISE_FREQ + d.y, d.z * NOISE_FREQ - d.y, seed) * rough;
		return centre + Vector3(d.x * rad.x, d.y * rad.y, d.z * rad.z) * k;
	};
	const Vector2 t2(float(int(PropTag::TINT)), 0);
	uint32_t n = 0;
	for (const Tri &t : icosphere(subdiv)) {
		Vector3 p[3] = { displace(t[0]), displace(t[1]), displace(t[2]) };
		Vector3 nn = (p[1] - p[0]).cross(p[2] - p[0]).normalized();
		Vector3 mid = (p[0] + p[1] + p[2]) / 3.f;
		if (nn.dot(mid - centre) < 0) nn = -nn;
		float k = clampf(UNLIT_TONE + LIT_UP * nn.y + (hash_float(hash_combine(seed, n++)) - 0.5f) * tn.jitter, 0.f, 1.f);
		float h = clampf((mid.y - (centre.y - rad.y)) / (2.f * rad.y), 0.f, 1.f);
		Color col = tn.dark.lerp(tn.lit, k) * (tn.shade_bottom + (1.f - tn.shade_bottom) * h);
		col.a = 1;
		int a = mb.add(p[0], nn, col, Vector2(), t2), b = mb.add(p[1], nn, col, Vector2(), t2), cc = mb.add(p[2], nn, col, Vector2(), t2);
		mb.tri(a, b, cc, nn);
	}
}

TreeSpecies make_species(const Dictionary &d, PackedStringArray &errors) {
	TreeSpecies sp;
	if (!d.has("trunk") || !d.has("crowns")) {
		errors.push_back("streets.json trees: espécie sem trunk/crowns");
		return sp;
	}
	Dictionary trunk = d["trunk"];
	Array crowns = d["crowns"];
	Tones tn{ hexc(d["dark"]), hexc(d["lit"]), float(d["jitter"]), float(d["shade_bottom"]) };
	sp.crown_r = float(d["crown_reach_m"]);
	for (int lod = 0; lod < TREE_LODS; lod++) {
		MeshBuilder mb;
		mb.cylinder(Vector3(), float(trunk["radius"]), float(trunk["height"]), LOD_TRUNK_SIDES[lod], hexc(trunk["color"]), false,
				Vector2(float(int(PropTag::FIXED)), 0));
		for (int i = 0; i < crowns.size(); i++) crown(mb, crowns[i], tn, LOD_SUBDIV[lod]);
		sp.mesh[lod] = mb.commit();
	}
	return sp;
}
} // namespace

bool TreeKit::build(const Dictionary &trees, PackedStringArray &errors) {
	species_.clear();
	Array list = trees.get("species", Array());
	if (list.is_empty()) errors.push_back("streets.json trees: sem espécies");
	for (int i = 0; i < list.size(); i++) species_.push_back(make_species(list[i], errors));
	return errors.is_empty();
}

} // namespace city
