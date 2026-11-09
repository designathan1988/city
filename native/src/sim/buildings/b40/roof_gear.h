// What the player sets on a building's roof (Build > Roof): the gear's kinds, a piece of it at a point of the plan, and the flag a
// mast flies, by one of the Roadcraft's designs in three colours. A block holds its gear (types.h Volume::gear).
#pragma once

#include <cstdint>

namespace city::b40 {

// What the player sets on a roof, at a point of the plan; `turn` in radians, `h` what stands up from the roof.
enum RoofGear : uint8_t { GEAR_SOLAR, GEAR_SKYLIGHT, GEAR_VENT, GEAR_CHIMNEY, GEAR_TANK, GEAR_SPIRE, GEAR_LANTERN, GEAR_COUNT };
// A flag flown from a mast (a spire, a lantern): one of the Roadcraft's designs (flags.ts FLAG_PATTERNS, drawn by
// building_parts.gdshader) in three colours, 0xRRGGBB: the field, the second and the third.
enum FlagPattern : uint8_t {
	FLAG_SOLID, FLAG_BICOLOUR_H, FLAG_BICOLOUR_V, FLAG_TRICOLOUR_H, FLAG_TRICOLOUR_V, FLAG_CROSS, FLAG_NORDIC,
	FLAG_DIAGONAL, FLAG_SALTIRE, FLAG_CANTON, FLAG_STRIPES, FLAG_BORDER, FLAG_DISC, FLAG_CHEVRON, FLAG_COUNT
};
constexpr int FLAG_COLOURS = 3, FLAG_NONE = -1;
// The source game's default design (flags.ts DEFAULT_FLAG): blue, white and red.
constexpr uint32_t FLAG_BLUE = 0x0b5ea8, FLAG_WHITE = 0xffffff, FLAG_RED = 0xc8102e;
struct FlagDesign {
	int8_t pattern = FLAG_NONE; // a FlagPattern; FLAG_NONE: no flag flown
	uint32_t rgb[FLAG_COLOURS] = { FLAG_BLUE, FLAG_WHITE, FLAG_RED };
};
constexpr FlagDesign DEFAULT_FLAG{ FLAG_TRICOLOUR_V }; // what a new lantern flies (buildingRoofs.ts); a spire flies none until set
struct RoofPart {
	uint8_t kind = GEAR_VENT;
	float x = 0, y = 0, w = 1, d = 1, h = 1, turn = 0;
	FlagDesign flag; // on a spire or a lantern
};

} // namespace city::b40
