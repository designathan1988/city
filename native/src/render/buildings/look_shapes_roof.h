// The building render's shapes of the roofs: the edges, the plant, the terraces, the gear (building_look.json shapes, read by building_look.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city::b40 {

#define LOOK_SHAPE_ROOF_EMIT(X, XI, XD, XV, XVI) \
	X(parapet_t) X(coping_out) X(lip_h) X(inner_shade) X(lip_shade) X(cornice_drop) X(cornice_out) X(cornice_h) \
	X(deco_out) X(deco_h) X(deco2_drop) X(deco2_out) X(deco2_h) X(slab_up) X(slab_out) X(slab_h) X(rail_in) \
	X(machine_in) X(machine_min)
struct RoofEmitLook {
	SHAPE_DECLARE(LOOK_SHAPE_ROOF_EMIT)
};

#define LOOK_SHAPE_ROOF_PLANT(X, XI, XD, XV, XVI) \
	XV(fan_at) X(panel_gap) XV(tall_tank) X(coarse_min_h) X(cell) X(margin) X(min_side) X(hatch_p) X(big_roof) \
	X(age_k) X(mast_tall) X(solar_age) X(mast_age) X(decay) X(round_jitter) X(turn) X(solar_var) X(solar_base) \
	XI(lift_storeys) XI(tank_storeys) XI(mast_storeys) XI(per_area_cap) XI(variants) X(drain) X(drain_at) \
	X(drain_t) X(plinth) X(tank_r) X(rim_r) X(rim_drop) X(lid_r) X(lid_up) X(top_shade) X(lid_shade) X(casing_up) \
	X(fan_r) X(fan_h) X(cond_up) X(fan_plate) X(panel_w) X(panel_step) X(panel_row) X(panel_max) X(panel_z0) \
	X(panel_z1) X(pole) X(bar) X(bar2) X(bar_drop) X(bar2_drop) X(dish_post) X(dish_plate_z) X(dish_plate_t) \
	X(vent_r) X(vent_cap_r) X(vent_cap) X(chim_r) X(chim_top) X(door_w) X(door_h) X(door_t) X(lid) X(lid_t) \
	X(ladder) X(ladder_up)
struct RoofPlantLook {
	SHAPE_DECLARE(LOOK_SHAPE_ROOF_PLANT)
};

#define LOOK_SHAPE_ROOF_TERRACE(X, XI, XD, XV, XVI) \
	X(terrace_min_width) X(deck_in) X(deck_share) X(deck_span) X(deck_t) X(pot_in) X(pot_end) X(pot_r) X(pot_h) \
	X(pot_every) X(pot_span) X(table_every) X(table_share) X(table_span) X(table_half) X(table_lo) X(table_hi) \
	X(pole_half) X(parasol_half) X(parasol_low) X(parasol_high) X(stool_half) X(stool_h) X(stool_off) \
	X(lounge_room) X(lounge_after) X(lounge_from) X(lounge_end) X(lounge_every) X(lounge_half) X(lounge_w) \
	X(lounge_h) X(post_half) X(post_h) X(post_every) X(beam_half) X(beam_past) X(beam_lo) X(slat_every) \
	X(slat_half) X(slat_reach) X(slat_lo) X(slat_hi) X(pergola_half) X(tank_in) X(tank_r) X(tank_h) X(rim_r) \
	X(rim_top) X(rim_shade) X(pots) X(loungers) X(pergola) X(tanks)
struct RoofTerraceLook {
	SHAPE_DECLARE(LOOK_SHAPE_ROOF_TERRACE)
};

#define LOOK_SHAPE_ROOF_GEAR(X, XI, XD, XV, XVI) \
	X(panel_lift) X(panel_top) X(panel_rim) X(vent_h) X(tank_h) X(chimney_h) X(cap_h) X(cap_out) X(gear_sink_m) \
	X(spire_foot_h) X(spire_foot_sink) X(spire_share) X(lantern_h) X(base_h) X(glass_h) X(band_t) X(band_out) \
	X(mullion) X(lid_out) X(lid_h) X(dome_h) X(dome_top) X(mast_foot) X(mast_top) X(beacon) X(beacon_low) \
	X(beacon_high) XI(round) XI(bands) XI(dome_steps) X(spire_flag_at) X(spire_flag_w) X(spire_flag_h) \
	X(lantern_flag_drop) X(lantern_flag_w) X(lantern_flag_aspect)
struct RoofGearLook {
	SHAPE_DECLARE(LOOK_SHAPE_ROOF_GEAR)
};

#define LOOK_SHAPE_ROOF_PITCHED(X, XI, XD, XV, XVI) \
	X(eaves) X(thick) X(fascia) X(soffit) X(cap_w) X(cap_lift) X(cap_shade) X(mullion)
struct RoofPitchedLook {
	SHAPE_DECLARE(LOOK_SHAPE_ROOF_PITCHED)
};

} // namespace city::b40
