// The street's block of the save (save_street.cpp): its payload; save_blocks.cpp frames it with SAVE_TAG_STREET.
#pragma once

#include <cstdint>

namespace city {

struct ByteReader;
struct ByteWriter;
class StreetWorks;

constexpr uint32_t SAVE_TAG_STREET = 0x54525453U; // 'STRT'

// False, and nothing written, when the player placed nothing.
bool write_street_block(ByteWriter &w, const StreetWorks &k);
bool read_street_block(ByteReader &r, StreetWorks &k, uint32_t version); // false on a corrupt block; `version`: the file's

} // namespace city
