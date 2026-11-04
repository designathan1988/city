// The landscape in the save file: the weather by name order of its X-macro, the clouds, the fog dabs, the painted tiles
// that exist, the planted trees and clearings with their slots, and the map's ecosystem. Corners of water the river
// brush laid are written as the difference from what the seed regenerates.
#include "sim/persistence/save_format.h"

#include "core/bytes.h"
#include "sim/state/city_state.h"
#include "sim/terrain/terrain_gen.h"

namespace city {

namespace {
constexpr uint32_t MAX_LIST = 1u << 24;

template <typename T>
void put_slots(ByteWriter &w, const std::vector<T> &list) {
	uint32_t n = 0;
	for (const T &v : list) n += v.alive ? 1 : 0;
	w.put<uint32_t>(uint32_t(list.size()));
	w.put<uint32_t>(n);
	for (uint32_t i = 0; i < list.size(); i++)
		if (list[i].alive) {
			w.put<uint32_t>(i);
			w.put<T>(list[i]);
		}
}

template <typename T, typename F>
bool get_slots(ByteReader &r, F put) {
	uint32_t size = r.get<uint32_t>(), n = r.get<uint32_t>();
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
	w.put<uint32_t>(uint32_t(n.ground.all().size()));
	for (const auto &kv : n.ground.all()) {
		w.put<int32_t>(kv.first);
		w.put_bytes(kv.second->data(), sizeof(PaintTile));
	}
	put_slots(w, n.planted.trees);
	put_slots(w, n.planted.clearings);
	const Heightfield &t = s.terrain;
	std::vector<uint32_t> index;
	std::vector<float> level;
	TerrainGen gen(s.seed, s.map_half, s.catalog.tune.world_water_spacing_m);
	for (int gz = 0; gz < t.water_n; gz++)
		for (int gx = 0; gx < t.water_n; gx++) {
			size_t i = size_t(gz) * t.water_n + gx;
			if (t.water[i] == TerrainGen::NO_WATER) continue;
			if (!gen.flat() && gen.water_at_sample(gx, gz) == t.water[i]) continue;
			index.push_back(uint32_t(i));
			level.push_back(t.water[i]);
		}
	w.put_vec(index);
	w.put_vec(level);
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
	uint32_t tiles = r.get<uint32_t>();
	if (!r.ok || tiles > uint32_t(s.catalog.nature.paint.max_tiles)) return false;
	auto tile = std::make_unique<PaintTile>();
	for (uint32_t i = 0; i < tiles && r.ok; i++) {
		int key = r.get<int32_t>();
		r.get_bytes(tile->data(), sizeof(PaintTile));
		if (r.ok) n.ground.put_tile(key, tile.get());
	}
	if (!get_slots<PlantedTree>(r, [&](int id, const PlantedTree &t) { n.planted.put(id, t); })) return false;
	if (!get_slots<TreeClearing>(r, [&](int id, const TreeClearing &c) { n.planted.put_clearing(id, c); })) return false;
	std::vector<uint32_t> index;
	std::vector<float> level;
	r.get_vec(index);
	r.get_vec(level);
	if (!r.ok || index.size() != level.size()) return false;
	for (size_t i = 0; i < index.size(); i++)
		if (index[i] < s.terrain.water.size()) s.terrain.water[index[i]] = level[i];
	n.sky_revision++;
	n.fog_revision++;
	return true;
}

} // namespace city
