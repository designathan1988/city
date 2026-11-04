// The save file of a city, one file per city: what the player made is written, what the seed regenerates is not. Height tiles are written only when the player sculpted them
// (Heightfield::tile_dirty); lots, trees, traffic and the render are derived again on load. Little endian.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace city {

struct CityState;
struct ByteWriter;
struct ByteReader;

constexpr uint32_t SAVE_MAGIC = 0x31534443U; // 'CDS1'
constexpr uint32_t SAVE_END = 0x444E4523U; // '#END'
constexpr uint32_t SAVE_VERSION = 2; // 2: the road variants, before the roads that use them

// The bytes of the whole city (caller holds map_mutex, shared is enough: it only reads).
std::vector<uint8_t> encode_city(const CityState &s);
// Replaces the city with the saved one. On failure `error` is a strings.csv key and the state is left as a fresh map
// of the saved seed (the caller starts a new map when it was not even that).
bool decode_city(CityState &s, const uint8_t *data, size_t size, std::string &error);

// The landscape part of the file (save_nature.cpp).
void write_nature(ByteWriter &w, const CityState &s);
bool read_nature(ByteReader &r, CityState &s);

} // namespace city
