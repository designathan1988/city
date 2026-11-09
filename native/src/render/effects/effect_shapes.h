// The effects' shapes: every value an effects file draws with, one block per file, filled from data/render/effects.json "shapes" by effect_params.cpp (a missing key refuses the table). The blocks are declared by what they play (effect_shapes_blow.h, effect_shapes_ruins.h, effect_shapes_world.h); this file lists them and makes the EffectShapes the EffectParams hold.
#pragma once

#include "render/effects/effect_shapes_blow.h"
#include "render/effects/effect_shapes_ruins.h"
#include "render/effects/effect_shapes_world.h"

namespace city {

// Every block: its key in the table, its type and its list.
#define EFFECT_SHAPE_BLOCKS(S) \
	S(effect_blast, BlastShape, EFFECT_SHAPE_EFFECT_BLAST) \
	S(crater_slabs, CraterSlabsShape, EFFECT_SHAPE_CRATER_SLABS) \
	S(gore, GoreShape, EFFECT_SHAPE_GORE) \
	S(wires, WiresShape, EFFECT_SHAPE_WIRES) \
	S(effect_bodies, BodiesShape, EFFECT_SHAPE_EFFECT_BODIES) \
	S(effect_things, ThingsShape, EFFECT_SHAPE_EFFECT_THINGS) \
	S(effect_ruins, RuinsShape, EFFECT_SHAPE_EFFECT_RUINS) \
	S(ruin_pieces, PiecesShape, EFFECT_SHAPE_RUIN_PIECES) \
	S(ruin_sim, RuinSimShape, EFFECT_SHAPE_RUIN_SIM) \
	S(mounds, MoundsShape, EFFECT_SHAPE_MOUNDS) \
	S(effect_world, WorldShape, EFFECT_SHAPE_EFFECT_WORLD) \
	S(puffs, PuffsShape, EFFECT_SHAPE_PUFFS) \
	S(debris, DebrisShape, EFFECT_SHAPE_DEBRIS)

struct EffectShapes {
#define EFFECT_SHAPE_MEMBER(key, Type, LIST) Type key;
	EFFECT_SHAPE_BLOCKS(EFFECT_SHAPE_MEMBER)
#undef EFFECT_SHAPE_MEMBER
};

} // namespace city
