// The Landscape drawer's commands (nature_commands.cpp, element_commands.cpp): the sky, the fog, the painted ground, the planted trees,
// the elements and the map's biome. Callers hold map_mutex exclusively, as for every command (commands.h).
#pragma once

#include "sim/commands/commands.h"

namespace city {

constexpr int TREE_PLANT = 0, TREE_ONE = 1, TREE_CUT = 2; // what a dab of the tree brush does
EditResult set_weather(CityState &s, const Weather &next);
EditResult add_cloud(CityState &s, const PlacedCloud &c);
EditResult edit_cloud(CityState &s, int id, const PlacedCloud &c);
EditResult remove_cloud(CityState &s, int id);
// Up to `count` clouds over the sky, each like `like` varied by `variation` (0..1).
EditResult scatter_sky(CityState &s, int count, const PlacedCloud &like, float variation);
EditResult clear_clouds(CityState &s);
EditResult fog_dab(CityState &s, const FogDab &dab);
EditResult clear_fog(CityState &s);
EditResult paint_dab(CityState &s, const PaintDab &dab);
EditResult clear_gullies(CityState &s);
EditResult tree_dab(CityState &s, double x, double z, double radius, int mode, int kind, const TreeBrush &brush);
EditResult clear_trees(CityState &s);
// One dab of the element brush: its density per ElementDef::area_m2, size and spacing in metres, the rest shares.
struct ElementBrush {
	float density = 0, size = 0, variation = 0, spacing = 0, strength = 0, intensity = 0;
};
EditResult element_dab(CityState &s, double x, double z, double radius, int kind, const ElementBrush &b, bool erase);
EditResult clear_elements(CityState &s);
// The map's ecosystem: a Biome, or Biome::NONE.
EditResult set_biome(CityState &s, int region);
// Takes the planted trees in a circle away and leaves a clearing there (the woods' trees go too): the clearing's id, -1 when full.
int cut_trees_at(CityState &s, float x, float z, float radius);

} // namespace city
