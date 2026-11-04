// The emitters a tree species is built from (render/trees/tree_kit.cpp), in model units (the tree one unit tall): tubes
// for the trunk and the limbs, a displaced icosphere for each crown heart, leaf cards on the crown's surface and palm
// frond strips. The vertex layout is tree_kit.h's.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>

namespace city {

struct Rng;

struct Tones {
	godot::Color dark, lit;
	godot::Color tone(float k) const { return dark.lerp(lit, k); }
};

struct CrownSpec {
	godot::Vector3 centre, radius;
	float rough = 0;
	uint32_t seed = 0;
	int subdiv = 1, cards = 0;
	float card = 0, taper = 0; // card size over the tree's height; how much smaller the cards grow towards the top
};

struct BloomSpec {
	bool on = false;
	Tones tones;
	float above = 0; // faces and cards higher than this (model units) take the bloom's tones
};

// Palm fronds: each a quadratic Bezier from the crown out to its tip, arched over a control point `rise` of its length
// above the crown and ending `droop` of its length below it (young fronds stand up), drawn as a strip `width` wide
// whose sides fold down by `fold` of the width.
struct FrondSpec {
	int count = 0, young = 0, steps = 1;
	float len0 = 0, len1 = 0, rise0 = 0, rise1 = 0, droop0 = 0, droop1 = 0;
	float young_len = 0, young_rise = 0, young_droop = 0, width = 0, fold = 0;
};

// The shading every face and card of a species shares.
struct Shade {
	Tones leaf;
	float jitter = 0, shade_bottom = 1, heart_shade = 1, heart_shrink = 1;
	BloomSpec bloom;
};

// A tube from `a` to `b`, radii r0 at a and r1 at b, `sides` flat faces in `col` lit by how much each faces the sky.
void tree_tube(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, float r0, float r1, int sides, const godot::Color &col);
// One crown heart (flat faces, darker low in the crown and, under cards, shrunk and shaded) and its cards on the full
// surface; returns the crown's mean tone for the far card.
godot::Color tree_crown(MeshBuilder &body, MeshBuilder &cards, const CrownSpec &c, const Shade &sh, Rng &r);
// The fronds of a palm whose crown sits at `top`.
void tree_fronds(MeshBuilder &cards, const godot::Vector3 &top, const FrondSpec &f, const Tones &tones, Rng &r);

} // namespace city
