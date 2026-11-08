// The floors people live on: a block of flats from its plan (the living room by the windows, the bedroom in the last bay,
// the bathroom in the corridor's corner), a house with its walls on the bays' boundaries and its stairs, and homes along a
// floor with no flat plan. Sizes of rooms are common Brazilian dwelling practice; the pieces' are interiors.json's.
#include "sim/buildings/interior/plan.h"

#include "sim/buildings/b40/gen.h"

#include <algorithm>

namespace city::b40 {

namespace {
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
	const InteriorHomeShape &K = h.p.data.shapes.homes;
	const float W = h.f.width, D = h.f.depth, living = W - h.f.bay;
	const float door = std::min(living * K.door_share, living - K.door_margin);
	if (corridor) {
		if (door - HALF_DOOR > 0) h.line(0, D, door - HALF_DOOR, D);
		h.line(door + HALF_DOOR, D, W, D);
	}
	h.line(0, 0, 0, D);
	const float bed_door = D - K.bed_door_in;
	h.line(living, 0, living, bed_door - HALF_DOOR);
	h.line(living, bed_door + HALF_DOOR, living, D - K.bath_d);
	h.line(living, D - K.bath_d, living + K.bath_door - HALF_DOOR, D - K.bath_d);
	h.line(living + K.bath_door + HALF_DOOR, D - K.bath_d, W, D - K.bath_d);
	h.line(living, D - K.bath_d, living, D);
	const float away = h.facing(0, -1);
	h.put(FK_TV, K.tv_u, D * K.room_mid, h.facing(1, 0));
	h.put(FK_SOFA, std::min(living - K.sofa_in, K.sofa_u), D * K.room_mid, h.facing(-1, 0));
	h.put(FK_TABLE, living / 2, K.table_s, away);
	h.put(FK_CHAIR, living / 2 - K.chair_u, K.chair_s, away);
	h.put(FK_CHAIR, living / 2 + K.chair_u, K.chair_s, away);
	h.put(FK_FRIDGE, K.fridge_u, D - K.fridge_s, away);
	h.put(FK_STOVE, K.stove_u, D - K.stove_s, away);
	h.put(FK_SINK, K.sink_u, D - K.stove_s, away);
	h.put(FK_PLANT, living - K.plant_u, K.plant_s, 0);
	h.put(FK_BED, W - K.bed_u, K.bed_s, h.facing(-1, 0));
	h.put(FK_WARDROBE, living + h.f.bay / 2, D - K.wardrobe_s, away);
	h.put(FK_TOILET, W - K.toilet_u, D - K.toilet_s, h.facing(-1, 0));
	h.put(FK_BATH, living + K.bath_u, D - K.bath_s, away);
}

void house_ground(Plan &p, float mid, float stairs0, bool single, bool top) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	const float split = p.y0 + p.depth() / 2, ly = (p.y0 + split) / 2;
	p.wall_across(split, p.x0, p.x1, mid);
	p.put(FK_TV, p.x0 + K.shelf_off, ly, face(1, 0));
	p.put(FK_SOFA, std::min(p.x1 - K.sofa_end, p.x0 + K.sofa_x), ly, face(-1, 0));
	p.put(FK_ARMCHAIR, std::min(p.x1 - K.sofa_end, p.x0 + K.armchair_x), p.y0 + K.armchair_y, face(0, 1));
	p.put(FK_PLANT, p.x1 - K.plant_end, p.y0 + K.plant_end, 0);
	p.put(FK_BOOKSHELF, p.x1 - K.shelf_off, ly + K.book_y, face(-1, 0));
	if (!single) p.put(top ? FK_STAIRWELL : FK_STAIRS, p.x1 - K.stair_x, p.y1 - K.stair_y, face(-1, 0));
	const float kx1 = single ? mid - K.kitchen_end : std::min(mid, stairs0) - K.kitchen_gap;
	const std::pair<FurnitureKind, float> kitchen[] = { { FK_FRIDGE, p.x0 + K.fridge_x }, { FK_STOVE, p.x0 + K.stove_x }, { FK_SINK, p.x0 + K.sink_x } };
	for (const auto &[kind, x] : kitchen)
		if (x < kx1 - K.kitchen_fit) p.put(kind, x, p.y1 - K.kitchen_y, face(0, -1));
	const float tx = (p.x0 + kx1) / 2, ty = (split + p.y1) / 2 - K.table_back;
	p.put(FK_TABLE, tx, ty, 0);
	for (float dx : { -K.seat_x, K.seat_x })
		for (float dy : { -K.seat_y, K.seat_y }) p.put(FK_CHAIR, tx + dx, ty + dy, face(0, dy > 0 ? -1 : 1));
	if (!single) return;
	p.wall_along(mid, split, p.y1, split + K.room_door); // one storey: the bedroom and the bathroom at the back
	const float bath_y = p.y1 - K.bath_deep;
	p.wall_across(bath_y, mid, p.x1, mid + K.bath_door);
	p.put(FK_BED, p.x1 - K.bed_end, (split + bath_y) / 2, face(-1, 0));
	p.put(FK_WARDROBE, mid + K.wardrobe_x, (split + bath_y) / 2 + K.wardrobe_y, face(1, 0));
	p.put(FK_BATH, p.x1 - K.bath_x, p.y1 - K.bath_s, face(0, -1));
	p.put(FK_TOILET, mid + K.bath_x, p.y1 - K.toilet_y, face(0, -1));
}

void house_upstairs(Plan &p, float mid, float stairs0, bool top) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	const float split = p.y0 + p.depth() / 2, hall = split + K.hall_d, by = (p.y0 + split) / 2;
	float from = p.x0;
	for (float d : { mid - K.door_apart, mid + K.door_apart }) {
		p.wall(from, split, d - HALF_DOOR, split);
		from = d + HALF_DOOR;
	}
	p.wall(from, split, p.x1, split);
	p.wall(mid, p.y0, mid, split);
	p.wall_across(hall, p.x0, stairs0, stairs0 - K.door_apart);
	p.wall(stairs0, hall, stairs0, p.y1);
	p.put(FK_BED, p.x0 + K.bed_end, by, face(1, 0));
	p.put(FK_WARDROBE, mid - K.wardrobe_mid, p.y0 + K.wardrobe_front, face(-1, 0));
	p.put(FK_BED, p.x1 - K.bed_end, by, face(-1, 0));
	p.put(FK_DESK, mid + K.door_apart, p.y0 + K.desk_y, face(0, -1));
	p.put(FK_BATH, p.x0 + K.bath_x, p.y1 - K.bath_s, face(0, -1));
	p.put(FK_TOILET, stairs0 - K.bath_x, p.y1 - K.toilet_y, face(0, -1));
	p.put(FK_SINK, (p.x0 + stairs0) / 2 + K.sink_shift, hall + K.sink_y, face(0, 1));
	p.put(top ? FK_STAIRWELL : FK_STAIRS, p.x1 - K.stair_x, p.y1 - K.stair_y, face(-1, 0));
}

void one_unit(Plan &p, float ux0, float uy0, float ux1, float uy1, bool hotel) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	const float w = ux1 - ux0, d = uy1 - uy0, mid = uy0 + d * K.unit_split;
	if (hotel) {
		p.put(FK_BED, ux0 + w / 2, uy1 - K.hotel_bed, PI_F);
		p.put(FK_WARDROBE, ux0 + K.hotel_wardrobe_x, uy0 + K.hotel_wardrobe_y, 0);
		p.put(FK_DESK, ux1 - K.hotel_desk, mid, RIGHT_ANGLE);
		return;
	}
	p.wall_across(mid, ux0, ux1, ux0 + w * K.live_door);
	const float table_y = uy0 + (mid - uy0) / 2;
	p.put(FK_SOFA, ux0 + w * K.sofa_share, mid - K.sofa_back, 0);
	p.put(FK_TV, ux0 + w * K.sofa_share, uy0 + K.tv_off, PI_F);
	p.put(FK_TABLE, ux0 + w * K.table_share, table_y, 0);
	p.put(FK_CHAIR, ux0 + w * K.table_share - K.seat_x, table_y + K.seat_off, 0);
	p.put(FK_CHAIR, ux0 + w * K.table_share + K.seat_x, table_y + K.seat_off, 0);
	p.put(FK_FRIDGE, ux1 - K.u_fridge, uy0 + K.u_fridge, 0);
	p.put(FK_STOVE, ux1 - K.u_stove, uy0 + K.u_kitchen, 0);
	p.put(FK_SINK, ux1 - K.u_sink, uy0 + K.u_kitchen, 0);
	p.put(FK_BED, ux0 + w * K.bed_share, uy1 - K.hotel_bed, PI_F);
	p.put(FK_WARDROBE, ux0 + w * K.table_share, uy1 - K.u_kitchen, PI_F);
	if (w > K.wide_unit) {
		p.wall_along(ux1 - K.bath_wall, mid, uy1, mid + K.bath_wall_door);
		p.put(FK_BATH, ux1 - K.u_bath_x, uy1 - K.u_bath_y, 0);
		p.put(FK_TOILET, ux1 - K.u_toilet_x, uy1 - K.u_toilet_y, PI_F);
	}
	p.put(FK_PLANT, ux0 + K.u_plant, uy0 + K.u_plant);
}
} // namespace

bool flat_floor(Plan &p) {
	const FlatPlan plan = flat_plan(p.data, p.v, 2);
	if (!plan.corridor && plan.flats.size() < 2) return false;
	for (const FlatCell &f : plan.flats) furnish_flat(Home{ p, f }, plan.corridor);
	return true;
}

void house_floor(Plan &p) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	const Volume &v = p.v;
	const int k = p.level - v.base;
	for (const auto &[key, bay] : v.bays)
		if (bay == BAY_GARAGE_DOOR && int(key >> KEY_STOREY_SHIFT) == k) {
			p.put(FK_RACK, p.x0 + K.rack_off, p.cy(), face(1, 0));
			p.put(FK_SHELF, p.cx(), p.y1 - K.shelf_back, face(0, -1));
			return;
		}
	if (p.width() < K.small_house || p.depth() < K.small_house) {
		p.put(FK_DESK, p.cx(), p.y1 - K.desk_off, face(0, -1));
		p.put(FK_OFFICE_CHAIR, p.cx(), p.y1 - K.chair_off, face(0, 1));
		p.put(FK_BOOKSHELF, p.x0 + K.shelf_off, p.cy(), face(1, 0));
		return;
	}
	const int n = std::max(1, bays_of(v, 0));
	const float bay = v.w / float(n);
	const float mid = std::min(p.x1 - K.mid_margin, std::max(p.x0 + K.mid_margin, v.x + float(std::max(1, n / 2)) * bay));
	const bool single = v.storeys == 1, top = p.level == v.base + v.storeys - 1;
	const float stairs0 = p.x1 - K.stair_run;
	if (p.level == v.base) house_ground(p, mid, stairs0, single, top);
	else house_upstairs(p, mid, stairs0, top);
}

void home_units(Plan &p, const std::vector<Space> &spaces, bool hotel) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	const bool along_x = p.v.w >= p.v.d;
	for (size_t i = 0; i < spaces.size(); i++) {
		const Space &u = spaces[i];
		const float ux0 = std::max(p.x0, u.x), uy0 = std::max(p.y0, u.y), ux1 = std::min(p.x1, u.x + u.w), uy1 = std::min(p.y1, u.y + u.d);
		const float w = ux1 - ux0, d = uy1 - uy0;
		if (w < K.unit_min || d < K.unit_min) continue;
		if (i > 0) {
			if (along_x) p.wall_along(ux0, uy0, uy1, uy0 + d * K.unit_door);
			else p.wall_across(uy0, ux0, ux1, ux0 + w * K.unit_door);
		}
		one_unit(p, ux0, uy0, ux1, uy1, hotel);
	}
}

void lobby(Plan &p) {
	const InteriorHomeShape &K = p.data.shapes.homes;
	p.put(FK_COUNTER, p.cx(), p.y0 + K.desk_front, PI_F);
	p.put(FK_SOFA, p.x0 + K.lobby_sofa_x, p.y0 + K.lobby_sofa_y, -RIGHT_ANGLE);
	p.put(FK_PLANT, p.x0 + K.lobby_plant, p.y0 + K.lobby_plant);
	p.put(FK_PLANT, p.x1 - K.lobby_plant, p.y0 + K.lobby_plant);
}

} // namespace city::b40
