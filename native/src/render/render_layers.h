// Render layers: the bit of RenderingServer::instance_set_layer_mask each renderer tags its instances with. A layer is
// hidden by clearing its bit in the camera's cull mask (the sun's casters too: Godot's scene cull skips an instance whose
// layer the camera does not see, also in the shadow pass, renderer_scene_cull.cpp), so the Effects panel and the quality
// tiers switch a whole layer in one assignment: no instance is touched, no chunk rebuilt. The names reach the script side
// through CityCore::constants() (game/settings/render_switches.gd).
#pragma once

#include <cstdint>

namespace city {

enum RenderLayer : uint32_t {
	LAYER_BASE = 1u << 0, // what no switch hides: the tools' previews
	LAYER_TERRAIN = LAYER_BASE << 1,
	LAYER_WATER = LAYER_TERRAIN << 1,
	LAYER_ROADS = LAYER_WATER << 1,
	LAYER_BUILDING_MASSES = LAYER_ROADS << 1, // far level
	LAYER_BUILDING_FACADES = LAYER_BUILDING_MASSES << 1, // mid level
	LAYER_BUILDING_SHELLS = LAYER_BUILDING_FACADES << 1, // near level
	LAYER_BUILDING_PARTS = LAYER_BUILDING_SHELLS << 1, // near level's parts
	LAYER_BUILDING_DETAILS = LAYER_BUILDING_PARTS << 1, // instanced facade bands
	LAYER_VEGETATION = LAYER_BUILDING_DETAILS << 1,
	LAYER_FURNITURE = LAYER_VEGETATION << 1,
	LAYER_SIGNALS = LAYER_FURNITURE << 1,
	LAYER_VEHICLES = LAYER_SIGNALS << 1,
	LAYER_PEOPLE = LAYER_VEHICLES << 1,
	LAYER_OVERLAYS = LAYER_PEOPLE << 1,
	LAYER_BUILDING_FLAGS = LAYER_OVERLAYS << 1, // the masts' cloths (the 4.0 draws them in a layer of their own)
	// The far traffic's cars (far_traffic.h): apart from the vehicles', which the camera stops seeing past the tier's detail
	// cut-off (game/settings/render_switches.gd): there the far cars are the only traffic drawn.
	LAYER_FAR_TRAFFIC = LAYER_BUILDING_FLAGS << 1,
};

struct RenderLayerName {
	const char *name;
	RenderLayer bit;
};

inline constexpr RenderLayerName RENDER_LAYERS[] = {
	{ "base", LAYER_BASE }, { "terrain", LAYER_TERRAIN }, { "water", LAYER_WATER }, { "roads", LAYER_ROADS },
	{ "building_masses", LAYER_BUILDING_MASSES }, { "building_facades", LAYER_BUILDING_FACADES },
	{ "building_shells", LAYER_BUILDING_SHELLS }, { "building_parts", LAYER_BUILDING_PARTS },
	{ "building_details", LAYER_BUILDING_DETAILS }, { "vegetation", LAYER_VEGETATION }, { "furniture", LAYER_FURNITURE },
	{ "signals", LAYER_SIGNALS }, { "vehicles", LAYER_VEHICLES }, { "people", LAYER_PEOPLE }, { "overlays", LAYER_OVERLAYS },
	{ "building_flags", LAYER_BUILDING_FLAGS }, { "far_traffic", LAYER_FAR_TRAFFIC },
};

} // namespace city
