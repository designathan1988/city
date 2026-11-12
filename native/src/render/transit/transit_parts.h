// What the transport is made of (data/render/transit.json): kinds of box and the boxes of a track, a station, a stop and a metro entrance,
// laid into mesh builders. Pure geometry over the plan: transit_renderer.h owns the meshes.
#pragma once

#include "render/mesh_builder.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <unordered_map>
#include <vector>

namespace city {

constexpr int TRANSIT_XRAY_LAYERS = 5; // materials.json transit_xray: drawn in this order through the ground

struct TransitPart {
	godot::Color colour;
	float roughness = 0, metal = 0;
	int xray = -1;
	bool glass = false;
};

// `at` along, out, up from its frame; `size` long, high, wide.
struct TransitBox {
	int part = 0;
	godot::Vector3 at, size;
	bool tint = false;
};
using TransitBoxes = std::vector<TransitBox>;

struct TrackLayer {
	float up = 0, high = 0, wide = 0, over = 0, along = 0;
};

struct TransitLook {
	std::vector<TransitPart> parts;
	std::unordered_map<std::string, int> part_of;
	godot::Color default_line;
	float gauge = 0, piece = 0, sleeper_step = 0, run = 0, rail_paved_up = 0, rail_paved_high = 0;
	TrackLayer ballast, sleeper, rail;
	float kerb = 0, depth = 0, bore_wide = 0, bore_high = 0, floor_up = 0, slab = 0, wall_off = 0, roof_wide = 0, bore_over = 0;
	godot::Vector2 shaft, passage;
	float entrance_step = 0, entrance_reach = 0, entrance_clear = 0;
	int entrance_turns = 0;
	TransitBoxes station, stop, terminal, entrance, below;
	godot::Vector3 car;
	float car_up = 0;

	bool read(const godot::Dictionary &t, godot::PackedStringArray &errors);
	int part(const char *name) const { return part_of.at(name); }
};

// One stretch's surfaces: opaque, see-through, and the layers drawn through the ground.
struct TransitMeshes {
	MeshBuilder solid, glass, xray[TRANSIT_XRAY_LAYERS];
};

// Lays a box: `base` the middle of its bottom, `dir` along its length, `tint` when it takes the line's colour.
void transit_box(TransitMeshes &m, const TransitLook &look, int part, const godot::Vector3 &base, const V2 &dir, const godot::Vector3 &size, const godot::Color *tint = nullptr);

// One input segment setup, one short piece, one completed stretch, or completion per admitted call.
// No Data/City pointer is retained; the caller restarts on changed source revisions.
struct TransitTrackCursor {
	size_t point = 1, stretch = 0;
	int index = 0, pieces = 0;
	float walked = 0, piece = 0;
	V2 a, direction;
	bool has_stretch = false;
};
enum class TransitTrackStep { MORE, READY, FINISHED };
TransitTrackStep transit_track_step(const CityState &s, const TransitLook &look, const RailTrack &track,
		TransitTrackCursor &cursor, TransitMeshes &out, size_t &stretch);
void transit_stop_parts(const CityState &s, const TransitLook &look, const TransitStop &stop, TransitMeshes &out);

} // namespace city
