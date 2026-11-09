// The building generator's shapes of the lots: the parts several zones lay alike and each zone's own layout (back, boundary, cars, front, ground, plan, space, yard, open lots, the slope a site steps the lot down, the fit to a lot's outline): each block's fields (buildings.json "shapes", read by
// api/buildings_loader.cpp) and its struct.
#pragma once

#include "core/shape_fields.h"

namespace city::b40 {

#define B40_PARTS(X, XI, XD, XV, XVI) \
	X(cover_h) X(court_w) X(court_d) X(court_line) X(bench_h) X(drain) X(drain_h) X(pallet) X(pallet_h) X(lamp) \
	X(swing_w) X(swing_d) X(swing_h) X(way_side)
struct LotPartShape {
	SHAPE_DECLARE(B40_PARTS)
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
	SHAPE_DECLARE(B40_BACK)
};

#define B40_BOUNDARY(X, XI, XD, XV, XVI) \
	X(boundary_wall_t) X(fence_thick) X(hedge_thick) X(gate_post) X(gate_gap) X(attach) X(shortest_run) \
	X(walk_trim)
struct LotBoundaryShape {
	SHAPE_DECLARE(B40_BOUNDARY)
};

#define B40_CARS(X, XI, XD, XV, XVI) \
	X(garage_wall) X(garage_set) X(carport_post) X(carport_slab) X(pad_margin) X(drain_before) X(drive_drain_at) \
	X(alley_lamp_min) X(alley_lamp_h) X(drain_every) X(alley_drain) X(bin_w) X(bin_d) X(bin_h) X(crate) \
	X(crate_h) X(back_end) X(crate_gap) X(side_tree_every) X(side_tree) X(side_tree_h) X(side_shrub_h)
struct LotCarsShape {
	SHAPE_DECLARE(B40_CARS)
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
	SHAPE_DECLARE(B40_FRONT)
};

#define B40_GROUND(X, XI, XD, XV, XVI) \
	X(door_way_pad) X(door_way_min) X(edge_keep)
struct LotGroundShape {
	SHAPE_DECLARE(B40_GROUND)
};

#define B40_PLAN(X, XI, XD, XV, XVI) \
	X(drive) X(house_drive) X(footpath) X(alley) X(wide_path) X(truck_lane) X(narrow_w) X(deep_house) X(carpad) \
	X(mid_house) X(small_garden) X(garden_lo) X(garden_hi) X(side_garden_w) X(side_garden_lo) X(side_garden_hi) \
	X(side_path_w) X(house_min) X(house_max) X(yard_lo) X(yard_hi) X(yard_min) X(apartment_w) X(flats_d) \
	X(flats_garden) X(flats_garden_lo) X(flats_garden_hi) X(parking_w) X(min_block) X(attached_share) \
	X(court_spare) X(court_max) X(tower_plaza_d) X(tower_plaza) X(shop_court_d) X(shop_court) X(shop_court_lo) \
	X(shop_court_hi) X(shop_park_w) X(shop_park) X(shop_service) X(service_lo) X(service_hi) X(office_plaza_d) \
	X(plaza_lo) X(plaza_hi) X(office_court) X(office_park_w) X(workplace_min) X(office_service) \
	X(office_service_d) X(narrow_lot) X(corner_garden) X(house_paths_w) X(house_path) X(service_max) X(apron_d) X(apron_deep) \
	X(apron) X(works_w) X(works_d) X(works_path_w) X(loading_full) X(loading_extra) X(loading_max) X(loading_min) \
	X(loading_share)
struct LotPlanShape {
	SHAPE_DECLARE(B40_PLAN)
};

#define B40_SPACE(X, XI, XD, XV, XVI) \
	X(lot_clear_in) X(part_gap) X(cover_step)
struct LotSpaceShape {
	SHAPE_DECLARE(B40_SPACE)
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
	SHAPE_DECLARE(B40_YARD)
};

#define B40_SLOPE(X, XI, XD, XV, XVI) \
	X(min_part) X(fill_weight) X(gain) X(on_edge) X(max_lift) X(grid) XI(samples) X(garage_bay) X(garage_apron) X(apron_spare) \
	X(garage_grade) X(garage_back_min) X(garage_door_out) X(ramp_landing) X(ramp_min) X(house_ramp_start) X(yard_grade) \
	X(works_grade) X(retain_t) X(retain_over) X(retain_in) X(guard_h) X(wall_piece) X(walk_clear) X(flight_w_min) X(flight_trim) \
	X(step_run) X(street_threshold) XI(front_points) X(terrace_min) X(terrace_share) XI(terrace_risers_min) XI(terrace_risers_max) \
	X(terrace_lot_min) X(terrace_from) X(stair_w) X(stair_clear) X(drain_in)
struct LotSlopeShape {
	SHAPE_DECLARE(B40_SLOPE)
};

#define B40_FIT(X, XI, XD, XV, XVI) \
	X(rect_share) X(slice) X(least) X(front_band) X(edge_h) X(lawn_under) X(shortest) X(piece)
struct LotFitShape {
	SHAPE_DECLARE(B40_FIT)
};

#define B40_OPEN(X, XI, XD, XV, XVI) \
	X(lawn_clear) X(path_w) X(ring_share) X(ring_w) X(tree_clear) X(ring_min) X(basin) X(pool_plaza) X(rim_h) \
	X(rim_t) X(spout) X(spout_h) X(bowl) X(bowl_h) X(fountain_min) X(play) X(play_min) X(hedge_h) X(hedge_t) \
	X(bench_w) X(bench_d) X(bench_every) X(lamp) X(lamp_h) X(lamp_every) X(bin) X(bin_h) X(post) X(swing_h) \
	X(swing_w) X(frame) X(frame_h) X(seat) X(seat_z) X(seesaw_l) X(seesaw_w) X(seesaw_z) X(chain) X(lot_edge) \
	X(band_min) X(band) X(band_col) X(band_h) X(band_roof) X(band_deck) X(court_run) X(fence_h) X(goal_w) \
	X(goal_h) X(goal_d) X(circle) X(lift_line) X(lift_inner)
struct OpenLotShape {
	SHAPE_DECLARE(B40_OPEN)
};

} // namespace city::b40
