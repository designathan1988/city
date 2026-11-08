// The tagged blocks at the end of a save, after the edited bodies: each a tag, its byte size and its payload, so a reader
// skips a tag it does not know by its size and a save without a block reads as the state with none of it (the land founded
// again from the seed clears them). The only owner of that frame; each block's payload is its owner's.
#pragma once

#include <cstdint>
#include <functional>

namespace city {

struct ByteReader;
struct ByteWriter;
struct CityState;

// Writes the blocks that have something to keep (demolition, street).
// `written` is told each block's name as it ends (the state hash's parts).
void write_extra_blocks(ByteWriter &w, const CityState &s, const std::function<void(const char *)> &written);
// Reads them up to and including SAVE_END; false when the bytes are damaged.
bool read_extra_blocks(ByteReader &r, CityState &s, uint32_t version); // version: the file's (a block's records read as that file wrote them)

} // namespace city
