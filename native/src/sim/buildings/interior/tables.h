// What the interiors are made from, read once from data/render/interiors.json (api/interiors_loader.cpp): the size of each
// piece of furniture, which pieces read as a box through a window, and the plain floors as recipes of steps. The floors
// with a plan of their own (flats, houses, rooms off a corridor) are interior_homes.cpp's and interior_rooms.cpp's.
#pragma once

#include <cstdint>
#include <vector>

namespace city::b40 {

enum FurnitureKind : uint8_t { FK_BED, FK_SINGLE_BED, FK_SOFA, FK_ARMCHAIR, FK_TABLE, FK_CHAIR, FK_DESK, FK_OFFICE_CHAIR, FK_SHELF,
	FK_BOOKSHELF, FK_COUNTER, FK_FRIDGE, FK_STOVE, FK_SINK, FK_TOILET, FK_BATH, FK_WARDROBE, FK_TV, FK_PLANT, FK_PEW, FK_SEAT,
	FK_WARD_BED, FK_BARS, FK_SCREEN, FK_ALTAR, FK_MACHINE, FK_RACK, FK_LOCKER, FK_ATM, FK_STAGE, FK_BAR_COUNTER, FK_BLACKBOARD,
	FK_CHECKOUT, FK_TREADMILL, FK_PALLET, FK_CEILING_LAMP, FK_FLOOR_LAMP, FK_TABLE_LAMP, FK_STAIRS, FK_STAIRWELL, FK_COUNT };

// The names of the kinds, in the order above (the data tables name them: interiors.json, furniture.json).
inline constexpr const char *const FURNITURE_NAMES[FK_COUNT] = { "bed", "singleBed", "sofa", "armchair", "table", "chair", "desk", "officeChair", "shelf", "bookshelf",
	"counter", "fridge", "stove", "sink", "toilet", "bath", "wardrobe", "tv", "plant", "pew", "seat", "wardBed", "bars", "screen", "altar",
	"machine", "rack", "locker", "atm", "stage", "barCounter", "blackboard", "checkout", "treadmill", "pallet", "ceilingLamp", "floorLamp",
	"tableLamp", "stairs", "stairwell" };

// What a building is for, as its floors are furnished.
enum Purpose : uint8_t { PU_HOUSE, PU_FLATS, PU_HOTEL, PU_OFFICE, PU_CITY_HALL, PU_POLICE, PU_COURTHOUSE, PU_POST_OFFICE, PU_COUNCIL,
	PU_HOSPITAL, PU_SCHOOL, PU_LIBRARY, PU_MUSEUM, PU_PRISON, PU_CHURCH, PU_BUS_STATION, PU_SHOP, PU_SUPERMARKET, PU_MALL, PU_BANK,
	PU_RESTAURANT, PU_BAR, PU_NIGHTCLUB, PU_CINEMA, PU_GYM, PU_CLUB, PU_FIRE_STATION, PU_FACTORY, PU_WAREHOUSE, PU_COUNT };

// A place on a floor: an edge of what may be furnished and how far from it. YS: the far side of the ground floor's front.
enum Edge : uint8_t { EDGE_X0, EDGE_X1, EDGE_Y0, EDGE_Y1, EDGE_CX, EDGE_CY, EDGE_YS, EDGE_COUNT };
struct Ref {
	Edge edge = EDGE_X0;
	float off = 0;
};

enum StepOp : uint8_t { STEP_PUT, STEP_GRID, STEP_BACK, STEP_SIDE_LEFT, STEP_SIDE_RIGHT, STEP_WALL_ACROSS, STEP_LOBBY, STEP_COUNT };
enum StepLevel : uint8_t { LEVEL_ANY, LEVEL_GROUND, LEVEL_UPPER };

// One step of a recipe. PUT at (a, b); GRID over a, b to c, d every gx, gy; BACK and SIDE from a to c every gx; WALL_ACROSS at
// b with its door at a. `wide`: the grid's gx is at least half the floor's width.
struct Step {
	StepOp op = STEP_PUT;
	FurnitureKind kind = FK_BED;
	StepLevel level = LEVEL_ANY;
	Ref a, b, c, d;
	float gx = 0, gy = 0, angle = 0;
	bool wide = false;
};

struct InteriorTables {
	float size[FK_COUNT][3] = {}; // across, deep, high, metres
	bool seen[FK_COUNT] = {}; // reads as a box from across a room
	std::vector<Step> recipes[PU_COUNT];
};

} // namespace city::b40
