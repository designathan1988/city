// What is inside a building, floor by floor: its furniture, its partitions and the homes or shops it is divided in. A floor
// is furnished from what it reads of its level alone, so two levels alike are one type, furnished once. Everything is in
// the lot frame of the body. What a window shows of it is render/buildings/pane_rooms.cpp's.
#pragma once

#include "sim/buildings/b40/data.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace city::b40 {

struct Furniture {
	FurnitureKind kind = FK_BED;
	float x = 0, y = 0, w = 0, d = 0, h = 0;
	float angle = 0; // the way it faces, radians (0 faces -y, the street)
};
struct Partition {
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
};
struct Space { // one home, shop or office on a floor
	int volume = 0;
	float x = 0, y = 0, w = 0, d = 0;
};
struct FloorInterior {
	std::vector<Furniture> furniture;
	std::vector<Partition> partitions;
	std::vector<Space> spaces;
};
enum FloorFinish : uint8_t { FLOOR_WOOD, FLOOR_STONE, FLOOR_PORCELAIN };

// What the record of function `fn` (BuildingFn) with this body is for; a shop's own trade comes from its seed.
Purpose purpose_of(const Body &b, int fn, uint32_t seed);
// A storey's floor: stone where people pass, wood or porcelain at home, one kind for the whole building.
FloorFinish floor_finish(Purpose p, int level, uint32_t seed);

class Interior {
public:
	Interior(const Data &data, const Body &body, Purpose purpose) : data_(data), body_(body), purpose_(purpose) {}
	const FloorInterior &at(int level);
	Purpose purpose() const { return purpose_; }

private:
	std::string type_of(int level) const; // what the floor reads of its level: levels alike are one type
	const Data &data_;
	const Body &body_;
	Purpose purpose_;
	std::unordered_map<std::string, std::unique_ptr<FloorInterior>> types_;
	std::unordered_map<int, const FloorInterior *> levels_;
};

} // namespace city::b40
