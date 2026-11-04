// Builds the tree species (see tree_kit.h) from streets.json "trees": the leaf atlas once, then for every species its
// body (trunk tube in rings that follow its lean, limbs, crown hearts), its cards (leaf cards or palm fronds) and the
// mean crown tone its far card is tinted with; one far card shared by every species.
#include "render/trees/tree_kit.h"

#include "core/color.h"
#include "core/random.h"
#include "render/trees/tree_parts.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF_F = 0.5f;
// The far card stands at this share of the tree's height and reaches half a unit either way across (its instance
// basis scales it to the tree's width) and `FAR_HALF_TALL` of the height up and down.
constexpr float FAR_CENTRE = 0.6f, FAR_HALF_TALL = 0.32f;

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}
float at(const Variant &v, int i) {
	Array a = v;
	return a.size() > i ? float(a[i]) : 0.f;
}
Color hexc(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

bool need(const Dictionary &d, const char *const *keys, int n, const String &where, PackedStringArray &errors) {
	for (int i = 0; i < n; i++)
		if (!d.has(keys[i])) errors.push_back("streets.json trees " + where + ": falta " + keys[i]);
	return errors.is_empty();
}

FrondSpec fronds_of(const Dictionary &f) {
	FrondSpec s;
	s.count = f["count"]; s.young = f["young"]; s.steps = f["segments"];
	s.len0 = at(f["length"], 0); s.len1 = at(f["length"], 1); s.rise0 = at(f["rise"], 0); s.rise1 = at(f["rise"], 1);
	s.droop0 = at(f["droop"], 0); s.droop1 = at(f["droop"], 1);
	s.young_len = f["young_length"]; s.young_rise = f["young_rise"]; s.young_droop = f["young_droop"]; s.width = f["width"]; s.fold = f["fold"];
	return s;
}

// The trunk from the ground to `top`, in `rings` tubes along a curve that bends out by `lean` (quadratic in height).
Vector3 trunk(MeshBuilder &mb, const Dictionary &t, Rng &r) {
	float top = t["top"], r0 = t["r0"], r1 = t["r1"], lean = t["lean"];
	int sides = t["facets"], rings = t["rings"];
	float turn = r.f() * TAU_F;
	Vector3 dir(std::cos(turn), 0, std::sin(turn));
	auto at_h = [&](float k) { return Vector3(0, top * k, 0) + dir * (lean * k * k); };
	Color bark = hexc(t["color"]);
	for (int i = 0; i < rings; i++) {
		float k0 = float(i) / rings, k1 = float(i + 1) / rings;
		tree_tube(mb, at_h(k0), at_h(k1), r0 + (r1 - r0) * k0, r0 + (r1 - r0) * k1, sides, bark);
	}
	return at_h(1.f);
}

void limbs(MeshBuilder &mb, const Dictionary &l, const Vector3 &from, const Color &bark, int sides, Rng &r) {
	int count = l["count"];
	float reach = l["reach"], rise = l["rise"], rr = l["r"];
	for (int i = 0; i < count; i++) {
		float a = TAU_F * (i + r.f() * HALF_F) / count;
		Vector3 to = from + Vector3(std::cos(a) * reach, rise, std::sin(a) * reach);
		tree_tube(mb, from, to, rr, rr * HALF_F, sides, bark);
	}
}

TreeSpecies make_species(const Dictionary &d, PackedStringArray &errors) {
	TreeSpecies sp;
	static const char *const KEYS[] = { "name", "seed", "trunk", "limbs", "crowns", "dark", "lit", "jitter", "shade_bottom", "heart_shade",
		"heart_shrink", "crown_reach", "far_width", "sway", "flutter", "cards_cast" };
	if (!need(d, KEYS, int(sizeof(KEYS) / sizeof(KEYS[0])), String(d.get("name", "?")), errors)) return sp;
	sp.name = d["name"];
	Rng r{ uint32_t(int(d["seed"])) };
	Shade sh{ Tones{ hexc(d["dark"]), hexc(d["lit"]) }, float(d["jitter"]), float(d["shade_bottom"]), float(d["heart_shade"]),
		float(d["heart_shrink"]), BloomSpec{} };
	if (d.has("bloom")) {
		Dictionary b = d["bloom"];
		sh.bloom = BloomSpec{ true, Tones{ hexc(b["dark"]), hexc(b["lit"]) }, float(b["above"]) };
	}
	MeshBuilder body, cards;
	Dictionary t = d["trunk"];
	Vector3 top = trunk(body, t, r);
	limbs(body, d["limbs"], top, hexc(t["color"]), int(t["facets"]), r);
	Array crowns = d["crowns"];
	Color sum;
	for (int i = 0; i < crowns.size(); i++) {
		Dictionary c = crowns[i];
		CrownSpec cs{ vec3(c["center"]) + Vector3(top.x, 0, top.z), vec3(c["radius"]), float(c["rough"]), uint32_t(int(c["seed"])), int(c["subdiv"]),
			int(c["cards"]), float(c["card"]), float(c["taper"]) };
		sum += tree_crown(body, cards, cs, sh, r);
	}
	if (d.has("fronds")) tree_fronds(cards, top, fronds_of(d["fronds"]), sh.leaf, r);
	sp.far_tone = crowns.is_empty() ? sh.leaf.tone(HALF_F) : sum / float(crowns.size());
	sp.far_tone.a = 1;
	sp.body = body.commit();
	if (!cards.empty()) sp.cards = cards.commit();
	sp.crown_reach = d["crown_reach"]; sp.far_width = d["far_width"];
	sp.sway = d["sway"]; sp.flutter = d["flutter"]; sp.cards_cast = d["cards_cast"];
	return sp;
}

Ref<ArrayMesh> far_card_mesh() {
	MeshBuilder mb;
	const Vector2 corner[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
	const Vector2 tag(float(int(TreePart::FAR)), FAR_HALF_TALL);
	for (const Vector2 &c : corner) mb.add(Vector3(0, FAR_CENTRE, 0), Vector3(0, 1, 0), Color(1, 1, 1, 0), c, tag);
	for (int k : { 0, 1, 2, 0, 2, 3 }) mb.idx.push_back(k);
	return mb.commit();
}
} // namespace

bool TreeKit::build(const Dictionary &trees, PackedStringArray &errors) {
	species_.clear();
	if (!trees.has("atlas") || !atlas_.build(trees["atlas"], errors)) {
		errors.push_back("streets.json trees: atlas das folhas inválido");
		return false;
	}
	Array list = trees.get("species", Array());
	if (list.is_empty()) errors.push_back("streets.json trees: sem espécies");
	for (int i = 0; i < list.size(); i++) species_.push_back(make_species(list[i], errors));
	far_card_ = far_card_mesh();
	return errors.is_empty();
}

int TreeKit::find(const String &name) const {
	for (int i = 0; i < count(); i++)
		if (species_[size_t(i)].name == name) return i;
	return -1;
}

} // namespace city
