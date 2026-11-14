// The zone codes written on a street's sides, on a lot and on a building, and how each is read: its use whatever its density, and
// its density. Their look and names are data (data/catalog/zones.json, whose `base` and `density` say the same); a code is saved, so
// a new one goes at the end.
#pragma once

namespace city {

enum Zone : int { ZONE_NONE = 0, ZONE_RES = 1, ZONE_COM = 2, ZONE_IND = 3, ZONE_MIXED = 4, ZONE_RES_LOW = ZONE_MIXED + 1, ZONE_RES_HIGH = ZONE_MIXED + 2,
	ZONE_COM_LOW = ZONE_RES_HIGH + 1, ZONE_COM_HIGH = ZONE_RES_HIGH + 2, ZONE_IND_LOW = ZONE_COM_HIGH + 1, ZONE_IND_HIGH = ZONE_COM_HIGH + 2,
	ZONE_COUNT = ZONE_IND_HIGH + 1 };
constexpr int ZONE_CODES = ZONE_COUNT;

// What a zone is for: residential, commercial, industrial or mixed (the four first codes), whatever its density.
constexpr int zone_use(int z) {
	return z == ZONE_RES_LOW || z == ZONE_RES_HIGH ? ZONE_RES : z == ZONE_COM_LOW || z == ZONE_COM_HIGH ? ZONE_COM : z == ZONE_IND_LOW || z == ZONE_IND_HIGH ? ZONE_IND : z;
}
// Its density: 0 low, 1 medium, 2 high.
constexpr int zone_step(int z) {
	return z == ZONE_RES_LOW || z == ZONE_COM_LOW || z == ZONE_IND_LOW ? 0 : z == ZONE_RES_HIGH || z == ZONE_COM_HIGH || z == ZONE_IND_HIGH ? 2 : 1;
}
// The code of a use at a density; a use with one density (mixed, none) is its own code at any.
constexpr int zone_of(int use, int step) {
	constexpr int LOW[] = { ZONE_NONE, ZONE_RES_LOW, ZONE_COM_LOW, ZONE_IND_LOW, ZONE_MIXED }, HIGH[] = { ZONE_NONE, ZONE_RES_HIGH, ZONE_COM_HIGH, ZONE_IND_HIGH, ZONE_MIXED };
	return use < ZONE_NONE || use > ZONE_MIXED ? use : step == 0 ? LOW[use] : step == 2 ? HIGH[use] : use;
}

} // namespace city
