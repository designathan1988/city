// Typed, reversible landscape payloads. Their owned data is visible to history accounting and page serialization.
#pragma once

#include "sim/nature/nature_state.h"
#include "sim/demolition/damage_state.h"

#include <array>
#include <memory>
#include <optional>
#include <variant>

namespace city {

struct CityState;
struct WeatherEdit { Weather before, after; };
struct CloudsEdit { std::vector<PlacedCloud> before, after; int32_t next_before = 1, next_after = 1; };
struct FogAdded { FogDab dab; };
struct FogCleared { std::vector<FogDab> before; };
struct ClearingEdit { int id = -1; TreeClearing clearing; };
struct TreesEdit {
	std::vector<int> ids;
	std::vector<PlantedTree> trees;
	bool planted = true;
	std::optional<ClearingEdit> clearing;
	float x = 0, z = 0, radius = 0;
};
struct TreesCleared {
	std::shared_ptr<const PlantedTrees> before;
	float half = 0;
	size_t bytes = 0; // immutable snapshot: compute once, not once per future history insertion
	TreesCleared(std::shared_ptr<const PlantedTrees> value, float extent) : before(std::move(value)), half(extent), bytes(before ? before->retained_bytes() : 0) {}
};
struct BiomeEdit { int before = 0, after = 0; float half = 0; };
struct ElementsEdit { std::vector<ElementItem> items; bool lay = true; };
struct WaterEdit {
	std::vector<uint32_t> index;
	std::vector<float> before, after;
	std::optional<float> level; // a river dab sets a constant; a reset restores each sample's generated level
	std::array<double, 4> bounds{};
};
struct StrikeCountEdit { uint32_t before = 0, after = 0; };
struct ScarAdded { Scar scar; };

using NatureEdit = std::variant<WeatherEdit, CloudsEdit, FogAdded, FogCleared, TreesEdit, TreesCleared, BiomeEdit,
	ElementsEdit, WaterEdit, StrikeCountEdit, ScarAdded>;

void nature_sky_changed(CityState &s);
void nature_fog_changed(CityState &s);
void nature_trees_changed(CityState &s, float x, float z, float radius);
void nature_water_changed(CityState &s, const WaterEdit &edit);
void apply_elements_edit(CityState &s, const ElementsEdit &edit, bool forward);
void apply_nature_edit(CityState &s, const NatureEdit &edit, bool forward);

} // namespace city
