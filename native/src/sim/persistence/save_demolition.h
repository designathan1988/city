// The block of a save that holds what blows left on the map (DamageState): craters, ruins, street furniture knocked out,
// buildings worn by blows, the rest of the growth hold and the streams of blows and rounds. A tagged block (tag, byte
// size, payload) written after the growth queue and before SAVE_END, so a reader that does not know the tag skips it by
// its size and a save without it reads as a city nothing blew up. Fires and people are not saved. Little endian.
#pragma once

#include "core/bytes.h"
#include "sim/buildings/building_store.h"

namespace city {

struct CityState;

constexpr uint32_t SAVE_TAG_DEMOLITION = 0x31474D44U; // 'DMG1'

// One building record as saved (the table of buildings and a ruin keep the same record).
void put_building(ByteWriter &w, const Building &b);
bool get_building(ByteReader &r, Building &b);

// Writes the blocks that follow the growth queue (the demolition block when anything was blown).
void write_extra_blocks(ByteWriter &w, const CityState &s);
// Reads them up to and including SAVE_END; false when the bytes are damaged.
bool read_extra_blocks(ByteReader &r, CityState &s);

} // namespace city
