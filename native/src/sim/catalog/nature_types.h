// The landscape's definitions, filled from data/catalog/nature.json by api/nature_loader.cpp (no values here): the
// limits and defaults of the map's weather, the placed clouds, the painted fog, the ground paint, the tree brush and
// the plant communities of each biome (sim/nature/ecology.h).
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace city {

// Every number of the map's weather and sky (Landscape > Sky): one X-macro names them for the struct, the data, the
// save and the sliders, so a new one is one line.
#define CITY_WEATHER_FIELDS(X) \
	X(rain) X(wind) X(wind_direction) X(lightning) X(thunder) X(waves) X(foam) X(current) X(fog_density) X(mist) \
	X(mist_height_m) X(gully_auto)

struct LimitDef {
	float lo = 0, hi = 0, def = 0;
	float clamp(float v) const { return v < lo ? lo : (v > hi ? hi : v); }
};

struct WeatherDef {
#define CITY_WEATHER_LIMIT(name) LimitDef name;
	CITY_WEATHER_FIELDS(CITY_WEATHER_LIMIT)
#undef CITY_WEATHER_LIMIT
};

struct CloudDef {
	int max_clouds = 0, scatter_tries = 0;
	float span_m = 0; // the sky repeats every this much each way
	float crowd_share = 0, scatter_reach = 0, show_from = 0, show_span = 0;
	float pick_centre = 0, pick_radius = 0, grip_height = 0;
	LimitDef height_m, size_m, density;
};

struct FogDef {
	int max_dabs = 0;
	float texel_m = 0, min_density = 0;
	LimitDef height_m, speed_ms;
};

// What one kind of paint is: a surface of the ground (`layer` in terrain.gdshader's table), a cover standing on it, a
// biome, the rubber ("grass") or the gully's two channels.
enum class PaintClass : uint8_t { GROUND, COVER, BIOME, ERASER };
enum class CoverKind : uint8_t { FOREST, SCRUB, FLOWERS, ROCKS, COUNT };
constexpr int COVER_KINDS = int(CoverKind::COUNT);

struct PaintKindDef {
	std::string id;
	PaintClass cls = PaintClass::GROUND;
	int slot = 0; // ground: its table entry; cover: CoverKind; biome: its index in `biomes`
};

struct PaintDef {
	int max_tiles = 0; // tiles of the map a save may hold
	float strength_scale = 0, min_strength = 0, land_rim = 0, rock_reach = 0;
	float rock_rim = 0; // the share of a landform rock's dab its falloff takes: flat to the cliff's foot
	std::vector<int> landform_rock; // kind index of mesa, canyon, escarpment, sugarloaf
	std::vector<PaintKindDef> kinds;
};

struct TreeBrushDef {
	int max_planted = 0, max_clearings = 0, max_per_dab = 0, tries_per_tree = 0, model_slots = 0;
	float min_height_share = 0, m2_per_hectare = 0, seed_top = 0;
	std::vector<int> kind_first, kind_count; // per TreeKind: its slots among the models (mixed is every slot)
};

// The strata of one plant community, as densities 0..1.
struct Community {
	float canopy = 0, trees = 0, emergent = 0, shrub = 0, palm = 0, cactus = 0, grass = 0, dry = 0, wet = 0, rocky = 0;
};
enum class Formation : int { GALLERY, VEREDA, CERRADAO, CERRADO, RUPESTRE, OMBROPHILOUS, HIGH_GRASSLAND, TERRA_FIRME, VARZEA, THORN_SCRUB,
	CAATINGA_RIVER, CAMPOS, CAPAO, FLOODED_FIELD, CORDILHEIRA, OUTCROP, COUNT };
constexpr int FORMATIONS = int(Formation::COUNT);
enum class Biome : int { NONE = -1, CERRADO = 0, ATLANTIC, AMAZON, CAATINGA, PAMPA, PANTANAL, COUNT };
constexpr int BIOMES = int(Biome::COUNT);

// How a biome mixes its communities by the site, the keys of nature.json ecology.mixes (each one's origin is in its _doc).
#define CITY_ECOLOGY_MIX_FIELDS(X) \
	X(vereda_deep_from) X(vereda_deep_to) X(vereda_patch_from) X(vereda_patch_to) X(vereda_water_near_m) X(vereda_water_far_m) \
	X(dense_deep_from) X(dense_deep_to) X(dense_deep_share) X(dense_patch_from) X(dense_patch_to) X(dense_patch_share) \
	X(rupestre_ridge) X(rupestre_sandstone) X(cerrado_floor) X(ridge_altitude_m) X(patch_altitude_m) X(varzea_water_near_m) \
	X(varzea_water_far_m) X(varzea_valley) X(caatinga_ridge) X(capao_patch_from) X(capao_patch_to) X(capao_deep_from) \
	X(capao_deep_to) X(capao_riparian) X(pampa_ridge) X(pampa_basalt) X(low_valley_base) X(low_valley) X(low_ridge_from) \
	X(low_ridge_to) X(low_water_near_m) X(low_water_far_m) X(cordilheira_patch_from) X(cordilheira_patch_to) X(cordilheira_low) \
	X(pantanal_riparian) X(pantanal_rest) X(ragged_base) X(ragged_span) X(shrub_base) X(shrub_span)

struct EcologyMixDef {
#define CITY_MIX_FIELD(name) float name = 0;
	CITY_ECOLOGY_MIX_FIELDS(CITY_MIX_FIELD)
#undef CITY_MIX_FIELD
};

struct EcologyDef {
	std::array<Community, FORMATIONS> formations;
	EcologyMixDef mix;
	float grid_m = 0, patch_m = 0, fray_m = 0;
	float low_m = 0, high_m = 0, flat_from_deg = 0, flat_to_deg = 0, steep_from_deg = 0, steep_to_deg = 0;
	float riparian_near_m = 0, riparian_near_patch_m = 0, riparian_far_m = 0, riparian_far_patch_m = 0;
	float forest_line_from_m = 0, forest_line_to_m = 0;
	int mean_cells = 0;
	float canopy_wood = 0, trees_wood = 0, emergent_wood = 0; // how each stratum makes a wood (tree_place field)
	int default_biome = 0;
	int sandstone_surface = 0, basalt_surface = 0; // the painted ground (PaintCell::surface) that is sandstone, basalt: the site's rock
};

// The element brush: how many a map keeps, how many a dab lays, the area its density is per, how small one may be and how
// many kinds there are (nature.json "elements").
struct ElementDef {
	int max_items = 0, max_per_dab = 0, kinds = 0;
	int first_effect = 0; // the kinds before it are things laid on the ground (cover.json elements.props), from it effects (render/element_effects.gd)
	float area_m2 = 1, least_size_m = 0;
};

struct NatureDef {
	WeatherDef weather;
	CloudDef clouds;
	FogDef fog;
	PaintDef paint;
	TreeBrushDef trees;
	EcologyDef ecology;
	ElementDef elements;
};

} // namespace city
