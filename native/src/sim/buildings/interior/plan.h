// One block's floor while it is furnished: where things may stand and what stands (interior.cpp and the floors of
// interior_homes.cpp and interior_rooms.cpp). Internal to sim/buildings/interior.
#pragma once

#include "sim/buildings/interior/interior.h"

#include <array>
#include <cmath>
#include <functional>
#include <unordered_map>

namespace city::b40 {

constexpr float PI_F = 3.14159265f, RIGHT_ANGLE = PI_F / 2;
// The way a piece faces, as a direction in the floor's own axes (+y away from the street).
inline float face(float dx, float dy) { return std::atan2(dx, -dy); }

// One home on a residential floor, and how it lies: its facade corner, `t` along the facade, `n` from it inwards.
struct FlatCell {
	float x = 0, y = 0, w = 0, d = 0, ox = 0, oy = 0, tx = 0, ty = 0, nx = 0, ny = 0;
	float width = 0, depth = 0, bay = 0;
};
struct FlatPlan {
	bool corridor = false;
	std::vector<FlatCell> flats;
};
// A floor of one block as homes of `group_bays` bays: a corridor down its long axis when it is deep enough for homes
// both sides.
FlatPlan flat_plan(const Volume &v, int group_bays);

class Plan {
public:
	Plan(const Data &data, const Body &b, const Volume &v, int level, float module);
	const Data &data;
	const Body &b;
	const Volume &v;
	int level;
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	std::vector<Furniture> items;
	std::vector<Partition> walls;
	float width() const { return x1 - x0; }
	float depth() const { return y1 - y0; }
	float cx() const { return (x0 + x1) / 2; }
	float cy() const { return (y0 + y1) / 2; }
	float y_start() const; // past the ground floor's front
	bool free(float x, float y, float w, float d, float angle) const;
	bool put(FurnitureKind kind, float x, float y, float angle = 0, float scale = 1);
	void grid(FurnitureKind kind, float ax, float ay, float bx, float by, float gx, float gy, float angle = 0);
	void along_back(FurnitureKind kind, float from, float to, float gap);
	void along_side(FurnitureKind kind, bool left, float from, float to, float gap);
	void wall(float ax, float ay, float bx, float by) { walls.push_back({ ax, ay, bx, by }); }
	void wall_across(float y, float ax, float bx, float door);
	void wall_along(float x, float ay, float by, float door);

private:
	bool inside(float x, float y) const;
	std::vector<Point2> ring_;
	std::vector<std::array<float, 4>> cores_;
	std::unordered_map<int64_t, std::vector<int>> filed_; // items by the cells their padded boxes cover
};

// One room off a corridor, in its own axes: u along the facade, s in from it.
struct RoomCell {
	Plan &p;
	const FlatCell &f;
	int index = 0, count = 0;
	bool corridor = false;
	float width() const { return f.width; }
	float depth() const { return f.depth; }
	void put(FurnitureKind kind, float u, float s, float du, float ds) const;
	void wall(float u0, float s0, float u1, float s1) const;
};
using RoomFill = void (*)(const RoomCell &);

// interior_rooms.cpp: a floor as rooms off a corridor (false when the block is no such floor), and the floors of a trade.
bool rooms_floor(Plan &p, int group_bays, RoomFill fill);
void office_room(const RoomCell &r);
void class_room(const RoomCell &r);
void ward_room(const RoomCell &r);
void hotel_room(const RoomCell &r);
void office_desks(Plan &p);
void classrooms(Plan &p, float from);
void wards(Plan &p, float from);
void cells(Plan &p, float from);
void shop_floor(Plan &p);
void eating_floor(Plan &p, bool bar);
void shops_around(Plan &p, float from);
// interior_homes.cpp
bool flat_floor(Plan &p);
void house_floor(Plan &p);
void home_units(Plan &p, const std::vector<Space> &spaces, bool hotel);
void lobby(Plan &p);

} // namespace city::b40
