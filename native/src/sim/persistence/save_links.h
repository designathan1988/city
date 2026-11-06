// The save's block of the lanes the player connected by hand at a junction (RoadNode::links).
#pragma once

#include <cstdint>

namespace city {

struct ByteReader;
struct ByteWriter;
class RoadNetwork;

constexpr uint32_t SAVE_TAG_LINKS = 0x314B4E4CU; // 'LNK1'

bool write_links_block(ByteWriter &w, const RoadNetwork &net); // false: no node has any
bool read_links_block(ByteReader &r, RoadNetwork &net);

} // namespace city
