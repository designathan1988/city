// The lots' block of the save (save_lots.cpp): its payload; save_blocks.cpp frames it.
#pragma once

#include <cstdint>

namespace city {

struct ByteReader;
struct ByteWriter;
struct LotBook;

constexpr uint32_t SAVE_TAG_LOTS = 0x31544F4CU; // 'LOT1'

// False, and nothing written, when the player drew none.
bool write_lots_block(ByteWriter &w, const LotBook &book);
bool read_lots_block(ByteReader &r, LotBook &book); // false on a corrupt block

} // namespace city
