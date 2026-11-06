// The save file of a city, one file per city: what the player made is written, what the seed regenerates is not. Height tiles are written only when the player sculpted them
// (Heightfield::tile_dirty); lots, trees, traffic and the render are derived again on load. Little endian.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace city {

struct CityState;
struct Catalog;
struct ByteWriter;
struct ByteReader;

constexpr uint32_t SAVE_MAGIC = 0x31534443U; // 'CDS1'
constexpr uint32_t SAVE_END = 0x444E4523U; // '#END'
constexpr uint32_t SAVE_BODIES = 0x53594442U; // 'BDYS': the section of edited bodies; a file without it has none
constexpr uint32_t SPEC_SAVE_VERSION = 7; // the roads the player composed, among the variants
constexpr uint32_t SAVE_VERSION = 8; // 2: the landscape after the buildings; 3: the road variants, before the roads that use them; 4: the junctions' signal settings; 5: public transport after the growth queue; 6: a node's authored height (lift) in place of its height, solved again on load; 8: the land's side in the header
constexpr uint32_t SIGNAL_SAVE_VERSION = 4, TRANSIT_SAVE_VERSION = 5, GRADE_SAVE_VERSION = 6, LAND_SAVE_VERSION = 8;

// The line between a composed road's two directions, as saved.
inline const char *const CENTRE_LINES[] = { "auto", "none", "dashed", "solid", "double" };
constexpr int CENTRE_LINE_COUNT = 5;

// The bytes of the whole city (caller holds map_mutex, shared is enough: it only reads).
struct SavePart { // where a part of the save ends in its bytes (api/city_core_state_probe.cpp)
	const char *name;
	size_t end;
};
std::vector<uint8_t> encode_city(const CityState &s, std::vector<SavePart> *parts = nullptr);
// Whether the bytes are a whole city the catalogue can open, read into a scratch city with this copy of it: nothing of the
// open city is touched (Roadcraft persistence.ts reads the file before it replaces the document). `error`: a strings.csv key.
constexpr uint32_t MAX_RECORDS = 50000000; // sanity bound on any saved table: a corrupt count stops here, before any allocation

// Where the opening of a city went, in milliseconds.
struct OpenTimes {
	double read = 0, grade = 0, compose = 0, lots = 0;
};

bool check_city(Catalog catalog, const uint8_t *data, size_t size, std::string &error);
// Replaces the city with the saved one. Give it bytes check_city took: a file that fails halfway leaves `s` mixed.
bool decode_city(CityState &s, const uint8_t *data, size_t size, std::string &error, OpenTimes *times = nullptr);

// The landscape part of the file (save_nature.cpp).
void write_nature(ByteWriter &w, const CityState &s);
bool read_nature(ByteReader &r, CityState &s);

} // namespace city
