// The building generator's shapes of the interiors: flights and room counts, homes, the rooms of other floors and the floor plan: each block's fields (buildings.json "shapes", read by
// api/buildings_loader.cpp) and its struct.
#pragma once

#include "sim/buildings/b40/shape_fields.h"

namespace city::b40 {

#define B40_INTERIOR(X, XI, XD, XV, XVI) \
	X(flight_least_m) X(flight_over_m) X(flight_line_m) X(flight_rise_m) X(flight_room_m) X(flight_tread_m) \
	X(flight_tread_min_m) X(flight_width_m) X(flight_side_m) X(flight_clear_m) X(room_reach) X(unit_modules) \
	XI(office_room_bays) XI(class_room_bays) XI(hotel_room_bays) XI(ward_room_bays) XI(light_share) \
	XI(wood_share) X(ward_from_ground) X(reach_past_m) X(reach_unsized_m)
struct InteriorShape {
	B40_INTERIOR(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_HOMES(X, XI, XD, XV, XVI) \
	X(door_margin) X(door_share) X(bed_door_in) X(bath_d) X(bath_door) X(tv_u) X(room_mid) X(sofa_in) X(sofa_u) \
	X(table_s) X(chair_u) X(chair_s) X(fridge_u) X(fridge_s) X(stove_u) X(stove_s) X(sink_u) X(plant_u) \
	X(plant_s) X(bed_u) X(bed_s) X(wardrobe_s) X(toilet_u) X(toilet_s) X(bath_u) X(bath_s) X(small_house) \
	X(desk_off) X(chair_off) X(shelf_off) X(rack_off) X(shelf_back) X(mid_margin) X(stair_run) X(stair_x) \
	X(stair_y) X(kitchen_gap) X(kitchen_end) X(sofa_x) X(sofa_end) X(armchair_x) X(armchair_y) X(plant_end) \
	X(book_y) X(fridge_x) X(stove_x) X(sink_x) X(kitchen_y) X(kitchen_fit) X(table_back) X(seat_x) X(seat_y) \
	X(bath_deep) X(room_door) X(bed_end) X(wardrobe_x) X(wardrobe_y) X(bath_x) X(toilet_y) X(hall_d) \
	X(door_apart) X(desk_y) X(sink_y) X(sink_shift) X(wardrobe_mid) X(wardrobe_front) X(unit_min) X(unit_door) \
	X(unit_split) X(hotel_bed) X(hotel_wardrobe_x) X(hotel_wardrobe_y) X(hotel_desk) X(live_door) X(sofa_share) \
	X(sofa_back) X(tv_off) X(table_share) X(seat_off) X(u_fridge) X(u_stove) X(u_sink) X(u_kitchen) X(bed_share) \
	X(wide_unit) X(bath_wall) X(bath_wall_door) X(u_bath_x) X(u_bath_y) X(u_toilet_x) X(u_toilet_y) X(u_plant) \
	X(desk_front) X(lobby_sofa_x) X(lobby_sofa_y) X(lobby_plant)
struct InteriorHomeShape {
	B40_HOMES(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_ROOMS(X, XI, XD, XV, XVI) \
	X(room_door_min) X(room_door_back) X(meet_depth) X(meet_seat) X(meet_row) X(shelf_off) X(plant_off) \
	X(desks_after) X(wc_depth) X(wc_door) X(toilet_gap) X(toilet_from) X(toilet_s) X(basin_s) X(basin_a) \
	X(basin_b) X(desks_from) X(desks_back) X(shelf_back) X(plant_back) X(desk_row) X(desk_col) X(desk_u) \
	X(desk_end) X(chair_s) X(aisle) X(open_in) X(open_gx) X(open_gy) X(open_chair) X(open_plant) \
	X(open_plant_gap) X(board_u) X(teacher_u) X(pupil_u) X(pupil_gap) X(pupil_s) X(pupil_end) X(pupil_chair) \
	X(class_room) X(class_min) X(class_door) X(board_back) X(teacher_back) X(row_in) X(row_from) X(row_back) \
	X(row_gx) X(row_gy) X(row_chair) X(bed_s) X(bed_gap) X(bed_u) X(bed_end) X(visitor_u) X(two_rows) \
	X(ward_plant) X(ward_wall) X(ward_in) X(ward_from) X(ward_gx) X(ward_gy) X(hotel_bed) X(hotel_bed_in) \
	X(hotel_bed_u) X(hotel_tv) X(hotel_desk_u) X(hotel_desk_s) X(hotel_wardrobe_u) X(hotel_wardrobe_s) \
	X(hotel_bath_s) X(hotel_bath_door) X(hotel_toilet_u) X(hotel_toilet_s) X(hotel_tub_u) X(hotel_tub_s) \
	X(back_need) X(back_door) X(shop_back) X(eat_back) X(bar_back) X(counter_off) X(shelf_from) X(shelf_to) \
	X(shelf_gap) X(mid_in) X(mid_to) X(mid_gx) X(mid_gy) X(rack_from) X(rack_gap) X(rack_end) X(rack_off) \
	X(stock_desk_u) X(stock_desk_s) X(stock_toilet_u) X(stock_toilet_s) X(eat_counter) X(eat_from) X(eat_to) \
	X(bar_counter) X(bar_seats_w) X(bar_seats_from) X(bar_seats_to) X(bar_seat_gx) X(bar_seat_gy) X(bar_to) \
	X(cook_from) X(cook_end) X(cook_gap) X(cook_s) X(cook_table) X(cook_table_scale) X(dine_y) X(dine_x) \
	X(dine_row) X(dine_col) X(dine_scale) X(dine_seat) X(room_cell) X(cell_bed) X(cell_table_in) X(cell_table_to) \
	X(cell_table_gx) X(mall_unit) X(mall_door) X(mall_counter) X(mall_shelf) X(mall_shelf_s) X(sofa_in) \
	X(sofa_from) X(sofa_to) X(sofa_gx) X(sofa_gy) X(mall_plant_in) X(mall_plant_from) X(mall_plant_gx) \
	X(mall_plant_gy)
struct InteriorRoomShape {
	B40_ROOMS(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

#define B40_FLOOR_PLAN(X, XI, XD, XV, XVI) \
	X(inset) X(pad) X(front_room) X(core_x) X(core_y) X(corridor) X(double_loaded) X(off_wall)
struct FloorPlanShape {
	B40_FLOOR_PLAN(B40_SHAPE_FLOAT, B40_SHAPE_COUNT, B40_SHAPE_DOUBLE, B40_SHAPE_FLOATS, B40_SHAPE_COUNTS)
};

} // namespace city::b40
