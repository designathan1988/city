// The save file of a city (Roadcraft app/saveFlow.ts + jobs/storage.ts, one file instead of tiles): what the player
// made is written, what the seed regenerates is not. Height tiles are written only when the player sculpted them
// (Heightfield::tile_dirty); lots, trees, traffic and the render are derived again on load. Little endian.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace city {

struct CityState;

constexpr uint32_t SAVE_MAGIC = 0x31534443U; // 'CDS1'
constexpr uint32_t SAVE_END = 0x444E4523U; // '#END'
constexpr uint32_t SAVE_VERSION = 1;

// The bytes of the whole city (caller holds map_mutex, shared is enough: it only reads).
std::vector<uint8_t> encode_city(const CityState &s);
// Replaces the city with the saved one. On failure `error` is a strings.csv key and the state is left as a fresh map
// of the saved seed (the caller starts a new map when it was not even that).
bool decode_city(CityState &s, const uint8_t *data, size_t size, std::string &error);

} // namespace city
