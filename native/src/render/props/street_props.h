// The street furniture as it is drawn (Roadcraft jobs/props.ts): signal posts on the approaches of signalled junctions,
// concrete utility poles with lamps, wires between consecutive poles, hydrants, bins, benches and street trees. Where
// each piece stands is the simulation's (sim/roads/street_furniture.h, also read by the walkers); here only the world
// pose of each place, the wires' look and the trees' look. The meshes are render/props/prop_kit.h, the trees render/trees.
#pragma once

#include "render/props/prop_kit.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

struct WireLook {
	float y[4] = {}, z[4] = {}; // strand heights and offsets across the pole (local z)
	int strands = 0, pieces = 1;
	float thick = 0, sag = 0, min_span = 0, max_span = 0;
	godot::Color color;
};

// How the furniture looks beyond its meshes (data/render/streets.json "wires").
struct FurnitureLook {
	WireLook wire;
	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

// A signal post: the approach it shows (directed segment into the junction) and its place (PropKind::SIGNAL).
struct SignalHead {
	int ds = -1;
	godot::Vector3 origin;
	godot::Basis basis;
};

struct TreeSpot {
	godot::Vector3 p;
	float scale = 1, yaw = 0;
	int species = 0;
};

// Props, wire strands and signal posts of everything owned by chunk `key` (segments by midpoint, nodes by position).
void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out,
		MeshBuilder &wires, std::vector<SignalHead> &heads);
// Street trees on the footways of the segments owned by chunk `key`.
void place_street_trees(const CityState &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out);

} // namespace city
