// Builds the tree species (see tree_kit.h) from streets.json "trees": the leaf atlas once, then for every species its
// body (trunk tube in rings that follow its lean, limbs, crown hearts), its cards (leaf cards or palm fronds) and the
// mean crown tone its far card is tinted with. A species is read whole through the strict reader (render/data_reader.h) before
// anything of it is built: a bad row is an error, never a tree drawn from zeros.
#include "render/trees/tree_kit.h"

#include "render/agents/agent_lod.h"

#include "core/color.h"
#include "core/random.h"
#include "render/data_reader.h"
#include "render/trees/tree_parts.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float HALF_F = 0.5f;
// A species on a trunk this short is a bush: its flowers are a scatter over its sunlit side, not its whole top (the 4.0, lowPolyTrees.ts bush).
constexpr float BUSH_TRUNK_TOP = 0.2f, BUSH_BLOOM_SCATTER = 0.3f;
constexpr int XYZ = 3, RANGE = 2; // [x, y, z]; [low, high]
constexpr float RING_LAP = 0.02f; // a ring runs this far into the next, of a ring, so no seam opens at a bend

float num(DataReader &r, const Dictionary &d, const char *key) { return float(r.num(d, key)); }
Color colour(DataReader &r, const Dictionary &d, const char *key) { return rgba8_to_color(r.colour(d, key)); }
Tones tones(DataReader &r, const Dictionary &d) { return Tones{ colour(r, d, "dark"), colour(r, d, "lit") }; }

Vector3 vec3(DataReader &r, const Dictionary &d, const char *key) {
	const Array a = r.list(d, key, XYZ);
	DataReader rv = r.in(key);
	return Vector3(float(rv.num_at(a, 0)), float(rv.num_at(a, 1)), float(rv.num_at(a, 2)));
}

void range(DataReader &r, const Dictionary &d, const char *key, float &low, float &high) {
	const Array a = r.list(d, key, RANGE);
	low = float(r.in(key).num_at(a, 0));
	high = float(r.in(key).num_at(a, 1));
}

struct TrunkSpec {
	float top = 0, r0 = 0, r1 = 0, lean = 0, flare = 0, flare_h = 0;
	int sides = 0, rings = 0;
	Tones bark;
	bool smooth = false; // a palm's stem has no furrows
};

struct LimbSpec {
	int count = 0;
	float reach = 0, rise = 0, r = 0;
};

struct CrownRow {
	CrownSpec spec; // its centre from the trunk's foot; the trunk's top is added when it is built
	bool own_tones = false; // a palm's crownshaft and nuts are not leaves: their own tones
	Tones tones;
};

struct SpeciesSpec {
	String name;
	uint32_t seed = 0;
	Shade shade;
	TrunkSpec trunk;
	LimbSpec limbs;
	std::vector<CrownRow> crowns;
	bool has_fronds = false, has_cone = false;
	FrondSpec fronds;
	ConeSpec cone;
	float crown_reach = 0;
	bool cards_cast = false;
};

TrunkSpec trunk_of(DataReader r, const Dictionary &t) {
	TrunkSpec s;
	s.top = num(r, t, "top");
	s.r0 = num(r, t, "r0");
	s.r1 = num(r, t, "r1");
	s.lean = num(r, t, "lean");
	s.flare = num(r, t, "flare");
	s.flare_h = num(r, t, "flare_h");
	s.sides = r.integer(t, "facets");
	s.rings = r.integer(t, "rings");
	s.bark = tones(r, t);
	s.smooth = r.flag_or(t, "smooth", false);
	return s;
}

LimbSpec limbs_of(DataReader r, const Dictionary &l) { return LimbSpec{ r.integer(l, "count"), num(r, l, "reach"), num(r, l, "rise"), num(r, l, "r") }; }

CrownRow crown_of(DataReader r, const Dictionary &c) {
	CrownRow row;
	row.spec = CrownSpec{ vec3(r, c, "center"), vec3(r, c, "radius"), num(r, c, "rough"), uint32_t(r.integer(c, "seed")), r.integer(c, "subdiv"),
		r.integer(c, "cards"), num(r, c, "card"), num(r, c, "taper") };
	row.own_tones = c.has("dark");
	if (row.own_tones) row.tones = tones(r, c);
	return row;
}

FrondSpec fronds_of(DataReader r, const Dictionary &f) {
	FrondSpec s;
	s.count = r.integer(f, "count");
	s.young = r.integer(f, "young");
	s.steps = r.integer(f, "segments");
	range(r, f, "length", s.len0, s.len1);
	range(r, f, "lift", s.lift0, s.lift1);
	range(r, f, "droop_rad", s.bend0, s.bend1);
	s.spin = num(r, f, "spin");
	range(r, f, "young_length", s.young_len0, s.young_len1);
	range(r, f, "young_lift", s.young_lift0, s.young_lift1);
	s.young_bend = num(r, f, "young_droop_rad");
	s.young_turn = num(r, f, "young_turn");
	s.width = num(r, f, "width");
	s.fold = num(r, f, "fold");
	s.young_fold = num(r, f, "young_fold");
	return s;
}

ConeSpec cone_of(DataReader r, const Dictionary &c) {
	ConeSpec s;
	s.base = num(r, c, "base");
	s.height = num(r, c, "height");
	s.core = num(r, c, "core");
	s.crown = num(r, c, "crown");
	s.card = num(r, c, "card");
	s.taper = num(r, c, "taper");
	s.cards = r.integer(c, "cards");
	return s;
}

SpeciesSpec species_of(DataReader r, const Dictionary &d) {
	SpeciesSpec s;
	s.name = r.text(d, "name");
	s.seed = uint32_t(r.integer(d, "seed"));
	s.shade = Shade{ tones(r, d), num(r, d, "jitter"), num(r, d, "shade_bottom"), num(r, d, "heart_shade"), num(r, d, "heart_shrink"), BloomSpec{} };
	s.trunk = trunk_of(r.in("trunk"), r.table(d, "trunk"));
	if (d.has("bloom")) {
		DataReader rb = r.in("bloom");
		const Dictionary b = r.table(d, "bloom");
		s.shade.bloom = BloomSpec{ true, tones(rb, b), num(rb, b, "above"), s.trunk.top < BUSH_TRUNK_TOP ? BUSH_BLOOM_SCATTER : 1.f };
	}
	s.limbs = limbs_of(r.in("limbs"), r.table(d, "limbs"));
	const Array crowns = r.list(d, "crowns");
	for (int i = 0; i < crowns.size(); i++) s.crowns.push_back(crown_of(r.in("crowns").in(i), r.in("crowns").table_at(crowns, i)));
	s.has_fronds = d.has("fronds");
	if (s.has_fronds) s.fronds = fronds_of(r.in("fronds"), r.table(d, "fronds"));
	s.has_cone = d.has("cone");
	if (s.has_cone) s.cone = cone_of(r.in("cone"), r.table(d, "cone"));
	s.crown_reach = num(r, d, "crown_reach");
	s.cards_cast = r.flag(d, "cards_cast");
	return s;
}

// The trunk from the ground to `top`, in `rings` tubes along a curve that bends out by `lean` (quadratic in height),
// its foot spreading into the roots by `flare` of its radius, gone within `flare_h` of the trunk.
Vector3 trunk(MeshBuilder &mb, const TrunkSpec &t, Rng &r) {
	float turn = r.f() * TAU_F;
	Vector3 dir(std::cos(turn), 0, std::sin(turn));
	auto at_h = [&](float k) { return Vector3(0, t.top * k, 0) + dir * (t.lean * k * k); };
	auto radius = [&](float k) { return t.r0 + (t.r1 - t.r0) * k + t.r0 * t.flare * std::exp(-k / t.flare_h); };
	const TreePart skin = t.smooth ? TreePart::BODY : TreePart::BARK;
	for (int i = 0; i < t.rings; i++) {
		float k0 = float(i) / t.rings, k1 = (float(i + 1) + RING_LAP) / t.rings;
		tree_tube(mb, at_h(k0), at_h(k1), radius(k0), radius(k1), t.sides, t.bark, skin);
	}
	return at_h(1.f);
}

void limbs(MeshBuilder &mb, const LimbSpec &l, const Vector3 &from, const Tones &bark, int sides, Rng &r) {
	for (int i = 0; i < l.count; i++) {
		float a = TAU_F * (i + r.f() * HALF_F) / l.count;
		Vector3 to = from + Vector3(std::cos(a) * l.reach, l.rise, std::sin(a) * l.reach);
		tree_tube(mb, from, to, l.r, l.r * HALF_F, sides, bark, TreePart::BARK);
	}
}

TreeSpecies make_species(const SpeciesSpec &s, int row, float caster_error_m, TreePictures &pictures, const RID &body_material, const RID &card_material) {
	TreeSpecies sp;
	sp.name = s.name;
	Rng r{ s.seed };
	MeshBuilder body, cards;
	const Vector3 top = trunk(body, s.trunk, r);
	limbs(body, s.limbs, top, s.trunk.bark, s.trunk.sides, r);
	Color sum;
	for (const CrownRow &c : s.crowns) {
		CrownSpec cs = c.spec;
		cs.centre += Vector3(top.x, 0, top.z);
		Shade own = s.shade;
		if (c.own_tones) own.leaf = c.tones;
		sum += tree_crown(body, cards, cs, own, r);
	}
	if (s.has_fronds) tree_fronds(cards, top, s.fronds, s.shade.leaf, r);
	sp.far_tone = s.crowns.empty() || s.has_fronds ? s.shade.leaf.tone(HALF_F) : sum / float(s.crowns.size());
	if (s.has_cone) sp.far_tone = tree_cone(body, cards, s.cone, s.shade, r);
	sp.far_tone.a = 1;
	pictures.draw(row, body, cards, sp.far_tone);
	sp.body = body.bake(body_material, false);
	sp.caster = simplify_whole(body, caster_error_m).bake(body_material, false);
	if (!cards.empty()) sp.cards = cards.bake(card_material, false);
	sp.crown_reach = s.crown_reach;
	sp.cards_cast = s.cards_cast;
	return sp;
}
} // namespace

bool TreeKit::build(const Dictionary &trees, const RID &body_material, const RID &card_material, PackedStringArray &errors) {
	species_.clear();
	const int64_t before = errors.size();
	DataReader r{ errors, "streets.json.trees" };
	if (!trees.has("atlas") || !atlas_.build(r.table(trees, "atlas"), errors)) {
		r.in("atlas").fail("the leaf atlas is not valid");
		return false;
	}
	const Array list = r.list(trees, "species");
	if (list.is_empty()) r.in("species").fail("no species");
	const float caster_error_m = num(r, trees, "caster_error_m");
	std::vector<SpeciesSpec> specs;
	for (int i = 0; i < list.size(); i++) specs.push_back(species_of(r.in("species").in(i), r.in("species").table_at(list, i)));
	if (errors.size() != before) return false; // nothing is built from a table that did not read
	if (!pictures_.begin(r.table(trees, "pictures"), int(list.size()), atlas_.take_image(), errors)) return false;
	for (int i = 0; i < int(specs.size()); i++) species_.push_back(make_species(specs[size_t(i)], i, caster_error_m, pictures_, body_material, card_material));
	pictures_.finish();
	return errors.size() == before;
}

int TreeKit::find(const String &name) const {
	for (int i = 0; i < count(); i++)
		if (species_[size_t(i)].name == name) return i;
	return -1;
}

} // namespace city
