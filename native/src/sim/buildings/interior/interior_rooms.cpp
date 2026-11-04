// Floors that are not homes: rooms off a corridor (offices, classrooms, wards, hotel rooms), open offices, shops with their
// stockroom, places to eat with their kitchen, cells and malls. Clearances are the ones a person needs to pass and sit.
#include "sim/buildings/interior/plan.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF_WAY = 0.5f, ROOM_DOOR_MIN = 0.9f, ROOM_DOOR_BACK = 1.2f;
// Offices.
constexpr float MEET_DEPTH = 4.f, MEET_SEAT = 0.9f, MEET_ROW = 0.8f, SHELF_OFF = 0.35f, PLANT_OFF = 0.4f, DESKS_AFTER = 3.f;
constexpr float WC_DEPTH = 1.6f, WC_DOOR = 1.4f, TOILET_GAP = 1.1f, TOILET_FROM = 0.6f, TOILET_S = 0.5f, BASIN_S = 0.4f, BASIN_A = 0.5f;
constexpr float BASIN_B = 1.8f, DESKS_FROM = 1.2f, DESKS_BACK = 2.2f, SHELF_BACK = 1.2f, PLANT_BACK = 0.6f;
constexpr float DESK_ROW = 2.6f, DESK_COL = 1.8f, DESK_U = 1.f, DESK_END = 0.8f, CHAIR_S = 0.9f, AISLE = 1.6f;
constexpr float OPEN_IN = 0.5f, OPEN_GX = 2.4f, OPEN_GY = 2.6f, OPEN_CHAIR = 0.75f, OPEN_PLANT = 1.f, OPEN_PLANT_GAP = 6.f;
// Classrooms and wards.
constexpr float BOARD_U = 0.2f, TEACHER_U = 1.6f, PUPIL_U = 3.2f, PUPIL_GAP = 1.8f, PUPIL_S = 1.2f, PUPIL_END = 0.6f, PUPIL_CHAIR = 0.85f;
constexpr float CLASS_ROOM = 9.f, CLASS_MIN = 5.f, CLASS_DOOR = 1.f, BOARD_BACK = 0.2f, TEACHER_BACK = 1.6f, ROW_IN = 0.5f, ROW_FROM = 1.f;
constexpr float ROW_BACK = 3.f, ROW_GX = 1.8f, ROW_GY = 1.7f, ROW_CHAIR = 0.7f;
constexpr float BED_S = 1.4f, BED_GAP = 2.4f, BED_U = 1.25f, BED_END = 1.f, VISITOR_U = 2.55f, TWO_ROWS = 5.f, WARD_PLANT = 0.4f;
constexpr float WARD_WALL = 0.5f, WARD_IN = 0.5f, WARD_FROM = 1.f, WARD_GX = 2.2f, WARD_GY = 3.2f;
// Hotel rooms.
constexpr float HOTEL_BED = 3.f, HOTEL_BED_IN = 0.5f, HOTEL_BED_U = 1.1f, HOTEL_TV = 0.3f, HOTEL_DESK_U = 0.8f, HOTEL_DESK_S = 0.6f;
constexpr float HOTEL_WARDROBE_U = 0.35f, HOTEL_WARDROBE_S = 3.f, HOTEL_BATH_S = 2.2f, HOTEL_BATH_DOOR = 1.6f, HOTEL_TOILET_U = 0.5f;
constexpr float HOTEL_TOILET_S = 0.5f, HOTEL_TUB_U = 1.6f, HOTEL_TUB_S = 1.f;
// Shops and places to eat.
constexpr float BACK_NEED = 6.f, BACK_DOOR = 1.6f, SHOP_BACK = 4.f, EAT_BACK = 4.5f, BAR_BACK = 3.5f;
constexpr float COUNTER_OFF = 1.3f, SHELF_FROM = 1.f, SHELF_TO = 1.5f, SHELF_GAP = 2.2f, MID_IN = 2.f, MID_TO = 2.6f, MID_GX = 3.f, MID_GY = 2.4f;
constexpr float RACK_FROM = 1.f, RACK_GAP = 2.6f, RACK_END = 2.6f, RACK_OFF = 0.9f, STOCK_DESK_U = 1.3f, STOCK_DESK_S = 1.2f;
constexpr float STOCK_TOILET_U = 0.4f, STOCK_TOILET_S = 0.5f, EAT_COUNTER = 1.2f, EAT_FROM = 0.6f, EAT_TO = 2.4f, BAR_COUNTER = 1.4f;
constexpr float BAR_SEATS_W = 1.8f, BAR_SEATS_FROM = 2.7f, BAR_SEATS_TO = 2.2f, BAR_SEAT_GX = 0.9f, BAR_SEAT_GY = 0.5f, BAR_TO = 3.4f;
constexpr float COOK_FROM = 0.8f, COOK_END = 2.5f, COOK_GAP = 1.f, COOK_S = 0.45f, COOK_TABLE = 0.3f, COOK_TABLE_SCALE = 0.8f;
constexpr float DINE_Y = 1.f, DINE_X = 1.4f, DINE_ROW = 2.6f, DINE_COL = 2.8f, DINE_SCALE = 0.6f, DINE_SEAT = 0.9f;
constexpr int COOK_KINDS = 3;
// Cells and malls.
constexpr float CELL = 3.f, CELL_BED = 1.1f, CELL_TABLE_IN = 1.f, CELL_TABLE_TO = 5.f, CELL_TABLE_GX = 3.5f, GRID_EPS = 1e-6f;
constexpr float MALL_UNIT = 8.f, MALL_DOOR = 4.f, MALL_COUNTER = 1.5f, MALL_SHELF = 1.f, MALL_SHELF_S = 5.f, SOFA_IN = 4.f, SOFA_FROM = 3.f;
constexpr float SOFA_TO = 11.f, SOFA_GX = 8.f, SOFA_GY = 6.f, MALL_PLANT_IN = 2.f, MALL_PLANT_FROM = 2.f, MALL_PLANT_GX = 6.f, MALL_PLANT_GY = 1.f;

void desk_rows(const RoomCell &r, float s0, float s1) {
	for (float s = s0; s + DESK_END <= s1; s += DESK_ROW)
		for (float u = DESK_U; u + DESK_END <= r.width(); u += DESK_COL) {
			r.put(FK_DESK, u, s, 0, -1);
			r.put(FK_OFFICE_CHAIR, u, s + CHAIR_S, 0, -1);
		}
}

// A room across the back of the floor, behind a wall with a door near its end: where the wall stands, or y1 with no room.
float back_room(Plan &p, float depth) {
	if (p.y1 - p.y0 < depth + BACK_NEED) return p.y1;
	const float y = p.y1 - depth;
	p.wall_across(y, p.x0, p.x1, p.x1 - BACK_DOOR);
	return y;
}

void kitchen(Plan &p, float wall) {
	constexpr FurnitureKind COOK[COOK_KINDS] = { FK_STOVE, FK_SINK, FK_FRIDGE };
	int k = 0;
	for (float x = p.x0 + COOK_FROM; x <= p.x1 - COOK_END; x += COOK_GAP) p.put(COOK[k++ % COOK_KINDS], x, p.y1 - COOK_S, face(0, -1));
	p.put(FK_TABLE, p.cx(), (wall + p.y1) / 2 - COOK_TABLE, 0, COOK_TABLE_SCALE);
}

void dining(Plan &p, float from, float to) {
	for (float y = from + DINE_Y; y <= to - DINE_Y; y += DINE_ROW)
		for (float x = p.x0 + DINE_X; x <= p.x1 - DINE_X; x += DINE_COL) {
			if (!p.put(FK_TABLE, x, y, 0, DINE_SCALE)) continue;
			p.put(FK_CHAIR, x - DINE_SEAT, y, RIGHT_ANGLE);
			p.put(FK_CHAIR, x + DINE_SEAT, y, -RIGHT_ANGLE);
		}
}
} // namespace

bool rooms_floor(Plan &p, int group_bays, RoomFill fill) {
	const FlatPlan plan = flat_plan(p.v, group_bays);
	if (!plan.corridor && plan.flats.size() < 2) return false;
	for (size_t i = 0; i < plan.flats.size(); i++) {
		const RoomCell r{ p, plan.flats[i], int(i), int(plan.flats.size()), plan.corridor };
		const float W = r.width(), D = r.depth(), door = std::max(ROOM_DOOR_MIN, W - ROOM_DOOR_BACK);
		if (plan.corridor) {
			if (door - HALF_WAY > 0) r.wall(0, D, door - HALF_WAY, D);
			if (door + HALF_WAY < W) r.wall(door + HALF_WAY, D, W, D);
		}
		r.wall(0, 0, 0, D);
		fill(r);
	}
	return true;
}

void office_room(const RoomCell &r) {
	if (r.index == 0) { // the meeting room
		const float s = std::min(r.depth() / 2, MEET_DEPTH);
		r.put(FK_TABLE, r.width() / 2, s, 0, -1);
		for (float side : { -1.f, 1.f })
			for (float k : { -1.f, 0.f, 1.f }) r.put(FK_CHAIR, r.width() / 2 + k * MEET_SEAT, s + side * MEET_ROW, 0, -side);
		r.put(FK_SHELF, SHELF_OFF, s, 1, 0);
		r.put(FK_PLANT, PLANT_OFF, PLANT_OFF, 0, -1);
		desk_rows(r, s + DESKS_AFTER, r.depth() - AISLE);
		return;
	}
	if (r.index == r.count - 1) { // the toilets
		const float back = r.depth() - WC_DEPTH;
		r.wall(0, back, r.width() - WC_DOOR, back);
		for (float u = TOILET_FROM; u < r.width() - WC_DEPTH; u += TOILET_GAP) r.put(FK_TOILET, u, r.depth() - TOILET_S, 0, -1);
		r.put(FK_SINK, BASIN_A, back - BASIN_S, 0, -1);
		r.put(FK_SINK, BASIN_B, back - BASIN_S, 0, -1);
		desk_rows(r, DESKS_FROM, back - DESKS_BACK);
		return;
	}
	desk_rows(r, DESKS_FROM, r.depth() - AISLE);
	r.put(FK_SHELF, r.width() - SHELF_OFF, r.depth() - SHELF_BACK, -1, 0);
	r.put(FK_PLANT, PLANT_OFF, r.depth() - PLANT_BACK, 0, -1);
}

void class_room(const RoomCell &r) {
	r.put(FK_BLACKBOARD, BOARD_U, r.depth() / 2, 1, 0);
	r.put(FK_DESK, TEACHER_U, r.depth() / 2, 1, 0);
	for (float u = PUPIL_U; u + PUPIL_S <= r.width(); u += PUPIL_GAP)
		for (float s = PUPIL_S; s + PUPIL_END <= r.depth() - PUPIL_END; s += PUPIL_GAP) {
			r.put(FK_DESK, u, s, -1, 0);
			r.put(FK_CHAIR, u + PUPIL_CHAIR, s, -1, 0);
		}
}

void ward_room(const RoomCell &r) {
	for (float s = BED_S; s + BED_END <= r.depth(); s += BED_GAP) {
		r.put(FK_WARD_BED, BED_U, s, 1, 0);
		r.put(FK_CHAIR, VISITOR_U, s + BED_END, -1, 0);
		if (r.width() > TWO_ROWS) {
			r.put(FK_WARD_BED, r.width() - BED_U, s, -1, 0);
			r.put(FK_CHAIR, r.width() - VISITOR_U, s + BED_END, 1, 0);
		}
	}
	r.put(FK_PLANT, r.width() / 2, WARD_PLANT, 0, 1);
}

void hotel_room(const RoomCell &r) {
	const float bed = std::min(HOTEL_BED, r.depth() / 2 - HOTEL_BED_IN);
	r.put(FK_BED, HOTEL_BED_U, bed, 1, 0);
	r.put(FK_TV, r.width() - HOTEL_TV, bed, -1, 0);
	r.put(FK_DESK, r.width() - HOTEL_DESK_U, HOTEL_DESK_S, 0, -1);
	r.put(FK_WARDROBE, r.width() - HOTEL_WARDROBE_U, r.depth() - HOTEL_WARDROBE_S, -1, 0);
	if (!r.corridor) return;
	r.wall(0, r.depth() - HOTEL_BATH_S, r.width() - HOTEL_BATH_DOOR, r.depth() - HOTEL_BATH_S);
	r.put(FK_TOILET, HOTEL_TOILET_U, r.depth() - HOTEL_TOILET_S, 1, 0);
	r.put(FK_BATH, HOTEL_TUB_U, r.depth() - HOTEL_TUB_S, 0, -1);
}

void office_desks(Plan &p) {
	const float from = p.y_start();
	p.grid(FK_DESK, p.x0 + OPEN_IN, from + OPEN_IN, p.x1 - OPEN_IN, p.y1 - OPEN_IN, OPEN_GX, OPEN_GY);
	const std::vector<Furniture> desks = p.items;
	for (const Furniture &it : desks)
		if (it.kind == FK_DESK) p.put(FK_OFFICE_CHAIR, it.x, it.y - OPEN_CHAIR, PI_F);
	p.along_back(FK_PLANT, p.x0 + OPEN_PLANT, p.x1 - OPEN_PLANT, OPEN_PLANT_GAP);
}

void classrooms(Plan &p, float from) {
	for (float x = p.x0; x + CLASS_MIN <= p.x1; x += CLASS_ROOM) {
		const float x1 = std::min(p.x1, x + CLASS_ROOM);
		if (x > p.x0) p.wall_along(x, from, p.y1, from + CLASS_DOOR);
		p.put(FK_BLACKBOARD, (x + x1) / 2, p.y1 - BOARD_BACK, PI_F);
		p.put(FK_DESK, (x + x1) / 2, p.y1 - TEACHER_BACK, PI_F);
		const size_t before = p.items.size();
		p.grid(FK_DESK, x + ROW_IN, from + ROW_FROM, x1 - ROW_IN, p.y1 - ROW_BACK, ROW_GX, ROW_GY, PI_F);
		const size_t after = p.items.size();
		for (size_t i = before; i < after; i++) {
			const Furniture it = p.items[i];
			p.put(FK_CHAIR, it.x, it.y - ROW_CHAIR, PI_F);
		}
	}
}

void wards(Plan &p, float from) {
	p.wall_across(from + WARD_WALL, p.x0, p.x1, p.cx());
	p.grid(FK_WARD_BED, p.x0 + WARD_IN, from + WARD_FROM, p.x1 - WARD_IN, p.y1 - WARD_IN, WARD_GX, WARD_GY, PI_F);
}

void cells(Plan &p, float from) {
	for (float x = p.x0; x + CELL <= p.x1 + GRID_EPS; x += CELL) {
		p.wall(x, p.y1 - CELL, x, p.y1);
		p.put(FK_BARS, x + CELL / 2, p.y1 - CELL, 0);
		p.put(FK_SINGLE_BED, x + CELL / 2, p.y1 - CELL_BED, PI_F);
	}
	p.grid(FK_TABLE, p.x0 + CELL_TABLE_IN, from + CELL_TABLE_IN, p.x1 - CELL_TABLE_IN, p.y1 - CELL_TABLE_TO, CELL_TABLE_GX, CELL);
}

void shop_floor(Plan &p) {
	const float from = p.y_start(), back = back_room(p, SHOP_BACK);
	p.put(FK_COUNTER, p.cx(), back - COUNTER_OFF, PI_F);
	p.along_side(FK_SHELF, true, from + SHELF_FROM, back - SHELF_TO, SHELF_GAP);
	p.along_side(FK_SHELF, false, from + SHELF_FROM, back - SHELF_TO, SHELF_GAP);
	p.grid(FK_SHELF, p.x0 + MID_IN, from + SHELF_FROM, p.x1 - MID_IN, back - MID_TO, MID_GX, MID_GY, RIGHT_ANGLE);
	if (back >= p.y1) return;
	for (float x = p.x0 + RACK_FROM; x <= p.x1 - RACK_END; x += RACK_GAP) p.put(FK_RACK, x, p.y1 - RACK_OFF, face(0, -1));
	p.put(FK_DESK, p.x1 - STOCK_DESK_U, back + STOCK_DESK_S, face(0, 1));
	p.put(FK_TOILET, p.x1 - STOCK_TOILET_U, p.y1 - STOCK_TOILET_S, face(-1, 0));
}

void eating_floor(Plan &p, bool bar) {
	const float back = back_room(p, bar ? BAR_BACK : EAT_BACK);
	if (bar) {
		p.put(FK_BAR_COUNTER, p.cx(), back - BAR_COUNTER, PI_F);
		p.grid(FK_SEAT, p.cx() - BAR_SEATS_W, back - BAR_SEATS_FROM, p.cx() + BAR_SEATS_W, back - BAR_SEATS_TO, BAR_SEAT_GX, BAR_SEAT_GY, PI_F);
	} else {
		p.put(FK_COUNTER, p.cx(), back - EAT_COUNTER, PI_F);
	}
	if (back < p.y1) kitchen(p, back);
	dining(p, p.y0 + EAT_FROM, back - (bar ? BAR_TO : EAT_TO));
}

void shops_around(Plan &p, float from) {
	for (float x = p.x0; x + MALL_UNIT <= p.x1 + GRID_EPS; x += MALL_UNIT) {
		p.wall_along(x, p.y1 - MALL_UNIT, p.y1, p.y1 - MALL_DOOR);
		p.wall_across(p.y1 - MALL_UNIT, x, x + MALL_UNIT, x + MALL_UNIT / 2);
		p.put(FK_COUNTER, x + MALL_UNIT / 2, p.y1 - MALL_COUNTER, PI_F);
		p.put(FK_SHELF, x + MALL_SHELF, p.y1 - MALL_SHELF_S, RIGHT_ANGLE);
	}
	p.grid(FK_SOFA, p.x0 + SOFA_IN, from + SOFA_FROM, p.x1 - SOFA_IN, p.y1 - SOFA_TO, SOFA_GX, SOFA_GY);
	p.grid(FK_PLANT, p.x0 + MALL_PLANT_IN, from + MALL_PLANT_FROM, p.x1 - MALL_PLANT_IN, from + SOFA_FROM, MALL_PLANT_GX, MALL_PLANT_GY);
}

} // namespace city::b40
