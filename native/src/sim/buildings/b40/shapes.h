// The building generator's shapes: every value one tunes in a generator file, one block per file (and the parts several
// files lay alike), filled from data/catalog/buildings.json "shapes" by api/buildings_loader.cpp (a missing key refuses the
// catalogue). Each list gives a field its type: X a float, XI a count, XD a double, XV and XVI lists of floats and of counts.
#pragma once

#include <vector>

namespace city::b40 {

#define B40_SHAPE_FLOAT(n) float n = 0;
#define B40_SHAPE_COUNT(n) int n = 0;
#define B40_SHAPE_DOUBLE(n) double n = 0;
#define B40_SHAPE_FLOATS(n) std::vector<float> n;
#define B40_SHAPE_COUNTS(n) std::vector<int> n;

#define B40_CIVIC(X, XI, XD, XV, XVI) \
	X(front_share) X(front_min) X(front_max) X(side_gap) X(back_gap) X(civic_storey) X(wing_d) X(wing_d_share) \
	X(side_wing_w) X(side_wing_share) X(school_roof_pitch) X(court_edge) XI(school_storeys_lo) \
	XI(school_storeys_hi) X(nave_w) X(nave_w_share) X(nave_d) X(nave_h) X(nave_pitch) X(tower_w) X(tower_storey) \
	X(tower_pitch) X(cross_h) X(cross_w) X(cross_t) X(cross_arm_z) XI(tower_storeys) X(ward_share) X(ward_d) \
	X(ward_d_share) X(podium_d_share) X(sign_w) X(sign_t) X(sign_bar) X(sign_out) X(band_h) X(band_out) \
	X(canopy_w) X(canopy_d) X(canopy_t) XI(hospital_lo) XI(hospital_hi) XI(clinic_storeys)
struct CivicShape {
	B40_CIVIC(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
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
	B40_SERVICES(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_FACADE(X, XI, XD, XV, XVI) \
	XI(low_storeys) XI(crown_min_levels) X(canopy_edge) X(canopy_wide_bays) X(canopy_pad) X(canopy_drop) \
	X(canopy_max_z) X(canopy_out) X(canopy_deep) X(canopy_thick) X(recess) X(recess_min_w) X(recess_share) \
	X(small_canopy_max) X(small_drop) X(small_max_z) X(small_out) X(small_deep) X(small_thick) \
	XI(relief_min_storeys) XI(relief_min_bays) XI(relief_wide_bays) XI(pier_min_storeys) X(stone_base_k) \
	X(relief_skip) X(relief_out) X(pier_out)
struct FacadeShape {
	B40_FACADE(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_MASSING(X, XI, XD, XV, XVI) \
	X(min_pitch) X(flat_below) X(ridge_to_street) X(sawtooth_run) X(rise_gable) X(rise_shed) X(rise_sawtooth) \
	X(rise_other) XV(pitch_gable) XV(pitch_hip) XV(pitch_shed) XV(pitch_saw) XV(pitch_other) X(second_skin_light) \
	X(second_skin_nudge) XI(second_skin_tries) X(membrane_nudge)
struct MassingShape {
	B40_MASSING(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
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
	B40_HOUSE(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
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
	B40_TALL(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_WORKS(X, XI, XD, XV, XVI) \
	X(hall_lo) X(hall_hi) X(office_storey) X(annex_d) X(annex_w) X(annex_room) X(annex_w_lo) X(annex_w_hi) \
	X(annex_d_lo) X(annex_d_hi) XI(loading_every)
struct WorksShape {
	B40_WORKS(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_PAINT(X, XI, XD, XV, XVI) \
	XD(hue_step) XD(sat_step) XD(light_step) XD(light_min) XD(light_max) XD(tint_lo) XD(tint_span)
struct PaintShape {
	B40_PAINT(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_PARTS(X, XI, XD, XV, XVI) \
	X(cover_h) X(court_w) X(court_d) X(court_line) X(bench_h) X(drain) X(drain_h) X(pallet) X(pallet_h) X(lamp) \
	X(swing_w) X(swing_d) X(swing_h) X(way_side)
struct LotPartShape {
	B40_PARTS(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_BACK(X, XI, XD, XV, XVI) \
	X(bay_off_wall) X(row_from) X(door_gap) X(planter_min) X(planter_d) X(planter_h) X(crown_top) X(walk_w) \
	X(bin_w) X(bin_d) X(bin_h) X(skip_w) X(skip_d) X(skip_h) X(bin_x) X(skip_x) X(bin_y) X(skip_y) X(pallet_x) \
	X(pallet_y) X(cage_w) X(cage_d) X(cage_h) X(cage_in) X(ac_w) X(ac_d) X(ac_h) X(ac_z) X(ac_every) \
	X(service_lamp_h) X(service_lamp_x) X(service_lamp_y) X(cage_min_w) X(square_w) X(square_d) X(square_from) \
	X(bench_w) X(bench_d) X(bench_x) X(play) X(play_in) X(play_min_d) X(play_min_w) X(court_tree) \
	X(court_tree_in) X(court_tree_back) X(court_bin_in) X(dock_edge) X(dock_on_door) X(dock_apart) X(bollard) \
	X(bollard_h) X(bollard_out) X(yard_lamp_h) X(yard_lamp_in) X(yard_back) X(wall_lamp_every) X(wall_lamp_z) \
	X(wall_lamp_out) X(wall_lamp_w) X(wall_lamp_h)
struct LotBackShape {
	B40_BACK(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_BOUNDARY(X, XI, XD, XV, XVI) \
	X(boundary_wall_t) X(fence_thick) X(hedge_thick) X(gate_post) X(gate_gap) X(attach) X(shortest_run) \
	X(walk_trim)
struct LotBoundaryShape {
	B40_BOUNDARY(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_CARS(X, XI, XD, XV, XVI) \
	X(garage_wall) X(garage_set) X(carport_post) X(carport_slab) X(pad_margin) X(drain_before) X(drive_drain_at) \
	X(alley_lamp_min) X(alley_lamp_h) X(drain_every) X(alley_drain) X(bin_w) X(bin_d) X(bin_h) X(crate) \
	X(crate_h) X(back_end) X(crate_gap) X(side_tree_every) X(side_tree) X(side_tree_h) X(side_shrub_h)
struct LotCarsShape {
	B40_CARS(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_FRONT(X, XI, XD, XV, XVI) \
	X(path_w) X(link_d) X(bed_in) X(bed_every) X(bed) X(bed_h) X(shrub_share) X(window_bed_w) X(window_bed_d) \
	X(window_bed_h) X(window_bed_every) X(window_bed_out) X(front_crown) X(front_crown_top) X(tree_min_f) \
	X(tree_in) X(tree_every) X(tree_from) X(pair_every) X(pair_share) X(pair_min_f) X(pair_at) X(pair_gap) \
	X(bench_w) X(bench_d) X(planter) X(planter_h) X(bollard) X(bollard_h) X(bollard_at) X(bollard_every) \
	X(plaza_bollard_every) X(bin_w) X(bin_d) X(bin_h) X(bin_in) X(bin_out) X(plaza_every) X(plaza_from) \
	X(plaza_planter) X(plaza_tree) X(plaza_tree_h) X(plaza_at) X(plaza_bench) X(plaza_bench_d) X(lamp_h) \
	X(lamp_in) X(lamp_back) X(apron_path) X(apron_lamp_h) X(apron_lamp_x) X(apron_lamp_in) X(visitor_from) \
	X(visitor_door)
struct LotFrontShape {
	B40_FRONT(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_GROUND(X, XI, XD, XV, XVI) \
	X(door_way_pad) X(door_way_min) X(edge_keep)
struct LotGroundShape {
	B40_GROUND(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_PLAN(X, XI, XD, XV, XVI) \
	X(drive) X(house_drive) X(footpath) X(alley) X(wide_path) X(truck_lane) X(narrow_w) X(deep_house) X(carpad) \
	X(mid_house) X(small_garden) X(garden_lo) X(garden_hi) X(side_garden_w) X(side_garden_lo) X(side_garden_hi) \
	X(side_path_w) X(house_min) X(house_max) X(yard_lo) X(yard_hi) X(yard_min) X(apartment_w) X(flats_d) \
	X(flats_garden) X(flats_garden_lo) X(flats_garden_hi) X(parking_w) X(min_block) X(attached_share) \
	X(court_spare) X(court_max) X(tower_plaza_d) X(tower_plaza) X(shop_court_d) X(shop_court) X(shop_court_lo) \
	X(shop_court_hi) X(shop_park_w) X(shop_park) X(shop_service) X(service_lo) X(service_hi) X(office_plaza_d) \
	X(plaza_lo) X(plaza_hi) X(office_court) X(office_park_w) X(workplace_min) X(office_service) \
	X(office_service_d) X(narrow_lot) X(house_paths_w) X(house_path) X(service_max) X(apron_d) X(apron_deep) \
	X(apron) X(works_w) X(works_d) X(works_path_w) X(loading_full) X(loading_extra) X(loading_max) X(loading_min) \
	X(loading_share)
struct LotPlanShape {
	B40_PLAN(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_SPACE(X, XI, XD, XV, XVI) \
	X(lot_clear_in) X(part_gap) X(cover_step)
struct LotSpaceShape {
	B40_SPACE(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_YARD(X, XI, XD, XV, XVI) \
	X(pool_gap) X(pool_side) X(pool_room) X(deck) X(lounger_w) X(lounger_l) X(lounger_h) X(lounger_gap) \
	X(lounger_out) X(bed_every) X(bed_w) X(bed_d) X(bed_h) X(bed_back) X(bed_from) X(corner_tree) X(tree_h) \
	X(corner_tree_in) X(corner_tree_back) X(bush) X(bush_top) X(shrub_in) X(shrub_every) X(shrub_to) \
	X(clothes_share) X(line_min_d) X(line_h) X(line_span) X(line_from) X(line_d) X(bench_w) X(bench_d) X(bench_x) \
	X(pot) X(pot_h) X(pot_every) X(pot_in) X(corner) X(bin_w) X(bin_d) X(bin_h) X(life_from) X(life_in) \
	X(orchard_every) X(fruit) X(fruit_h) X(plot_w) X(box_d) X(box_h) X(box_x) X(box_y) X(box_in) XI(box_cols) \
	XI(box_rows) X(coop_w) X(coop_d) X(coop_h) X(coop_in) X(sand) X(shed_w) X(shed_d) X(shed_h) X(edicula_w) \
	X(edicula_d) X(edicula_h) X(table) X(table_h) X(seat_w) X(seat_d) X(seat_at) X(dog) X(dog_h) X(deck_d) \
	X(free_min) X(small_w) X(small_d) X(planter) X(planter_h) X(square_max) X(square_in) X(free_tree_in) \
	X(free_flowers) X(flowers_h)
struct LotYardShape {
	B40_YARD(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_OPEN(X, XI, XD, XV, XVI) \
	X(lawn_clear) X(path_w) X(ring_share) X(ring_w) X(tree_clear) X(ring_min) X(basin) X(pool_plaza) X(rim_h) \
	X(rim_t) X(spout) X(spout_h) X(bowl) X(bowl_h) X(fountain_min) X(play) X(play_min) X(hedge_h) X(hedge_t) \
	X(bench_w) X(bench_d) X(bench_every) X(lamp) X(lamp_h) X(lamp_every) X(bin) X(bin_h) X(post) X(swing_h) \
	X(swing_w) X(frame) X(frame_h) X(seat) X(seat_z) X(seesaw_l) X(seesaw_w) X(seesaw_z) X(chain) X(lot_edge) \
	X(band_min) X(band) X(band_col) X(band_h) X(band_roof) X(band_deck) X(court_run) X(fence_h) X(goal_w) \
	X(goal_h) X(goal_d) X(circle) X(lift_line) X(lift_inner)
struct OpenLotShape {
	B40_OPEN(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

// Every block: its key in the table, its type and its list.
#define B40_SHAPES(S) \
	S(civic, CivicShape, B40_CIVIC) \
	S(services, ServiceShape, B40_SERVICES) \
	S(facade, FacadeShape, B40_FACADE) \
	S(massing, MassingShape, B40_MASSING) \
	S(house, HouseShape, B40_HOUSE) \
	S(tall, TallShape, B40_TALL) \
	S(works, WorksShape, B40_WORKS) \
	S(paint, PaintShape, B40_PAINT) \
	S(parts, LotPartShape, B40_PARTS) \
	S(back, LotBackShape, B40_BACK) \
	S(boundary, LotBoundaryShape, B40_BOUNDARY) \
	S(cars, LotCarsShape, B40_CARS) \
	S(front, LotFrontShape, B40_FRONT) \
	S(ground, LotGroundShape, B40_GROUND) \
	S(plan, LotPlanShape, B40_PLAN) \
	S(space, LotSpaceShape, B40_SPACE) \
	S(yard, LotYardShape, B40_YARD) \
	S(open, OpenLotShape, B40_OPEN)

struct Shapes {
#define B40_SHAPE_MEMBER(key, Type, LIST) Type key;
	B40_SHAPES(B40_SHAPE_MEMBER)
#undef B40_SHAPE_MEMBER
};

} // namespace city::b40
