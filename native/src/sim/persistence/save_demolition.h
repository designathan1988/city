// The block of a save that holds what blows left on the map (DamageState): craters, ruins, street furniture knocked out,
// buildings worn by blows, the rest of the growth hold and the streams of blows and rounds. A tagged block (tag, byte
// size, payload) written after the growth queue and before SAVE_END, so a reader that does not know the tag skips it by
// its size and a save without it reads as a city nothing blew up. Fires and people are not saved. Little endian.
#pragma once

#include "core/bytes.h"
#include "sim/buildings/building_store.h"
#include "sim/demolition/damage_state.h"

namespace city {

struct CityState;

constexpr uint32_t SAVE_TAG_DEMOLITION = 0x31474D44U; // 'DMG1'

// One building record as saved (the table of buildings and a ruin keep the same record), at least BUILDING_SAVE_BYTES long (the site byte after them since SITE_SAVE_VERSION).
constexpr size_t BUILDING_SAVE_BYTES = 5 * sizeof(float) + 2 * sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + 2 * sizeof(uint16_t) +
		sizeof(int16_t) + sizeof(double);
void put_building(ByteWriter &w, const Building &b);
bool get_building(ByteReader &r, Building &b, uint32_t version); // version: the file's (SITE_SAVE_VERSION brought the site byte)

// The block's payload (save_blocks.cpp frames it): false, and nothing written, when nothing was blown.
bool write_damage_block(ByteWriter &w, const DamageState &d);
bool read_damage_block(ByteReader &r, DamageState &d, uint32_t version);

} // namespace city
