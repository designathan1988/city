// The emitters a tree species is built from (render/trees/tree_kit.cpp), in model units (the tree one unit tall): tubes
// for the trunk and the limbs, a displaced icosphere for each crown heart, leaf cards on the crown's surface and palm
// frond strips. The vertex layout is tree_kit.h's.
#pragma once

#include "render/mesh_builder.h"
#include "render/trees/tree_part.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>

namespace city {

struct Rng;

struct Tones {
	godot::Color dark, lit;
	// Tones are stored in sRGB, as the vertex colour is: mixed and dimmed in linear, where light adds up.
	godot::Color tone(float k) const { return dark.srgb_to_linear().lerp(lit.srgb_to_linear(), k).linear_to_srgb(); }
};

// `c` (sRGB) with `k` of its light.
godot::Color dimmed(const godot::Color &c, float k);

struct CrownSpec {
	godot::Vector3 centre, radius;
	float rough = 0;
	uint32_t seed = 0;
	int subdiv = 1, cards = 0;
	float card = 0, taper = 0; // card size over the tree's height; how much smaller the cards grow towards the top
};

// A conifer's crown: a thin core and cards through a cone round it, narrower and smaller towards the tip.
struct ConeSpec {
	float base = 0, height = 0, core = 0, crown = 0, card = 0, taper = 0;
	int cards = 0;
};

struct BloomSpec {
	bool on = false;
	Tones tones;
	float above = 0; // faces and cards higher than this (model units) take the bloom's tones
};

// Palm fronds: each leaves the crown `lift` radians above level and bends over by `bend` along its length, in `steps` straight
// runs; a strip `width` of its length wide either side of the rib, whose sides fold down by `fold` of that (young fronds, the
// last `young`, stand up out of the middle).
struct FrondSpec {
	int count = 0, young = 0, steps = 1;
	float len0 = 0, len1 = 0, lift0 = 0, lift1 = 0, bend0 = 0, bend1 = 0, spin = 0;
	float young_len0 = 0, young_len1 = 0, young_lift0 = 0, young_lift1 = 0, young_bend = 0, young_turn = 1, width = 0, fold = 0, young_fold = 0;
};

// The shading every face and card of a species shares.
struct Shade {
	Tones leaf;
	float jitter = 0, shade_bottom = 1, heart_shade = 1, heart_shrink = 1;
	BloomSpec bloom;
};

// A tube from `a` to `b`, radii r0 at a and r1 at b, `sides` flat faces between `tones` by how much each faces the sky.
void tree_tube(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, float r0, float r1, int sides, const Tones &tones, TreePart part);
// One crown heart (flat faces, darker low in the crown and, under cards, shrunk and shaded) and its cards on the full
// surface; returns the crown's mean tone for the far card.
godot::Color tree_crown(MeshBuilder &body, MeshBuilder &cards, const CrownSpec &c, const Shade &sh, Rng &r);
// A conifer's crown; returns its mean tone for the far card.
godot::Color tree_cone(MeshBuilder &body, MeshBuilder &cards, const ConeSpec &c, const Shade &sh, Rng &r);
// The fronds of a palm whose crown sits at `top`.
void tree_fronds(MeshBuilder &cards, const godot::Vector3 &top, const FrondSpec &f, const Tones &tones, Rng &r);

} // namespace city
