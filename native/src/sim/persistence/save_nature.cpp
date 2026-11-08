// The landscape in the save file: the weather by name order of its X-macro, the clouds, the fog dabs, the painted tiles
// that exist, the planted trees and clearings with their slots, and the map's ecosystem. Corners of water the river
// brush laid are written as the difference from the generated level (Heightfield::water_original).
#include "sim/persistence/save_format.h"

#include "core/bytes.h"
#include "sim/state/city_state.h"
#include "sim/terrain/terrain_gen.h"

#include <algorithm>

namespace city {

namespace {
constexpr uint32_t MAX_LIST = 1u << 24; // the most slots a list of planted trees or clearings may claim

template <typename T>
void put_slots(ByteWriter &w, const std::vector<T> &list) {
	uint32_t n = 0;
	for (const T &v : list) n += v.alive ? 1 : 0;
	w.put_slots(list.size());
	w.put<uint32_t>(n);
	for (uint32_t i = 0; i < list.size(); i++)
		if (list[i].alive) {
			w.put<uint32_t>(i);
			w.put<T>(list[i]);
		}
}

template <typename T, typename F>
bool get_slots(ByteReader &r, F put, uint32_t &size) {
	size = r.get<uint32_t>(); // the slots, dead ones too: nothing of a dead one is written, so only the list bounds it
	const uint32_t n = r.get_count(sizeof(uint32_t) + sizeof(T)); // each alive one: its id and its record
	if (!r.ok || size > MAX_LIST || n > size) return false;
	for (uint32_t k = 0; k < n && r.ok; k++) {
		uint32_t id = r.get<uint32_t>();
		T v = r.get<T>();
		if (r.ok && id < size) put(int(id), v);
	}
	return r.ok;
}
} // namespace

void write_nature(ByteWriter &w, const CityState &s) {
	const NatureState &n = s.nature;
#define CITY_WEATHER_PUT(name) w.put<float>(n.weather.name);
	CITY_WEATHER_FIELDS(CITY_WEATHER_PUT)
#undef CITY_WEATHER_PUT
	w.put<int32_t>(n.next_cloud);
	w.put_vec(n.clouds);
	w.put_vec(n.fog);
	w.put<int32_t>(n.nature.region);
	w.put<uint32_t>(n.nature.seed);
	w.put_by_key(n.ground.all(), [&](const auto &kv) {
		w.put<int32_t>(kv.first);
		w.put_bytes(kv.second->data(), sizeof(PaintTile));
	});
	put_slots(w, n.planted.trees);
	put_slots(w, n.planted.clearings);
	const Heightfield &t = s.terrain;
	std::vector<uint32_t> index;
	std::vector<float> level;
	for (const auto &kv : t.water_original) // only what changed can differ from the generated land
		if (t.water[kv.first] != TerrainGen::NO_WATER && t.water[kv.first] != kv.second) index.push_back(kv.first);
	std::sort(index.begin(), index.end()); // in sample order, as the file always had them
	for (uint32_t i : index) level.push_back(t.water[i]);
	w.put_vec(index);
	w.put_vec(level);
	w.put_set(n.elements.all(), ElementItem::before);
}

bool read_nature(ByteReader &r, CityState &s) {
	NatureState &n = s.nature;
#define CITY_WEATHER_GET(name) n.weather.name = r.get<float>();
	CITY_WEATHER_FIELDS(CITY_WEATHER_GET)
#undef CITY_WEATHER_GET
	n.weather = clamped_weather(n.weather, s.catalog.nature.weather);
	n.next_cloud = r.get<int32_t>();
	r.get_vec(n.clouds, size_t(s.catalog.nature.clouds.max_clouds));
	r.get_vec(n.fog, size_t(s.catalog.nature.fog.max_dabs));
	n.nature.region = r.get<int32_t>();
	n.nature.seed = r.get<uint32_t>();
	const uint32_t tiles = r.get_count(sizeof(int32_t) + sizeof(PaintTile));
	if (!r.ok || tiles > uint32_t(s.catalog.nature.paint.max_tiles)) return false;
	auto tile = std::make_unique<PaintTile>();
	for (uint32_t i = 0; i < tiles && r.ok; i++) {
		int key = r.get<int32_t>();
		r.get_bytes(tile->data(), sizeof(PaintTile));
		if (r.ok) n.ground.put_tile(key, tile.get());
	}
	uint32_t tree_slots = 0, clearing_slots = 0;
	if (!get_slots<PlantedTree>(r, [&](int id, const PlantedTree &t) { n.planted.put(id, t); }, tree_slots)) return false;
	if (!get_slots<TreeClearing>(r, [&](int id, const TreeClearing &c) { n.planted.put_clearing(id, c); }, clearing_slots))
		return false;
	n.planted.keep_slots(int(tree_slots), int(clearing_slots));
	std::vector<uint32_t> index;
	std::vector<float> level;
	r.get_vec(index);
	r.get_vec(level);
	if (!r.ok || index.size() != level.size()) return false;
	for (size_t i = 0; i < index.size(); i++)
		if (index[i] < s.terrain.water.size()) s.terrain.set_water(index[i], level[i]);
	std::vector<ElementItem> elements;
	r.get_vec(elements, size_t(s.catalog.nature.elements.max_items));
	if (!r.ok) return false;
	for (const ElementItem &e : elements) n.elements.add(e);
	n.sky_revision++;
	n.fog_revision++;
	return true;
}

} // namespace city
