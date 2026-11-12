// Replay of typed landscape journal payloads and their domain-specific invalidations (nature_edit.h).
#include "sim/commands/nature_edit.h"

#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <cassert>
#include <type_traits>

namespace city {

void nature_sky_changed(CityState &s) { s.nature.sky_revision++; s.touch(); }
void nature_fog_changed(CityState &s) { s.nature.fog_revision++; nature_sky_changed(s); }
void nature_trees_changed(CityState &s, float x, float z, float radius) { mark_nature(s, x - radius, z - radius, x + radius, z + radius); }
void nature_water_changed(CityState &s, const WaterEdit &edit) {
	const auto &b = edit.bounds;
	for_chunks(b[0], b[1], b[2], b[3], [&](int64_t key) { s.log.water_chunks.push_back(key); });
}

void apply_elements_edit(CityState &s, const ElementsEdit &edit, bool forward) {
	for (const ElementItem &item : edit.items) {
		if (edit.lay == forward) s.nature.elements.add(item);
		else s.nature.elements.remove(item);
		mark_nature(s, item.x, item.z, item.x, item.z);
	}
}

void apply_nature_edit(CityState &s, const NatureEdit &edit, bool forward) {
	std::visit([&](const auto &value) {
		using T = std::decay_t<decltype(value)>;
		if constexpr (std::is_same_v<T, WeatherEdit>) {
			s.nature.weather = forward ? value.after : value.before;
			nature_sky_changed(s);
		} else if constexpr (std::is_same_v<T, CloudsEdit>) {
			s.nature.clouds = forward ? value.after : value.before;
			s.nature.next_cloud = forward ? value.next_after : value.next_before;
			nature_sky_changed(s);
		} else if constexpr (std::is_same_v<T, FogAdded>) {
			if (forward) s.nature.fog.push_back(value.dab);
			else { assert(!s.nature.fog.empty()); s.nature.fog.pop_back(); }
			nature_fog_changed(s);
		} else if constexpr (std::is_same_v<T, FogCleared>) {
			if (forward) s.nature.fog.clear(); else s.nature.fog.restore(value.before);
			nature_fog_changed(s);
		} else if constexpr (std::is_same_v<T, TreesEdit>) {
			assert(value.ids.size() == value.trees.size());
			for (size_t i = 0; i < value.ids.size(); i++) {
				if (forward == value.planted) s.nature.planted.put(value.ids[i], value.trees[i]);
				else s.nature.planted.remove(value.ids[i]);
			}
			if (value.clearing) {
				if (forward) s.nature.planted.put_clearing(value.clearing->id, value.clearing->clearing);
				else s.nature.planted.remove_clearing(value.clearing->id);
			}
			nature_trees_changed(s, value.x, value.z, value.radius);
		} else if constexpr (std::is_same_v<T, TreesCleared>) {
			if (forward) { s.nature.planted = PlantedTrees(); s.nature.planted.clear(); }
			else s.nature.planted = *value.before;
			mark_nature(s, -value.half, -value.half, value.half, value.half);
		} else if constexpr (std::is_same_v<T, BiomeEdit>) {
			s.nature.nature.region = forward ? value.after : value.before;
			mark_nature(s, -value.half, -value.half, value.half, value.half);
			nature_sky_changed(s);
		} else if constexpr (std::is_same_v<T, ElementsEdit>) {
			apply_elements_edit(s, value, forward);
		} else if constexpr (std::is_same_v<T, WaterEdit>) {
			assert(value.before.size() == value.index.size());
			assert(value.level || value.after.size() == value.index.size());
			for (size_t i = 0; i < value.index.size(); i++)
				s.terrain.write_water(value.index[i], forward ? (value.level ? *value.level : value.after[i]) : value.before[i]);
			nature_water_changed(s, value);
		} else if constexpr (std::is_same_v<T, StrikeCountEdit>) {
			s.damage.strikes = forward ? value.after : value.before;
		} else if constexpr (std::is_same_v<T, ScarAdded>) {
			if (forward) s.damage.add_scar(value.scar);
			else s.damage.pop_scar();
		}
	}, edit);
}

} // namespace city
