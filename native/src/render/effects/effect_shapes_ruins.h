// The effects' shapes of a building brought down: its heap, its pieces falling, their fall, the mounds (effects.json shapes, read by effect_params.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define EFFECT_SHAPE_EFFECT_RUINS(X, XI, XD, XV, XVI) \
	X(pieces_per_m2) X(pieces_per_storey) X(seed_span) XI(heap_fewest) XI(heap_most) X(storeys_per_m) \
	X(slab_min_m) X(piece_span_m) X(piece_flat_min) X(piece_flat_span) X(heap_height_k) X(heap_min_m) \
	X(heap_max_m) X(fall_out_m) X(fall_down_m)
struct RuinsShape {
	SHAPE_DECLARE(EFFECT_SHAPE_EFFECT_RUINS)
};

#define EFFECT_SHAPE_RUIN_PIECES(X, XI, XD, XV, XVI) \
	XI(most_falls) X(box_spread) XI(steps_most) XI(dust_a_frame) XI(dust_count) X(dust_chance) X(dust_size_m) \
	X(dust_life_s) X(dust_spread) X(land_radius_m) X(land_chance) X(land_life_s) X(land_spread_m) X(lot_pad_m)
struct PiecesShape {
	SHAPE_DECLARE(EFFECT_SHAPE_RUIN_PIECES)
};

#define EFFECT_SHAPE_RUIN_SIM(X, XI, XD, XV, XVI) \
	X(resting_m) X(throw_base_ms) X(throw_per_force_ms) X(throw_share_least) X(break_power_ms) X(throw_least) \
	X(throw_span) X(throw_up) X(turn_range) X(spin_base) X(spin_per_power) X(hop_s) X(hop_least) X(hop_span) \
	X(support_every_s) X(mound_fill) X(longest_fall_s) X(land_speed_ms) X(pile_cell_m) X(pile_over) \
	XI(pile_most_side) XI(settle_most_steps)
struct RuinSimShape {
	SHAPE_DECLARE(EFFECT_SHAPE_RUIN_SIM)
};

#define EFFECT_SHAPE_MOUNDS(X, XI, XD, XV, XVI) \
	XI(relief_cells) X(bell_power) X(bell_shoulder) X(lump_share) X(lump_wide) X(lump_mid) X(lump_fine) \
	X(coarse_share) X(mid_share) X(fine_share)
struct MoundsShape {
	SHAPE_DECLARE(EFFECT_SHAPE_MOUNDS)
};

} // namespace city
