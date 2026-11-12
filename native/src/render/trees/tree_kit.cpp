// Builds the tree species (see tree_kit.h) from streets.json "trees": the leaf atlas once, then for every species its
// body (trunk tube in rings that follow its lean, limbs, crown hearts, a hedge's clipped block), its cards (leaf cards or palm
// fronds), the mean crown tone its far card is tinted with and the measure of what it draws.
#include "render/trees/tree_kit.h"

#include "render/agents/agent_lod.h"

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
// A species on a trunk this short is a bush: its flowers are a scatter over its sunlit side, not its whole top (the 4.0, lowPolyTrees.ts bush).
constexpr float BUSH_TRUNK_TOP = 0.2f, BUSH_BLOOM_SCATTER = 0.3f;

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

ConeSpec cone_of(const Dictionary &c) {
	ConeSpec s;
	s.base = c["base"]; s.height = c["height"]; s.core = c["core"]; s.crown = c["crown"];
	s.card = c["card"]; s.taper = c["taper"]; s.cards = c["cards"];
	return s;
}

FrondSpec fronds_of(const Dictionary &f) {
	FrondSpec s;
	s.count = f["count"]; s.young = f["young"]; s.steps = f["segments"];
	s.len0 = at(f["length"], 0); s.len1 = at(f["length"], 1); s.lift0 = at(f["lift"], 0); s.lift1 = at(f["lift"], 1);
	s.bend0 = at(f["droop_rad"], 0); s.bend1 = at(f["droop_rad"], 1); s.spin = f["spin"];
	s.young_len0 = at(f["young_length"], 0); s.young_len1 = at(f["young_length"], 1); s.young_lift0 = at(f["young_lift"], 0); s.young_lift1 = at(f["young_lift"], 1);
	s.young_bend = f["young_droop_rad"]; s.young_turn = f["young_turn"]; s.width = f["width"]; s.fold = f["fold"]; s.young_fold = f["young_fold"];
	return s;
}

ClippedBlock block_of(const Dictionary &b) {
	ClippedBlock s;
	s.size = vec3(b["size"]); s.bevel = b["bevel"]; s.rough = b["rough"]; s.seed = uint32_t(int(b["seed"]));
	s.segments = std::max(1, int(b["segments"])); s.cards = b["cards"]; s.card = b["card"];
	return s;
}

constexpr float RING_LAP = 0.02f; // a ring runs this far into the next, of a ring, so no seam opens at a bend

Tones bark_of(const Dictionary &t) { return Tones{ hexc(t["dark"]), hexc(t["lit"]) }; }

// The trunk from the ground to `top`, in `rings` tubes along a curve that bends out by `lean` (quadratic in height),
// its foot spreading into the roots by `flare` of its radius, gone within `flare_h` of the trunk.
Vector3 trunk(MeshBuilder &mb, const Dictionary &t, Rng &r) {
	float top = t["top"], r0 = t["r0"], r1 = t["r1"], lean = t["lean"], flare = t["flare"], flare_h = t["flare_h"];
	int sides = t["facets"], rings = t["rings"];
	float turn = r.f() * TAU_F;
	Vector3 dir(std::cos(turn), 0, std::sin(turn));
	auto at_h = [&](float k) { return Vector3(0, top * k, 0) + dir * (lean * k * k); };
	auto radius = [&](float k) { return r0 + (r1 - r0) * k + r0 * flare * std::exp(-k / flare_h); };
	Tones bark = bark_of(t);
	TreePart skin = bool(t.get("smooth", false)) ? TreePart::BODY : TreePart::BARK; // a palm's stem has no furrows
	for (int i = 0; i < rings; i++) {
		float k0 = float(i) / rings, k1 = (float(i + 1) + RING_LAP) / rings;
		tree_tube(mb, at_h(k0), at_h(k1), radius(k0), radius(k1), sides, bark, skin);
	}
	return at_h(1.f);
}

void limbs(MeshBuilder &mb, const Dictionary &l, const Vector3 &from, const Tones &bark, int sides, Rng &r) {
	int count = l["count"];
	float reach = l["reach"], rise = l["rise"], rr = l["r"];
	for (int i = 0; i < count; i++) {
		float a = TAU_F * (i + r.f() * HALF_F) / count;
		Vector3 to = from + Vector3(std::cos(a) * reach, rise, std::sin(a) * reach);
		tree_tube(mb, from, to, rr, rr * HALF_F, sides, bark, TreePart::BARK);
	}
}

// `tuft`: how far a card's leaves stand from its centre, of its side (the atlas'): what the species' measure takes a card for.
TreeSpecies make_species(const Dictionary &d, int row, float caster_error_m, float tuft, TreePictures &pictures, const RID &body_material,
		const RID &card_material, PackedStringArray &errors) {
	TreeSpecies sp;
	static const char *const KEYS[] = { "name", "seed", "trunk", "limbs", "crowns", "dark", "lit", "jitter", "shade_bottom", "heart_shade",
		"heart_share", "crown_reach", "cards_shadow" };
	if (!need(d, KEYS, int(sizeof(KEYS) / sizeof(KEYS[0])), String(d.get("name", "?")), errors)) return sp;
	sp.name = d["name"];
	Rng r{ uint32_t(int(d["seed"])) };
	Shade sh{ Tones{ hexc(d["dark"]), hexc(d["lit"]) }, float(d["jitter"]), float(d["shade_bottom"]), float(d["heart_shade"]),
		float(d["heart_share"]), BloomSpec{} };
	if (d.has("bloom")) {
		Dictionary b = d["bloom"];
		const bool bush = float(Dictionary(d["trunk"])["top"]) < BUSH_TRUNK_TOP;
		sh.bloom = BloomSpec{ true, Tones{ hexc(b["dark"]), hexc(b["lit"]) }, float(b["above"]), bush ? BUSH_BLOOM_SCATTER : 1.f };
	}
	MeshBuilder body, cards;
	Dictionary t = d["trunk"];
	Vector3 top = trunk(body, t, r);
	limbs(body, d["limbs"], top, bark_of(t), int(t["facets"]), r);
	Array crowns = d["crowns"];
	Color sum;
	for (int i = 0; i < crowns.size(); i++) {
		Dictionary c = crowns[i];
		CrownSpec cs{ vec3(c["center"]) + Vector3(top.x, 0, top.z), vec3(c["radius"]), float(c["rough"]), uint32_t(int(c["seed"])), int(c["subdiv"]),
			int(c["cards"]), float(c["card"]), float(c["taper"]) };
		Shade own = sh; // a palm's crownshaft and nuts are not leaves: their own tones
		if (c.has("dark")) own.leaf = Tones{ hexc(c["dark"]), hexc(c["lit"]) };
		sum += tree_crown(body, cards, cs, own, r);
	}
	if (d.has("fronds")) tree_fronds(cards, top, fronds_of(d["fronds"]), sh.leaf, r);
	sp.far_tone = crowns.is_empty() || d.has("fronds") ? sh.leaf.tone(HALF_F) : sum / float(crowns.size());
	if (d.has("cone")) sp.far_tone = tree_cone(body, cards, cone_of(d["cone"]), sh, r);
	if (d.has("block")) sp.far_tone = tree_block(body, cards, block_of(d["block"]), sh, r);
	sp.far_tone.a = 1;
	sp.drawn.measure(body, cards, tuft); // the very points the meshes below are baked from
	if (sp.drawn.empty()) errors.push_back("streets.json trees " + sp.name + ": the species draws nothing");
	pictures.draw(row, body, cards, sp.far_tone);
	sp.body = body.bake(body_material, false).shared(); // a unit model: its attributes as they are; shared by every chunk's draw
	sp.caster = simplify_whole(body, caster_error_m).bake(body_material, false).shared();
	if (!cards.empty()) sp.cards = cards.bake(card_material, false).shared();
	sp.crown_reach = d["crown_reach"];
	sp.cards_shadow = d["cards_shadow"];
	return sp;
}
} // namespace

bool TreeKit::build(const Dictionary &trees, const RID &body_material, const RID &card_material, PackedStringArray &errors) {
	species_.clear();
	if (!trees.has("atlas") || !atlas_.build(trees["atlas"], errors)) {
		errors.push_back("streets.json trees: atlas das folhas inválido");
		return false;
	}
	Array list = trees.get("species", Array());
	if (list.is_empty()) errors.push_back("streets.json trees: sem espécies");
	if (!pictures_.begin(trees.get("pictures", Dictionary()), int(list.size()), atlas_.take_image(), errors)) return false;
	for (int i = 0; i < list.size(); i++)
		species_.push_back(make_species(list[i], i, trees["caster_error_m"], atlas_.tuft_reach(), pictures_, body_material, card_material, errors));
	pictures_.finish();
	return errors.is_empty();
}

int TreeKit::find(const String &name) const {
	for (int i = 0; i < count(); i++)
		if (species_[size_t(i)].name == name) return i;
	return -1;
}

} // namespace city
