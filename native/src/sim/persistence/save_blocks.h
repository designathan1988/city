// The tagged blocks at the end of a save, after the edited bodies: each a tag, its byte size and its payload, so a reader
// skips a tag it does not know by its size and a save without a block reads as the state with none of it (the land founded
// again from the seed clears them). The only owner of that frame; each block's payload is its owner's.
#pragma once

namespace city {

struct ByteReader;
struct ByteWriter;
struct CityState;

// Writes the blocks that have something to keep (demolition, street).
void write_extra_blocks(ByteWriter &w, const CityState &s);
// Reads them up to and including SAVE_END; false when the bytes are damaged.
bool read_extra_blocks(ByteReader &r, CityState &s);

} // namespace city
