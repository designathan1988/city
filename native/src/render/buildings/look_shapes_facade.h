// The building render's shapes of the facades: the openings' pieces, the wall and its bands, the instanced parts' models, the mouldings (building_look.json shapes, read by building_look.cpp).
#pragma once

#include "render/buildings/look_shape_fields.h"

namespace city::b40 {

#define LOOK_SHAPE_FACADE_PARTS(X, XI, XD, XV, XVI) \
	X(frame_d) X(frame_thin) X(frame_in) X(bal_edge) X(bal_slab) X(bal_out) X(canopy_pad) X(canopy_t) \
	X(canopy_out) X(fascia_h) X(fascia_t) X(fascia_drop) X(canopy_rise) X(canopy_room) X(bracket_w) X(bracket_h) \
	X(bracket_d) X(bracket_off) X(leaf_max) X(leaf_gap) X(light_inset) X(light_share) X(light_at) X(fan_min) \
	X(fan_gap) X(transom_t) X(dd_pad) X(dd_t) X(dd_out) X(dd_fascia) X(dd_room) X(awning_trim) X(awning_h) \
	X(awning_up) X(loggia_pane) X(glass_rail_in) X(cap_h) X(cap_t) X(brise_out) X(surround) X(surround_t) \
	X(surround_out) X(louvre_t) X(louvre_out) X(louvre_spread) X(shut_sill_past) X(shut_sill_t) X(shut_sill_out) \
	X(g_slab_t) X(g_out) X(g_beam_h) X(g_beam_t) X(g_beam_drop) X(g_beam_out) X(g_shade) X(g_end) X(g_rail_out) \
	X(g_wall_t) X(g_wall_d) X(g_wall_drop) X(garden_wall_u) X(g_post_d) X(g_post_out) X(transom_min) \
	X(transom_at) X(window_sill_min) X(lamp_w) X(lamp_h) X(lamp_d) X(lamp_off) X(lamp_drop) X(lamp_out) \
	X(lamp_room) X(shutter_chroma) X(shutter_max) X(wash_half) X(wash_up) X(wash_foot) X(wash_out)
struct FacadePartsLook {
	LOOK_SHAPE_FACADE_PARTS(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_FACADE_EMIT(X, XI, XD, XV, XVI) \
	X(face_clear_in) X(out_test) X(pilaster_k) X(pier_max_share) X(pier_from) X(pier_to) X(ground_lift) \
	X(band_out) X(band_h) X(base_out) X(base_h) X(slab_out) X(slab_h) X(slab_shade) X(plinth_out) X(plinth_bury) \
	X(step_min) X(facade_run_min) X(step_front) X(step_pad) X(step_foot) X(door_w)
struct FacadeEmitLook {
	LOOK_SHAPE_FACADE_EMIT(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_PART_MODELS(X, XI, XD, XV, XVI) \
	X(leaf_z) X(leaf_d) X(panel_w) X(panel_tall) X(panel_short) X(panel_y_hi) X(panel_y_lo) X(panel_x) X(panel_z) \
	X(panel_d) X(handle_x) X(handle_y) X(door_handle_w) X(handle_d) X(handle_z) X(panel_shade) X(louvre_t) \
	X(slat_fill) X(slat_shade) XI(slats) X(canvas_slope) X(valance_h) X(valance_t)
struct PartModelsLook {
	LOOK_SHAPE_PART_MODELS(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_PLACED(X, XI, XD, XV, XVI) \
	X(mould_k) X(band_low) X(band_mid) X(low_tier)
struct PlacedLook {
	LOOK_SHAPE_PLACED(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_FACADE_BAY(X, XI, XD, XV, XVI) \
	X(min_sill_room) X(min_w) X(w_margin) X(min_h) X(head_room) X(no_opening) X(threshold) X(threshold_below) \
	XI(picks) XI(dark_picks) XI(roll_from) XI(roll_to) XI(loggia_dark) XI(brise_dark) X(roll_base) X(roll_step) \
	X(roll_out) X(tall_opening) X(streak_out) X(streak_min_sill) X(streak_lo) X(streak_base) X(streak_step) \
	X(streak_half) X(streak_half_step) X(streak_top) X(streak_dark) X(streak_mid) XI(streak_skip) \
	XI(streak_reach) XI(streak_width) X(french_streak_sill) X(wall_run_m) X(pane_on_wall_m)
struct FacadeBayLook {
	LOOK_SHAPE_FACADE_BAY(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

#define LOOK_SHAPE_FACADE_PROPS(X, XI, XD, XV, XVI) \
	X(ac_gap) X(ac_under_sill) X(ac_room) X(ac_edge) X(mid) X(quarter_back) X(dish_room) X(dish_off) X(dish_drop) \
	X(pot_high) X(soil_down) X(pot_min) X(pot_span) X(pot_tall) X(pot_tall_span) X(leaf_wide) X(leaf_wide_span) \
	X(leaf_high) X(leaf_high_span) X(floor_lift) X(plant_lift) X(corner_in) X(rail_in) X(mid_drift) X(chair_turn) \
	X(half_deep) X(airer_min) X(table_deep) X(table_big) X(table_small) X(table_tall) X(table_side) \
	X(table_chair) X(table_edge) X(gourmet_lounger_narrow) X(bike_off) X(chair_back_deep) X(garden_end) \
	X(ledge_in) X(gourmet_in) X(ledge_deep) X(gourmet_deep) X(ledge_min) X(inset_in) X(inset_min) X(inset_corner) \
	X(inset_plant) X(inset_chair) X(inset_turn) XI(garden_few) XI(garden_many)
struct FacadePropsLook {
	LOOK_SHAPE_FACADE_PROPS(LOOK_SHAPE_FLOAT, LOOK_SHAPE_COUNT, LOOK_SHAPE_DOUBLE, LOOK_SHAPE_FLOATS, LOOK_SHAPE_COUNTS)
};

} // namespace city::b40
