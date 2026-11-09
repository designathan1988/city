// What a road segment's flags say (Roadcraft roads/flags.ts and network.ts): its furniture set and how it is lit, whether the
// generator laid it, the sides the player zoned, its cutting's walls, where the road goes on past it, the plates on its
// approaches and how it is built; and the flags of a segment laid the other way round. Stored in RoadSegment::flags
// (sim/roads/road_network.h) and the save file.
#pragma once

#include "sim/roads/road_grade.h"

#include <cstdint>

namespace city {

enum SegFlag : uint16_t {
	SEG_FURNITURE_MASK = 0x06, // bits 1-2: 0 full street furniture, 1 basic, 2 none
	SEG_GENERATED = 0x80, // laid by the city generator: its footways carry the generated city's kit (the 4.0 plants no street
	// trees there). Money does not read it: a generated road is worth its price, as in the 4.0 (saves before the economy
	// carry this bit as SEG_FREE, with the same meaning)
	SEG_ZONED_LEFT = 0x100, // the player zoned this side (what the Zones layer shows)
	SEG_ZONED_RIGHT = 0x200,
	SEG_CUT_WALLS = 0x40, // its cutting is held by retaining walls (Roadcraft 4.0 RoadSegment.cutWalls)
	SEG_LIT_BY_COLUMNS = 0x20, // lit by lighting columns on its footways, with no power line; clear (the road tool's default): the
	// line of utility poles carries the street lights (Roadcraft roadTool.ts layPart, powerLine.ts; sim/roads/street_furniture.h)
	SEG_RUNS_ON_A = 0x08, SEG_RUNS_ON_B = 0x10, // the road goes on past this end: a cut of the laying, no node the player made
	SEG_RULE_A_MASK = 0x0C00, // bits 10-11, 12-13: the plate the player put on its a and b approach (SegRule)
	SEG_RULE_B_MASK = 0x3000,
	SEG_STRUCTURE_MASK = 0xC000, // bits 14-15: how it is built (Structure: ground, elevated, bridge, tunnel)
};
constexpr int SEG_FURNITURE_SHIFT = 1, SEG_STRUCTURE_SHIFT = 14; // where their masks' bits start
// The street furniture a segment carries (its SEG_FURNITURE_MASK bits).
enum FurnitureLevel : uint8_t { FURN_FULL, FURN_BASIC, FURN_NONE };
constexpr Structure structure_of(uint16_t flags) { return Structure((flags & SEG_STRUCTURE_MASK) >> SEG_STRUCTURE_SHIFT); }
constexpr uint16_t with_structure(uint16_t flags, Structure s) {
	return uint16_t((flags & ~SEG_STRUCTURE_MASK) | (uint16_t(s) << SEG_STRUCTURE_SHIFT));
}
constexpr int SEG_RULE_A_SHIFT = 10, SEG_RULE_B_SHIFT = 12; // where their masks' bits start
enum SegRule : uint16_t { RULE_NONE = 0, RULE_STOP = 1, RULE_YIELD = 2 };
constexpr uint16_t seg_rule_mask(int end) { return end == 0 ? SEG_RULE_A_MASK : SEG_RULE_B_MASK; }
constexpr int seg_rule_shift(int end) { return end == 0 ? SEG_RULE_A_SHIFT : SEG_RULE_B_SHIFT; }
// The flags of a segment laid the other way round: what was on its left is on its right, its a end is its b end.
constexpr uint16_t flags_reversed(uint16_t f) {
	const uint16_t keep = uint16_t(f & ~(SEG_ZONED_LEFT | SEG_ZONED_RIGHT | SEG_RULE_A_MASK | SEG_RULE_B_MASK | SEG_RUNS_ON_A | SEG_RUNS_ON_B));
	const uint16_t sides = uint16_t(((f & SEG_ZONED_LEFT) ? SEG_ZONED_RIGHT : 0) | ((f & SEG_ZONED_RIGHT) ? SEG_ZONED_LEFT : 0));
	const uint16_t rules = uint16_t((((f & SEG_RULE_A_MASK) >> SEG_RULE_A_SHIFT) << SEG_RULE_B_SHIFT) | (((f & SEG_RULE_B_MASK) >> SEG_RULE_B_SHIFT) << SEG_RULE_A_SHIFT));
	const uint16_t runs = uint16_t(((f & SEG_RUNS_ON_A) ? SEG_RUNS_ON_B : 0) | ((f & SEG_RUNS_ON_B) ? SEG_RUNS_ON_A : 0));
	return uint16_t(keep | sides | rules | runs);
}

} // namespace city
