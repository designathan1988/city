// The street furniture as it is drawn: signal posts for the approaches of signalled junctions, the utility poles with their
// arms, lamps and wires, lighting columns, hydrants, bins, benches, post boxes, phones, drains and street trees. Where each
// piece stands is the simulation's (sim/roads/street_furniture.h and sim/street, also read by the walkers); here only the
// world pose of each place and the wires' look. The meshes are render/props/prop_kit.h, the trees render/trees.
#pragma once

#include "render/props/prop_kit.h"
#include "render/roads/road_look.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/packed_float32_array.hpp>

#include <vector>

namespace city {

struct WireLook {
	std::vector<float> y, z; // the courses' heights and the offsets across the arm: every course at every offset
	float radius = 0, sag = 0, max_sag = 0, min_span = 0, max_span = 0, near_end = 0, far_end = 0;
	godot::Color color;
};

// How the furniture looks beyond its meshes (data/render/streets.json "wires" and "lamp_light").
struct FurnitureLook {
	WireLook wire;
	float pool_radius = 0, beam_radius = 0, lamp_height = 0, lamp_reach = 0, column_height = 0, column_reach = 0;
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

// What a chunk draws as instances, each list one MultiMesh (rs_util.h layout).
struct StreetInstances {
	godot::PackedFloat32Array poles, wires, boxes, light;
	std::vector<SignalHead> heads;
};

// The height of what stands on the footway at (x, z), or on the ground off it.
float street_ground(const CityState &s, const StreetLook &L, double x, double z);
// Props, poles, wires and signal posts of everything owned by chunk `key` (segments by midpoint, nodes and records by place).
void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out, StreetInstances &inst);
// A planned run (Roadcraft poleLayer.ts buildPolePreview): the new poles and every span, as they will stand.
void place_pole_preview(const CityState &s, const StreetLook &L, const FurnitureLook &F, const std::vector<godot::Vector3> &at, const std::vector<bool> &lamp,
		const std::vector<bool> &standing, StreetInstances &inst);
// Street trees on the footways of the segments owned by chunk `key`.
void place_street_trees(const CityState &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out);

} // namespace city
