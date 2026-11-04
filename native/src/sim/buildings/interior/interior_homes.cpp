// The floors people live on: a block of flats from its plan (the living room by the windows, the bedroom in the last bay,
// the bathroom in the corridor's corner), a house with its walls on the bays' boundaries and its stairs, and homes along a
// floor with no flat plan. Sizes of rooms are common Brazilian dwelling practice; the pieces' are interiors.json's.
#include "sim/buildings/interior/plan.h"

#include "sim/buildings/b40/gen.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF_WAY = 0.5f, DOOR_MARGIN = 0.8f, DOOR_SHARE = 0.8f, BED_DOOR_IN = 1.4f, BATH_D = 2.3f, BATH_DOOR = 0.9f;
constexpr float TV_U = 0.35f, ROOM_MID = 0.42f, SOFA_IN = 0.6f, SOFA_U = 3.4f, TABLE_S = 1.5f, CHAIR_U = 0.6f, CHAIR_S = 2.2f;
constexpr float FRIDGE_U = 0.5f, FRIDGE_S = 0.45f, STOVE_U = 1.3f, STOVE_S = 0.4f, SINK_U = 2.2f, PLANT_U = 0.4f, PLANT_S = 0.45f;
constexpr float BED_U = 1.15f, BED_S = 2.1f, WARDROBE_S = 2.65f, TOILET_U = 0.4f, TOILET_S = 0.5f, BATH_U = 0.5f, BATH_S = 1.f;
// A house.
constexpr float SMALL_HOUSE = 5.f, DESK_OFF = 0.6f, CHAIR_OFF = 1.4f, SHELF_OFF = 0.3f, RACK_OFF = 0.5f, SHELF_BACK = 0.35f;
constexpr float MID_MARGIN = 2.5f, STAIR_RUN = 3.7f, STAIR_X = 1.85f, STAIR_Y = 0.6f, KITCHEN_GAP = 0.2f, KITCHEN_END = 0.3f;
constexpr float SOFA_X = 3.6f, SOFA_END = 0.8f, ARMCHAIR_X = 2.2f, ARMCHAIR_Y = 0.8f, PLANT_END = 0.5f, BOOK_Y = 0.6f;
constexpr float FRIDGE_X = 0.5f, STOVE_X = 1.4f, SINK_X = 2.4f, KITCHEN_Y = 0.45f, KITCHEN_FIT = 0.4f, TABLE_BACK = 0.4f;
constexpr float SEAT_X = 0.6f, SEAT_Y = 0.9f, BATH_DEEP = 2.2f, ROOM_DOOR = 0.8f, BED_END = 1.15f, WARDROBE_X = 0.4f, WARDROBE_Y = 0.6f;
constexpr float BATH_X = 0.5f, TOILET_Y = 0.45f, HALL_D = 1.2f, DOOR_APART = 1.f, DESK_Y = 0.45f, SINK_Y = 0.4f, SINK_SHIFT = 0.3f;
constexpr float WARDROBE_MID = 0.4f, WARDROBE_FRONT = 1.f;
// Homes along a floor.
constexpr float UNIT_MIN = 3.f, UNIT_DOOR = 0.15f, UNIT_SPLIT = 0.55f, HOTEL_BED = 1.2f, HOTEL_WARDROBE_X = 0.9f, HOTEL_WARDROBE_Y = 1.f;
constexpr float HOTEL_DESK = 1.f, LIVE_DOOR = 0.3f, SOFA_SHARE = 0.45f, SOFA_BACK = 1.1f, TV_OFF = 0.4f, TABLE_SHARE = 0.75f;
constexpr float SEAT_OFF = 0.7f, U_FRIDGE = 0.5f, U_STOVE = 1.4f, U_SINK = 2.4f, U_KITCHEN = 0.4f, BED_SHARE = 0.35f, WIDE_UNIT = 6.f;
constexpr float BATH_WALL = 2.6f, BATH_WALL_DOOR = 0.8f, U_BATH_X = 0.6f, U_BATH_Y = 1.2f, U_TOILET_X = 1.8f, U_TOILET_Y = 0.5f;
constexpr float U_PLANT = 0.5f;
// A lobby.
constexpr float DESK_FRONT = 5.f, LOBBY_SOFA_X = 2.f, LOBBY_SOFA_Y = 3.f, LOBBY_PLANT = 1.f;

struct Home {
	Plan &p;
	const FlatCell &f;
	float px(float u, float s) const { return f.ox + f.tx * u + f.nx * s; }
	float py(float u, float s) const { return f.oy + f.ty * u + f.ny * s; }
	void line(float u0, float s0, float u1, float s1) const { p.wall(px(u0, s0), py(u0, s0), px(u1, s1), py(u1, s1)); }
	float facing(float du, float ds) const { return std::atan2(f.tx * du + f.nx * ds, -(f.ty * du + f.ny * ds)); }
	void put(FurnitureKind k, float u, float s, float angle) const { p.put(k, px(u, s), py(u, s), angle); }
};

void furnish_flat(const Home &h, bool corridor) {
	const float W = h.f.width, D = h.f.depth, living = W - h.f.bay;
	const float door = std::min(living * DOOR_SHARE, living - DOOR_MARGIN);
	if (corridor) {
		if (door - HALF_WAY > 0) h.line(0, D, door - HALF_WAY, D);
		h.line(door + HALF_WAY, D, W, D);
	}
	h.line(0, 0, 0, D);
	const float bed_door = D - BED_DOOR_IN;
	h.line(living, 0, living, bed_door - HALF_WAY);
	h.line(living, bed_door + HALF_WAY, living, D - BATH_D);
	h.line(living, D - BATH_D, living + BATH_DOOR - HALF_WAY, D - BATH_D);
	h.line(living + BATH_DOOR + HALF_WAY, D - BATH_D, W, D - BATH_D);
	h.line(living, D - BATH_D, living, D);
	const float away = h.facing(0, -1);
	h.put(FK_TV, TV_U, D * ROOM_MID, h.facing(1, 0));
	h.put(FK_SOFA, std::min(living - SOFA_IN, SOFA_U), D * ROOM_MID, h.facing(-1, 0));
	h.put(FK_TABLE, living / 2, TABLE_S, away);
	h.put(FK_CHAIR, living / 2 - CHAIR_U, CHAIR_S, away);
	h.put(FK_CHAIR, living / 2 + CHAIR_U, CHAIR_S, away);
	h.put(FK_FRIDGE, FRIDGE_U, D - FRIDGE_S, away);
	h.put(FK_STOVE, STOVE_U, D - STOVE_S, away);
	h.put(FK_SINK, SINK_U, D - STOVE_S, away);
	h.put(FK_PLANT, living - PLANT_U, PLANT_S, 0);
	h.put(FK_BED, W - BED_U, BED_S, h.facing(-1, 0));
	h.put(FK_WARDROBE, living + h.f.bay / 2, D - WARDROBE_S, away);
	h.put(FK_TOILET, W - TOILET_U, D - TOILET_S, h.facing(-1, 0));
	h.put(FK_BATH, living + BATH_U, D - BATH_S, away);
}

void house_ground(Plan &p, float mid, float stairs0, bool single, bool top) {
	const float split = p.y0 + p.depth() / 2, ly = (p.y0 + split) / 2;
	p.wall_across(split, p.x0, p.x1, mid);
	p.put(FK_TV, p.x0 + SHELF_OFF, ly, face(1, 0));
	p.put(FK_SOFA, std::min(p.x1 - SOFA_END, p.x0 + SOFA_X), ly, face(-1, 0));
	p.put(FK_ARMCHAIR, std::min(p.x1 - SOFA_END, p.x0 + ARMCHAIR_X), p.y0 + ARMCHAIR_Y, face(0, 1));
	p.put(FK_PLANT, p.x1 - PLANT_END, p.y0 + PLANT_END, 0);
	p.put(FK_BOOKSHELF, p.x1 - SHELF_OFF, ly + BOOK_Y, face(-1, 0));
	if (!single) p.put(top ? FK_STAIRWELL : FK_STAIRS, p.x1 - STAIR_X, p.y1 - STAIR_Y, face(-1, 0));
	const float kx1 = single ? mid - KITCHEN_END : std::min(mid, stairs0) - KITCHEN_GAP;
	const std::pair<FurnitureKind, float> kitchen[] = { { FK_FRIDGE, p.x0 + FRIDGE_X }, { FK_STOVE, p.x0 + STOVE_X }, { FK_SINK, p.x0 + SINK_X } };
	for (const auto &[kind, x] : kitchen)
		if (x < kx1 - KITCHEN_FIT) p.put(kind, x, p.y1 - KITCHEN_Y, face(0, -1));
	const float tx = (p.x0 + kx1) / 2, ty = (split + p.y1) / 2 - TABLE_BACK;
	p.put(FK_TABLE, tx, ty, 0);
	for (float dx : { -SEAT_X, SEAT_X })
		for (float dy : { -SEAT_Y, SEAT_Y }) p.put(FK_CHAIR, tx + dx, ty + dy, face(0, dy > 0 ? -1 : 1));
	if (!single) return;
	p.wall_along(mid, split, p.y1, split + ROOM_DOOR); // one storey: the bedroom and the bathroom at the back
	const float bath_y = p.y1 - BATH_DEEP;
	p.wall_across(bath_y, mid, p.x1, mid + BATH_DOOR);
	p.put(FK_BED, p.x1 - BED_END, (split + bath_y) / 2, face(-1, 0));
	p.put(FK_WARDROBE, mid + WARDROBE_X, (split + bath_y) / 2 + WARDROBE_Y, face(1, 0));
	p.put(FK_BATH, p.x1 - BATH_X, p.y1 - BATH_S, face(0, -1));
	p.put(FK_TOILET, mid + BATH_X, p.y1 - TOILET_Y, face(0, -1));
}

void house_upstairs(Plan &p, float mid, float stairs0, bool top) {
	const float split = p.y0 + p.depth() / 2, hall = split + HALL_D, by = (p.y0 + split) / 2;
	float from = p.x0;
	for (float d : { mid - DOOR_APART, mid + DOOR_APART }) {
		p.wall(from, split, d - HALF_WAY, split);
		from = d + HALF_WAY;
	}
	p.wall(from, split, p.x1, split);
	p.wall(mid, p.y0, mid, split);
	p.wall_across(hall, p.x0, stairs0, stairs0 - DOOR_APART);
	p.wall(stairs0, hall, stairs0, p.y1);
	p.put(FK_BED, p.x0 + BED_END, by, face(1, 0));
	p.put(FK_WARDROBE, mid - WARDROBE_MID, p.y0 + WARDROBE_FRONT, face(-1, 0));
	p.put(FK_BED, p.x1 - BED_END, by, face(-1, 0));
	p.put(FK_DESK, mid + DOOR_APART, p.y0 + DESK_Y, face(0, -1));
	p.put(FK_BATH, p.x0 + BATH_X, p.y1 - BATH_S, face(0, -1));
	p.put(FK_TOILET, stairs0 - BATH_X, p.y1 - TOILET_Y, face(0, -1));
	p.put(FK_SINK, (p.x0 + stairs0) / 2 + SINK_SHIFT, hall + SINK_Y, face(0, 1));
	p.put(top ? FK_STAIRWELL : FK_STAIRS, p.x1 - STAIR_X, p.y1 - STAIR_Y, face(-1, 0));
}

void one_unit(Plan &p, float ux0, float uy0, float ux1, float uy1, bool hotel) {
	const float w = ux1 - ux0, d = uy1 - uy0, mid = uy0 + d * UNIT_SPLIT;
	if (hotel) {
		p.put(FK_BED, ux0 + w / 2, uy1 - HOTEL_BED, PI_F);
		p.put(FK_WARDROBE, ux0 + HOTEL_WARDROBE_X, uy0 + HOTEL_WARDROBE_Y, 0);
		p.put(FK_DESK, ux1 - HOTEL_DESK, mid, RIGHT_ANGLE);
		return;
	}
	p.wall_across(mid, ux0, ux1, ux0 + w * LIVE_DOOR);
	const float table_y = uy0 + (mid - uy0) / 2;
	p.put(FK_SOFA, ux0 + w * SOFA_SHARE, mid - SOFA_BACK, 0);
	p.put(FK_TV, ux0 + w * SOFA_SHARE, uy0 + TV_OFF, PI_F);
	p.put(FK_TABLE, ux0 + w * TABLE_SHARE, table_y, 0);
	p.put(FK_CHAIR, ux0 + w * TABLE_SHARE - SEAT_X, table_y + SEAT_OFF, 0);
	p.put(FK_CHAIR, ux0 + w * TABLE_SHARE + SEAT_X, table_y + SEAT_OFF, 0);
	p.put(FK_FRIDGE, ux1 - U_FRIDGE, uy0 + U_FRIDGE, 0);
	p.put(FK_STOVE, ux1 - U_STOVE, uy0 + U_KITCHEN, 0);
	p.put(FK_SINK, ux1 - U_SINK, uy0 + U_KITCHEN, 0);
	p.put(FK_BED, ux0 + w * BED_SHARE, uy1 - HOTEL_BED, PI_F);
	p.put(FK_WARDROBE, ux0 + w * TABLE_SHARE, uy1 - U_KITCHEN, PI_F);
	if (w > WIDE_UNIT) {
		p.wall_along(ux1 - BATH_WALL, mid, uy1, mid + BATH_WALL_DOOR);
		p.put(FK_BATH, ux1 - U_BATH_X, uy1 - U_BATH_Y, 0);
		p.put(FK_TOILET, ux1 - U_TOILET_X, uy1 - U_TOILET_Y, PI_F);
	}
	p.put(FK_PLANT, ux0 + U_PLANT, uy0 + U_PLANT);
}
} // namespace

bool flat_floor(Plan &p) {
	const FlatPlan plan = flat_plan(p.v, 2);
	if (!plan.corridor && plan.flats.size() < 2) return false;
	for (const FlatCell &f : plan.flats) furnish_flat(Home{ p, f }, plan.corridor);
	return true;
}

void house_floor(Plan &p) {
	const Volume &v = p.v;
	const int k = p.level - v.base;
	for (const auto &[key, bay] : v.bays)
		if (bay == BAY_GARAGE_DOOR && int(key >> KEY_STOREY_SHIFT) == k) {
			p.put(FK_RACK, p.x0 + RACK_OFF, p.cy(), face(1, 0));
			p.put(FK_SHELF, p.cx(), p.y1 - SHELF_BACK, face(0, -1));
			return;
		}
	if (p.width() < SMALL_HOUSE || p.depth() < SMALL_HOUSE) {
		p.put(FK_DESK, p.cx(), p.y1 - DESK_OFF, face(0, -1));
		p.put(FK_OFFICE_CHAIR, p.cx(), p.y1 - CHAIR_OFF, face(0, 1));
		p.put(FK_BOOKSHELF, p.x0 + SHELF_OFF, p.cy(), face(1, 0));
		return;
	}
	const int n = std::max(1, bays_of(v, 0));
	const float bay = v.w / float(n);
	const float mid = std::min(p.x1 - MID_MARGIN, std::max(p.x0 + MID_MARGIN, v.x + float(std::max(1, n / 2)) * bay));
	const bool single = v.storeys == 1, top = p.level == v.base + v.storeys - 1;
	const float stairs0 = p.x1 - STAIR_RUN;
	if (p.level == v.base) house_ground(p, mid, stairs0, single, top);
	else house_upstairs(p, mid, stairs0, top);
}

void home_units(Plan &p, const std::vector<Space> &spaces, bool hotel) {
	const bool along_x = p.v.w >= p.v.d;
	for (size_t i = 0; i < spaces.size(); i++) {
		const Space &u = spaces[i];
		const float ux0 = std::max(p.x0, u.x), uy0 = std::max(p.y0, u.y), ux1 = std::min(p.x1, u.x + u.w), uy1 = std::min(p.y1, u.y + u.d);
		const float w = ux1 - ux0, d = uy1 - uy0;
		if (w < UNIT_MIN || d < UNIT_MIN) continue;
		if (i > 0) {
			if (along_x) p.wall_along(ux0, uy0, uy1, uy0 + d * UNIT_DOOR);
			else p.wall_across(uy0, ux0, ux1, ux0 + w * UNIT_DOOR);
		}
		one_unit(p, ux0, uy0, ux1, uy1, hotel);
	}
}

void lobby(Plan &p) {
	p.put(FK_COUNTER, p.cx(), p.y0 + DESK_FRONT, PI_F);
	p.put(FK_SOFA, p.x0 + LOBBY_SOFA_X, p.y0 + LOBBY_SOFA_Y, -RIGHT_ANGLE);
	p.put(FK_PLANT, p.x0 + LOBBY_PLANT, p.y0 + LOBBY_PLANT);
	p.put(FK_PLANT, p.x1 - LOBBY_PLANT, p.y0 + LOBBY_PLANT);
}

} // namespace city::b40
