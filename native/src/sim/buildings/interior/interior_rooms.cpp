// Floors that are not homes: rooms off a corridor (offices, classrooms, wards, hotel rooms), open offices, shops with their
// stockroom, places to eat with their kitchen, cells and malls. Clearances are the ones a person needs to pass and sit.
#include "sim/buildings/interior/plan.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float GRID_EPS = 1e-6f; // a row of cells may end this far past the floor

void desk_rows(const RoomCell &r, float s0, float s1) {
	const InteriorRoomShape &K = r.p.data.shapes.rooms;
	for (float s = s0; s + K.desk_end <= s1; s += K.desk_row)
		for (float u = K.desk_u; u + K.desk_end <= r.width(); u += K.desk_col) {
			r.put(FK_DESK, u, s, 0, -1);
			r.put(FK_OFFICE_CHAIR, u, s + K.chair_s, 0, -1);
		}
}

// A room across the back of the floor, behind a wall with a door near its end: where the wall stands, or y1 with no room.
float back_room(Plan &p, float depth) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	if (p.y1 - p.y0 < depth + K.back_need) return p.y1;
	const float y = p.y1 - depth;
	p.wall_across(y, p.x0, p.x1, p.x1 - K.back_door);
	return y;
}

void kitchen(Plan &p, float wall) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	constexpr FurnitureKind COOK[] = { FK_STOVE, FK_SINK, FK_FRIDGE };
	int k = 0;
	for (float x = p.x0 + K.cook_from; x <= p.x1 - K.cook_end; x += K.cook_gap) p.put(COOK[k++ % std::size(COOK)], x, p.y1 - K.cook_s, face(0, -1));
	p.put(FK_TABLE, p.cx(), (wall + p.y1) / 2 - K.cook_table, 0, K.cook_table_scale);
}

void dining(Plan &p, float from, float to) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	for (float y = from + K.dine_y; y <= to - K.dine_y; y += K.dine_row)
		for (float x = p.x0 + K.dine_x; x <= p.x1 - K.dine_x; x += K.dine_col) {
			if (!p.put(FK_TABLE, x, y, 0, K.dine_scale)) continue;
			p.put(FK_CHAIR, x - K.dine_seat, y, RIGHT_ANGLE);
			p.put(FK_CHAIR, x + K.dine_seat, y, -RIGHT_ANGLE);
		}
}
} // namespace

bool rooms_floor(Plan &p, int group_bays, RoomFill fill) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	const FlatPlan plan = flat_plan(p.data, p.v, group_bays);
	if (!plan.corridor && plan.flats.size() < 2) return false;
	for (size_t i = 0; i < plan.flats.size(); i++) {
		const RoomCell r{ p, plan.flats[i], int(i), int(plan.flats.size()), plan.corridor };
		const float W = r.width(), D = r.depth(), door = std::max(K.room_door_min, W - K.room_door_back);
		if (plan.corridor) {
			if (door - HALF_DOOR > 0) r.wall(0, D, door - HALF_DOOR, D);
			if (door + HALF_DOOR < W) r.wall(door + HALF_DOOR, D, W, D);
		}
		r.wall(0, 0, 0, D);
		fill(r);
	}
	return true;
}

void office_room(const RoomCell &r) {
	const InteriorRoomShape &K = r.p.data.shapes.rooms;
	if (r.index == 0) { // the meeting room
		const float s = std::min(r.depth() / 2, K.meet_depth);
		r.put(FK_TABLE, r.width() / 2, s, 0, -1);
		for (float side : { -1.f, 1.f })
			for (float k : { -1.f, 0.f, 1.f }) r.put(FK_CHAIR, r.width() / 2 + k * K.meet_seat, s + side * K.meet_row, 0, -side);
		r.put(FK_SHELF, K.shelf_off, s, 1, 0);
		r.put(FK_PLANT, K.plant_off, K.plant_off, 0, -1);
		desk_rows(r, s + K.desks_after, r.depth() - K.aisle);
		return;
	}
	if (r.index == r.count - 1) { // the toilets
		const float back = r.depth() - K.wc_depth;
		r.wall(0, back, r.width() - K.wc_door, back);
		for (float u = K.toilet_from; u < r.width() - K.wc_depth; u += K.toilet_gap) r.put(FK_TOILET, u, r.depth() - K.toilet_s, 0, -1);
		r.put(FK_SINK, K.basin_a, back - K.basin_s, 0, -1);
		r.put(FK_SINK, K.basin_b, back - K.basin_s, 0, -1);
		desk_rows(r, K.desks_from, back - K.desks_back);
		return;
	}
	desk_rows(r, K.desks_from, r.depth() - K.aisle);
	r.put(FK_SHELF, r.width() - K.shelf_off, r.depth() - K.shelf_back, -1, 0);
	r.put(FK_PLANT, K.plant_off, r.depth() - K.plant_back, 0, -1);
}

void class_room(const RoomCell &r) {
	const InteriorRoomShape &K = r.p.data.shapes.rooms;
	r.put(FK_BLACKBOARD, K.board_u, r.depth() / 2, 1, 0);
	r.put(FK_DESK, K.teacher_u, r.depth() / 2, 1, 0);
	for (float u = K.pupil_u; u + K.pupil_s <= r.width(); u += K.pupil_gap)
		for (float s = K.pupil_s; s + K.pupil_end <= r.depth() - K.pupil_end; s += K.pupil_gap) {
			r.put(FK_DESK, u, s, -1, 0);
			r.put(FK_CHAIR, u + K.pupil_chair, s, -1, 0);
		}
}

void ward_room(const RoomCell &r) {
	const InteriorRoomShape &K = r.p.data.shapes.rooms;
	for (float s = K.bed_s; s + K.bed_end <= r.depth(); s += K.bed_gap) {
		r.put(FK_WARD_BED, K.bed_u, s, 1, 0);
		r.put(FK_CHAIR, K.visitor_u, s + K.bed_end, -1, 0);
		if (r.width() > K.two_rows) {
			r.put(FK_WARD_BED, r.width() - K.bed_u, s, -1, 0);
			r.put(FK_CHAIR, r.width() - K.visitor_u, s + K.bed_end, 1, 0);
		}
	}
	r.put(FK_PLANT, r.width() / 2, K.ward_plant, 0, 1);
}

void hotel_room(const RoomCell &r) {
	const InteriorRoomShape &K = r.p.data.shapes.rooms;
	const float bed = std::min(K.hotel_bed, r.depth() / 2 - K.hotel_bed_in);
	r.put(FK_BED, K.hotel_bed_u, bed, 1, 0);
	r.put(FK_TV, r.width() - K.hotel_tv, bed, -1, 0);
	r.put(FK_DESK, r.width() - K.hotel_desk_u, K.hotel_desk_s, 0, -1);
	r.put(FK_WARDROBE, r.width() - K.hotel_wardrobe_u, r.depth() - K.hotel_wardrobe_s, -1, 0);
	if (!r.corridor) return;
	r.wall(0, r.depth() - K.hotel_bath_s, r.width() - K.hotel_bath_door, r.depth() - K.hotel_bath_s);
	r.put(FK_TOILET, K.hotel_toilet_u, r.depth() - K.hotel_toilet_s, 1, 0);
	r.put(FK_BATH, K.hotel_tub_u, r.depth() - K.hotel_tub_s, 0, -1);
}

void office_desks(Plan &p) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	const float from = p.y_start();
	p.grid(FK_DESK, p.x0 + K.open_in, from + K.open_in, p.x1 - K.open_in, p.y1 - K.open_in, K.open_gx, K.open_gy);
	const std::vector<Furniture> desks = p.items;
	for (const Furniture &it : desks)
		if (it.kind == FK_DESK) p.put(FK_OFFICE_CHAIR, it.x, it.y - K.open_chair, PI_F);
	p.along_back(FK_PLANT, p.x0 + K.open_plant, p.x1 - K.open_plant, K.open_plant_gap);
}

void classrooms(Plan &p, float from) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	for (float x = p.x0; x + K.class_min <= p.x1; x += K.class_room) {
		const float x1 = std::min(p.x1, x + K.class_room);
		if (x > p.x0) p.wall_along(x, from, p.y1, from + K.class_door);
		p.put(FK_BLACKBOARD, (x + x1) / 2, p.y1 - K.board_back, PI_F);
		p.put(FK_DESK, (x + x1) / 2, p.y1 - K.teacher_back, PI_F);
		const size_t before = p.items.size();
		p.grid(FK_DESK, x + K.row_in, from + K.row_from, x1 - K.row_in, p.y1 - K.row_back, K.row_gx, K.row_gy, PI_F);
		const size_t after = p.items.size();
		for (size_t i = before; i < after; i++) {
			const Furniture it = p.items[i];
			p.put(FK_CHAIR, it.x, it.y - K.row_chair, PI_F);
		}
	}
}

void wards(Plan &p, float from) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	p.wall_across(from + K.ward_wall, p.x0, p.x1, p.cx());
	p.grid(FK_WARD_BED, p.x0 + K.ward_in, from + K.ward_from, p.x1 - K.ward_in, p.y1 - K.ward_in, K.ward_gx, K.ward_gy, PI_F);
}

void cells(Plan &p, float from) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	for (float x = p.x0; x + K.room_cell <= p.x1 + GRID_EPS; x += K.room_cell) {
		p.wall(x, p.y1 - K.room_cell, x, p.y1);
		p.put(FK_BARS, x + K.room_cell / 2, p.y1 - K.room_cell, 0);
		p.put(FK_SINGLE_BED, x + K.room_cell / 2, p.y1 - K.cell_bed, PI_F);
	}
	p.grid(FK_TABLE, p.x0 + K.cell_table_in, from + K.cell_table_in, p.x1 - K.cell_table_in, p.y1 - K.cell_table_to, K.cell_table_gx, K.room_cell);
}

void shop_floor(Plan &p) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	const float from = p.y_start(), back = back_room(p, K.shop_back);
	p.put(FK_COUNTER, p.cx(), back - K.counter_off, PI_F);
	p.along_side(FK_SHELF, true, from + K.shelf_from, back - K.shelf_to, K.shelf_gap);
	p.along_side(FK_SHELF, false, from + K.shelf_from, back - K.shelf_to, K.shelf_gap);
	p.grid(FK_SHELF, p.x0 + K.mid_in, from + K.shelf_from, p.x1 - K.mid_in, back - K.mid_to, K.mid_gx, K.mid_gy, RIGHT_ANGLE);
	if (back >= p.y1) return;
	for (float x = p.x0 + K.rack_from; x <= p.x1 - K.rack_end; x += K.rack_gap) p.put(FK_RACK, x, p.y1 - K.rack_off, face(0, -1));
	p.put(FK_DESK, p.x1 - K.stock_desk_u, back + K.stock_desk_s, face(0, 1));
	p.put(FK_TOILET, p.x1 - K.stock_toilet_u, p.y1 - K.stock_toilet_s, face(-1, 0));
}

void eating_floor(Plan &p, bool bar) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	const float back = back_room(p, bar ? K.bar_back : K.eat_back);
	if (bar) {
		p.put(FK_BAR_COUNTER, p.cx(), back - K.bar_counter, PI_F);
		p.grid(FK_SEAT, p.cx() - K.bar_seats_w, back - K.bar_seats_from, p.cx() + K.bar_seats_w, back - K.bar_seats_to, K.bar_seat_gx, K.bar_seat_gy, PI_F);
	} else {
		p.put(FK_COUNTER, p.cx(), back - K.eat_counter, PI_F);
	}
	if (back < p.y1) kitchen(p, back);
	dining(p, p.y0 + K.eat_from, back - (bar ? K.bar_to : K.eat_to));
}

void shops_around(Plan &p, float from) {
	const InteriorRoomShape &K = p.data.shapes.rooms;
	for (float x = p.x0; x + K.mall_unit <= p.x1 + GRID_EPS; x += K.mall_unit) {
		p.wall_along(x, p.y1 - K.mall_unit, p.y1, p.y1 - K.mall_door);
		p.wall_across(p.y1 - K.mall_unit, x, x + K.mall_unit, x + K.mall_unit / 2);
		p.put(FK_COUNTER, x + K.mall_unit / 2, p.y1 - K.mall_counter, PI_F);
		p.put(FK_SHELF, x + K.mall_shelf, p.y1 - K.mall_shelf_s, RIGHT_ANGLE);
	}
	p.grid(FK_SOFA, p.x0 + K.sofa_in, from + K.sofa_from, p.x1 - K.sofa_in, p.y1 - K.sofa_to, K.sofa_gx, K.sofa_gy);
	p.grid(FK_PLANT, p.x0 + K.mall_plant_in, from + K.mall_plant_from, p.x1 - K.mall_plant_in, from + K.sofa_from, K.mall_plant_gx, K.mall_plant_gy);
}

} // namespace city::b40
