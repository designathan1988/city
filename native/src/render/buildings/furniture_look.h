// What each piece of furniture is made of, part by part, read from data/render/furniture.json (tools/assets/make_furniture.py
// writes it): the palette (finish and colour of each name) and a model per kind of furniture, built at the size of its row
// in interiors.json and scaled to the piece placed (render/buildings/furniture_emit.cpp).
#pragma once

#include "render/data_reader.h"
#include "sim/buildings/b40/types.h"
#include "sim/buildings/interior/tables.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace city::b40 {

enum PartShape : uint8_t { PART_BOX, PART_CYL, PART_BALL };

// One part of a model. x across (right of the piece), y up, z to the front (the way it faces).
struct FurniturePart {
	PartShape shape = PART_BOX;
	float size[3] = {}; // box: across, up, deep; cylinder: radius, height, radius at the top; ball: radius
	float at[3] = {}; // box and cylinder: centre x, bottom y, centre z; ball: its centre
	float step[3] = {}, grow[3] = {}; // each copy after the first: moved by step, its size grown by grow
	int copies = 1, spin = 1; // copies in a row; turns about the up axis
	std::vector<uint16_t> colours; // palette indices, one a copy (cycled); the first is the part's own
	float edge = -1; // a box's rounded edges, metres (negative: its kind's, InteriorLook::edges)
	// A row of copies that are not all alike (books on a shelf): each copy's share of the row along `step` drawn within 1 +- vary[0]
	// of the step (the row keeps its length), its height within 1 - vary[1] of the size, its colour drawn from `colours`.
	float vary[2] = {};
};

struct InteriorLook {
	std::vector<Paint> palette;
	std::unordered_map<std::string, int> index; // palette name -> place
	std::vector<FurniturePart> models[FK_COUNT];
	float edges[FK_COUNT] = {}; // the rounding of each kind's boxes, metres (0: hard edges)

	// A named paint of the palette (magenta when missing: `load` refuses a table without the names the drawing asks for).
	const Paint &paint(const char *name) const;
	bool load(const DataValue &table, godot::PackedStringArray &errors);
};

} // namespace city::b40
