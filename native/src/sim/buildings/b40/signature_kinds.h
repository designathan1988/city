// The buildings drawn by a design of their own (Roadcraft towerKit.ts TOWER_KINDS, the lots of its lot lab): what project/assets/lots/<name>.sig holds. A
// body's `signature` is the place in this list, plus one; 0 is a body drawn by the facade kit. A save holds the number: new names go at the end.
#pragma once

#include <cstring>

namespace city::b40 {

constexpr const char *SIGNATURE_KINDS[] = { "balconyMid", "glassOffice", "glassBalcony", "beigeClassic", "darkGlass", "whiteBalcony", "brickFrame", "roundGlass", "darkGrid",
	"artDeco", "twistGreen", "hexTerracotta", "stepGarden", "waveWhite", "cubeBalcony", "twinNavy", "octCopper", "stackedBlocks", "pinkCorner", "triangleDark", "cityHallBrick",
	"decoSpire", "hospitalA", "loftBrick", "cityHallWhite", "forum", "church", "bank", "school", "fireStation", "police", "hospitalB", "cemetery", "beauxArts", "decoTower",
	"modernOffice", "residentialTerrace", "glassSpire" };
constexpr int SIGNATURE_COUNT = int(sizeof(SIGNATURE_KINDS) / sizeof(SIGNATURE_KINDS[0]));

// The body's number for "signature:<name>", 0 for anything else.
inline int signature_of(const char *blueprint) {
	constexpr const char *PREFIX = "signature:";
	const size_t skip = std::strlen(PREFIX);
	if (std::strncmp(blueprint, PREFIX, skip) != 0) return 0;
	for (int i = 0; i < SIGNATURE_COUNT; i++)
		if (std::strcmp(blueprint + skip, SIGNATURE_KINDS[i]) == 0) return i + 1;
	return 0;
}

} // namespace city::b40
