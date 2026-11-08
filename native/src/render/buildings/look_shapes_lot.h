// The building render's shapes of the lots: the ground and the boundary, the props, the small parts (building_look.json shapes, read by building_look.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city::b40 {

#define LOOK_SHAPE_LOT_EMIT(X, XI, XD, XV, XVI) \
	X(ground_cell_m) X(level_m) X(lot_raise) X(foot) X(piece) X(coping_h) X(coping_out) X(coping_shade) X(splash) \
	X(splash_shade) X(rail_t) X(rail_low) X(low_rail_h) X(post) X(post_every) X(post_over) X(light_post) \
	X(light_every) X(bar_every) X(gate_post) X(gate_rail) X(lamp_head) X(lamp_head_h) X(lamp_post) \
	X(bollard_band) X(bollard_band_h) X(bollard_band_out) X(soil_in) X(bench_seat) X(bench_seat_t) X(bench_back) \
	X(bench_back_t) X(fascia_shade) X(fascia_h) X(awn_shade) X(grille_t)
struct LotEmitLook {
	SHAPE_DECLARE(LOOK_SHAPE_LOT_EMIT)
};

#define LOOK_SHAPE_LOT_PROPS(X, XI, XD, XV, XVI) \
	X(line_lift) X(bumper) X(bumper_t) X(bumper_lo) X(bumper_hi) X(bumper_in) X(shade_span) X(bed_kerb_h) \
	X(bed_kerb_w) X(bed_soil_h) X(water_lift) X(coping_w) X(coping_over) X(lounger_seat) X(lounger_t) \
	X(lounger_head) X(lounger_back) X(table_top) X(leg) X(swing_post) X(swing_seat_z) X(swing_seat) \
	X(swing_seat_d) X(swing_seat_t) X(rope) X(rope_x) X(shed_roof) X(shed_eave) X(edicula_area) X(door_w) \
	X(door_h) X(door_out) X(line_post) X(wire) X(wire_down) X(cloth) X(cloth_h) X(cloth_at) X(pallet_t) \
	X(cage_post) X(cage_rail) X(cage_bar) X(cage_bar_every) X(cylinder) X(cylinder_h) X(cylinder_every)
struct LotPropsLook {
	SHAPE_DECLARE(LOOK_SHAPE_LOT_PROPS)
};

#define LOOK_SHAPE_LOT_PARTS(X, XI, XD, XV, XVI) \
	X(rock_shade) X(dial_out) X(dial_margin) X(hand_w) X(line_w) X(line_up) X(long_hand) X(short_hand)
struct LotPartsLook {
	SHAPE_DECLARE(LOOK_SHAPE_LOT_PARTS)
};

} // namespace city::b40
