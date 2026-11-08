// The effects' shapes of the owner, the puffs and the thrown pieces (effects.json shapes, read by effect_params.cpp).
#pragma once

#include "render/effects/effect_shape_fields.h"

namespace city {

#define EFFECT_SHAPE_EFFECT_WORLD(X, XI, XD, XV, XVI) \
	X(max_frame_s) X(mark_slots) XI(mound_slots) X(roof_flame_base_m) X(roof_flame_span_m) X(flame_span_share) \
	X(roof_flame_cap_m) X(shake_floor) X(flash_damp) X(light_atten) X(geyser_lift_m) X(jet_spread_m) \
	X(jet_life_s)
struct WorldShape {
	EFFECT_SHAPE_EFFECT_WORLD(EFFECT_SHAPE_FLOAT, EFFECT_SHAPE_COUNT, EFFECT_SHAPE_DOUBLE, EFFECT_SHAPE_FLOATS, EFFECT_SHAPE_COUNTS)
};

#define EFFECT_SHAPE_PUFFS(X, XI, XD, XV, XVI) \
	X(size_min) X(size_span) X(life_low) X(life_span) X(spread_flatten) X(spread_floor)
struct PuffsShape {
	EFFECT_SHAPE_PUFFS(EFFECT_SHAPE_FLOAT, EFFECT_SHAPE_COUNT, EFFECT_SHAPE_DOUBLE, EFFECT_SHAPE_FLOATS, EFFECT_SHAPE_COUNTS)
};

#define EFFECT_SHAPE_DEBRIS(X, XI, XD, XV, XVI) \
	X(random_spin_range) X(tint_min) X(tint_span) X(flame_over_m) X(smoke_over_m) X(smoke_spread_m) \
	X(flame_follow_share) XI(wrecks_kept)
struct DebrisShape {
	EFFECT_SHAPE_DEBRIS(EFFECT_SHAPE_FLOAT, EFFECT_SHAPE_COUNT, EFFECT_SHAPE_DOUBLE, EFFECT_SHAPE_FLOATS, EFFECT_SHAPE_COUNTS)
};

} // namespace city
