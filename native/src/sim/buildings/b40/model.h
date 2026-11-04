// Build > Models: a model as data/render/models.json puts it together, block by block. Coordinates in metres from the model's own
// front left corner; sim/commands/model_body.cpp turns one into a body in the lot frame. Plain data, filled by api/models_loader.cpp.
#pragma once

#include "sim/buildings/b40/types.h"

#include <string>
#include <vector>

namespace city::b40 {

constexpr int NO_BAY = -1, MIDDLE_BAY = -2, NO_PATTERN = -1, TOP_STOREY = -1;

// A side of the storeys `from`..`to` (TOP_STOREY: the last) filled with one component; `every` bays from `start` when it is a
// run of bays and not the whole side.
struct ModelDress {
	int side = 0, from = 0, to = TOP_STOREY, start = 0, every = 0;
	uint8_t fill = BAY_WINDOW;
};

struct ModelBlock {
	float x = 0, y = 0, w = 0, d = 0, pitch = -1;
	int base = 0, storeys = 1, door_bay = NO_BAY, pattern = NO_PATTERN;
	Roof roof = ROOF_FLAT;
	uint8_t fill = BAY_WINDOW, ground = BAY_NONE, door_kind = BAY_DOOR;
	std::vector<Point2> outline;
	std::vector<ModelDress> dress;
};

struct ModelSpec {
	std::string id, group;
	Kind kind = KIND_HOUSE;
	Use use = USE_RESIDENTIAL;
	int zone = 1, function = 0, palette = 0;
	float ground_h = 0, storey_h = 0;
	Paint wall, roof, trim;
	std::vector<Paint> paints; // what the elements below name by `paint`
	std::vector<ModelBlock> blocks;
	std::vector<LotElement> elements;
	std::vector<Core> cores;
};

struct ModelTable {
	float module_m = 0;
	std::vector<ModelSpec> items;
	const ModelSpec *find(const std::string &id) const {
		for (const ModelSpec &m : items)
			if (m.id == id) return &m;
		return nullptr;
	}
};

} // namespace city::b40
