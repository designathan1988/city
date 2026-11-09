// The building generator's shapes of the bodies: civic and service buildings, the facade grammar, the massing of houses, flats, shops, offices and works, the colours' nudge, a body read off a model, a roof's defaults: each block's fields (buildings.json "shapes", read by
// api/buildings_loader.cpp) and its struct.
#pragma once

#include "core/shape_fields.h"

namespace city::b40 {

#define B40_CIVIC(X, XI, XD, XV, XVI) \
	X(front_share) X(front_min) X(front_max) X(side_gap) X(back_gap) X(civic_storey) X(wing_d) X(wing_d_share) \
	X(side_wing_w) X(side_wing_share) X(school_roof_pitch) X(court_edge) XI(school_storeys_lo) \
	XI(school_storeys_hi) X(nave_w) X(nave_w_share) X(nave_d) X(nave_h) X(nave_pitch) X(tower_w) X(tower_storey) \
	X(tower_pitch) X(cross_h) X(cross_w) X(cross_t) X(cross_arm_z) XI(tower_storeys) X(ward_share) X(ward_d) \
	X(ward_d_share) X(podium_d_share) X(sign_w) X(sign_t) X(sign_bar) X(sign_out) X(band_h) X(band_out) \
	X(canopy_w) X(canopy_d) X(canopy_t) XI(hospital_lo) XI(hospital_hi) XI(clinic_storeys)
struct CivicShape {
	SHAPE_DECLARE(B40_CIVIC)
};

#define B40_SERVICES(X, XI, XD, XV, XVI) \
	X(store_front_w) X(shop_d) X(shop_h) X(canopy_z) X(canopy_t) X(canopy_w_max) X(canopy_d) X(canopy_edge) \
	X(column) X(pump_w) X(pump_d) X(pump_h) X(island_w) X(island_d) X(island_h) X(pump_every) X(stripe_t) \
	X(market_h) X(mall_h) X(sign_h) X(sign_out) X(sign_share) X(stall_d) X(stripe_w) X(aisle) X(park_share) \
	X(park_max) X(cart_w) X(cart_d) X(cart_h) X(hall_ground) X(hall_storey) X(portico_d) X(portico_w_share) \
	X(portico_w_max) X(col) X(col_h) X(entab_h) X(step_h) X(step_in) X(flag_pole) X(flag_h) X(flag_w) X(flag_d) \
	X(flag_t) X(stair_h) X(stair_d) XI(columns) XI(hall_storeys) XI(pediment_steps) X(station_storey) X(tower_w) \
	X(band_h) X(band_out) XI(station_storeys) XI(tower_storeys) XI(garage_bays)
struct ServiceShape {
	SHAPE_DECLARE(B40_SERVICES)
};

#define B40_FACADE(X, XI, XD, XV, XVI) \
	XI(low_storeys) XI(crown_min_levels) X(canopy_edge) X(canopy_wide_bays) X(canopy_pad) X(canopy_drop) \
	X(canopy_max_z) X(canopy_out) X(canopy_deep) X(canopy_thick) X(recess) X(recess_min_w) X(recess_share) \
	X(small_canopy_max) X(small_drop) X(small_max_z) X(small_out) X(small_deep) X(small_thick) \
	XI(relief_min_storeys) XI(relief_min_bays) XI(relief_wide_bays) XI(pier_min_storeys) X(stone_base_k) \
	X(relief_skip) X(relief_out) X(pier_out) X(street_m) X(quarter_m) X(centre_m) X(central_old) X(central_middle) X(rim_old) 	X(rim_middle)
struct FacadeShape {
	SHAPE_DECLARE(B40_FACADE)
};

#define B40_MASSING(X, XI, XD, XV, XVI) \
	X(min_pitch) X(flat_below) X(ridge_to_street) X(sawtooth_run) X(rise_gable) X(rise_shed) X(rise_sawtooth) \
	X(rise_other) XV(pitch_gable) XV(pitch_hip) XV(pitch_shed) XV(pitch_saw) XV(pitch_other) X(second_skin_light) \
	X(second_skin_nudge) XI(second_skin_tries) X(membrane_nudge)
struct MassingShape {
	SHAPE_DECLARE(B40_MASSING)
};

#define B40_HOUSE(X, XI, XD, XV, XVI) \
	X(storey_min_h) X(ground_hi) X(storey_lo) X(storey_hi) X(wide_lot) X(bungalow_w) X(bungalow_d) \
	X(ell_wing_share) X(ell_wing_lo) X(ell_wing_hi) X(ell_step_share) X(ell_step_lo) X(ell_step_hi) X(main_lo) \
	X(main_hi) X(ell_wing_depth) X(ell_same_storeys) X(ver_lo) X(ver_hi) X(ver_main_lo) X(ver_main_hi) \
	X(ver_back_room) X(ver_back) X(ver_back_min) X(ver_back_lo) X(ver_back_hi) X(ver_inset) X(ver_short) \
	X(ver_short_share) X(ver_floor_h) X(ver_slab_drop) X(ver_slab_h) X(ver_slab_pad) X(ver_slab_trim) \
	X(ver_slab_back) X(post_every) X(post) X(post_half) X(twin_lo) X(twin_hi) X(twin_back_lo) X(twin_back_hi) \
	X(twin_main_lo) X(twin_main_hi) X(bung_pitch_lo) X(bung_pitch_hi) X(wing_min_room) X(wing_min) X(wing_lo) \
	X(wing_hi) X(double_door)
struct HouseShape {
	SHAPE_DECLARE(B40_HOUSE)
};

#define B40_TALL(X, XI, XD, XV, XVI) \
	XI(round_sides) X(flats_ground_lo) X(flats_ground_hi) X(flats_storey_lo) X(flats_storey_hi) X(tall_share) \
	X(shop_ground) X(hip_share) X(terrace_share) X(bar_lo) X(bar_hi) X(facade_setback_lo) X(inset_hi) \
	X(tower_min) X(tower_max_d) X(office_max_d) X(ell_tower_w) X(twin_tower_w) X(ell_lo) X(ell_hi) X(leg_min) \
	X(gap_lo) X(gap_hi) XI(twin_min) XI(lift_extra) X(cornice_in_lo) X(crown_in_hi) X(crown_min) X(shape_rect) \
	X(shape_oct) X(cut_lo) X(cut_hi) X(setback_lo) X(setback_hi) XI(setback_split) XI(setback_steps) \
	XI(setback_part) X(u_room) X(u_w) X(u_wing_lo) X(u_wing_max) X(l_w) X(l_share) X(l_wing_lo) X(l_wing_max) \
	X(l_low) X(l_low_share) X(pair_w) X(pair_share) X(pair_lo) X(pair_hi) X(pair_back_lo) X(pair_back_hi) \
	X(pair_skin) X(bar_step) X(bar_step_d) XI(bar_step_floors) X(flats_core_d) X(office_core_d) X(shop_ground_lo) \
	X(shop_ground_hi) X(shop_storey) X(shop_pitch) X(shop_terrace) X(shop_deep) X(shop_main_lo) X(shop_back) \
	X(shop_pair_w) X(shop_pair) X(store_min) X(shop_double) X(shop_side_door_w) X(awning_share) X(awning_y) \
	X(awning_min) X(awning_d_lo) X(awning_d_hi) X(awning_z) X(awning_h) X(office_ground_lo) X(office_ground_hi) \
	X(office_storey_lo) X(office_storey_hi) X(podium_share) X(podium_min) X(office_crown_lo) X(office_crown_hi) \
	XI(apartment_min) XI(max_flats) XI(min_tower) XI(max_tower) XI(min_tall) XI(max_tall) XI(shop_max) \
	XI(shop_low_max) XV(setback_shares) XVI(twin_steps) XVI(office_twin_steps) XI(workplace_min_f) \
	XI(office_max_f) XI(office_twin_min)
struct TallShape {
	SHAPE_DECLARE(B40_TALL)
};

#define B40_WORKS(X, XI, XD, XV, XVI) \
	X(hall_lo) X(hall_hi) X(office_storey) X(annex_d) X(annex_w) X(annex_room) X(annex_w_lo) X(annex_w_hi) \
	X(annex_d_lo) X(annex_d_hi) XI(loading_every)
struct WorksShape {
	SHAPE_DECLARE(B40_WORKS)
};

#define B40_PAINT(X, XI, XD, XV, XVI) \
	XD(hue_step) XD(sat_step) XD(light_step) XD(light_min) XD(light_max) XD(tint_lo) XD(tint_span)
struct PaintShape {
	SHAPE_DECLARE(B40_PAINT)
};

#define B40_SURVEY(X, XI, XD, XV, XVI) \
	X(touch_m) X(flat_normal) X(least_top_z) X(least_top_side) X(least_block) X(least_block_h) X(first_storey_m) \
	X(storey_m) X(on_first_storey_m) X(face_out_m) X(light_wall) X(base_differs) X(low_share_end) X(pier_w_m) \
	X(pier_d_m) X(crown_least_m) X(crown_share) X(above_roof_m) X(wall_share) X(door_share) X(wide_share) \
	X(hull_gap_m) X(sample_from) X(wall_up_from) X(wall_up_step) X(bay_up_from) X(bay_up_span) XI(wall_along) \
	XI(wall_up) XI(bay_along) XI(bay_up) XI(least_samples) XI(least_light) XI(tall_storeys) XI(top_storeys) \
	XI(least_based_top) X(least_cell_m) X(hidden_m)
struct SurveyShape {
	SHAPE_DECLARE(B40_SURVEY)
};

#define B40_ROOF(X, XI, XD, XV, XVI) \
	X(gable_deg) X(shed_deg) X(saw_step_m)
struct RoofShape {
	SHAPE_DECLARE(B40_ROOF)
};

} // namespace city::b40
