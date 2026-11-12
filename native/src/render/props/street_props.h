// The street furniture as it is drawn: signal posts for the approaches of signalled junctions, the utility poles with their
// arms, lamps and wires, lighting columns, hydrants, bins, benches, post boxes, phones, drains and street trees. Where each
// piece stands is the simulation's (sim/roads/street_furniture.h and sim/street, also read by the walkers); here only the
// world pose of each place and the wires' look. The meshes are render/props/prop_kit.h, the trees render/trees.
#pragma once

#include "render/props/prop_kit.h"
#include "render/roads/road_look.h"
#include "sim/state/city_view.h"

#include <godot_cpp/variant/packed_float32_array.hpp>

#include <vector>

namespace city {

// A pole of the run under the pointer (the pole tool's preview): where, whether it carries a lamp, whether it already stands
// (only its wires are drawn) and what it is built as.
struct PreviewPole {
	godot::Vector3 at;
	bool lamp = false;
	bool standing = false;
	PoleKind kind = PoleKind::URBAN;
};

// The wires' look (streets.json "wires"): how deep a span hangs, what is strung, and what the coarse level keeps of a wire.
struct WireLook {
	float sag = 0, max_sag = 0, min_span = 0, max_span = 0, radius = 0;
	int far_chords = 0, far_sides = 0;
	// How far the coarse level is off a wire hanging `sag_m` deep: the parabola against its chords and the round against its
	// sides. What render/detail.h holds against the frame's lens to say where that level may stand.
	float chord_miss(float sag_m) const;
};

// How the furniture looks beyond its meshes (data/render/streets.json "wires" and "lamp_light"); pool_slab: how far over and under
// the ground a lamp's pool lights what stands there (render/props/lamp_pools.h).
struct FurnitureLook {
	WireLook wire;
	bool beams = false;
	float beam_radius = 0, lamp_height = 0, lamp_reach = 0, column_height = 0, column_reach = 0, pool_slab = 0;
	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

// What the light of a lamp asks of it: how far out its lens hangs and how tall it stands, on a utility pole or a lighting column.
struct ArmReach {
	float reach = 0, height = 0;
};
ArmReach arm_reach(const FurnitureLook &F, bool column);
// A lamp's lens in the world: from its foot, `lens.reach` out `toward` the road and `lens.height` up.
godot::Vector3 lens_at(const godot::Vector3 &foot, const godot::Vector3 &toward, const ArmReach &lens);
// A piece's world point (its footway's top, or the ground) and, given `basis`, its facing (+z toward the road or the traffic).
godot::Vector3 spot_world(const CityView &s, const StreetLook &L, const FurnitureSpot &p, godot::Basis *basis);
// A pole the player put up: its foot on the ground, the way it faces (toward the kerb of the street it stands by, else along its
// arms) and that street (-1: none).
struct PoleFoot {
	godot::Vector3 foot, toward;
	int seg = -1;
};
PoleFoot pole_foot(const CityView &s, const StreetLook &L, int id);

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
	godot::PackedFloat32Array poles, wires, boxes, beams, light;
	std::vector<SignalHead> heads;
	std::vector<godot::Vector3> lamps; // the lenses of the street lamps
	float wire_sag = 0; // the deepest sag among its spans (metres): what its coarse wires' error is worked out from
};

// The height of what stands on the footway at (x, z), or on the ground off it.
float street_ground(const CityView &s, const StreetLook &L, double x, double z);
// Props, poles, wires and signal posts of everything owned by chunk `key` (segments by midpoint, nodes and records by place).
void place_street_props(const CityView &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out, StreetInstances &inst);
// A planned run (Roadcraft poleLayer.ts buildPolePreview): the new poles and every span, as they will stand.
void place_pole_preview(const CityView &s, const StreetLook &L, const FurnitureLook &F, const std::vector<PreviewPole> &poles, StreetInstances &inst);
// Street trees on the footways of the segments owned by chunk `key`.
void place_street_trees(const CityView &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out);

} // namespace city
