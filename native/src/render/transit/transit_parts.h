// What the transport is made of (data/render/transit.json): kinds of box and the boxes of a track, a station, a stop and a metro entrance,
// laid into mesh builders. Pure geometry over the plan: transit_renderer.h owns the meshes.
#pragma once

#include "render/mesh_builder.h"
#include "sim/state/city_cut.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

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
	// The parts the code lays by name, resolved when the table is read ([0] at the surface, [1] the metro's).
	int ballast_part[2] = {}, sleeper_part[2] = {}, rail_part[2] = {};
	int bore_part = 0, roof_part = 0, car_part = 0;
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
	std::vector<std::pair<int, float>> preview; // the run being laid: a surface part and its opacity
	float preview_step = 0; // how far a point of that run moves before its part is made again

	// Every key, type and length checked (render/data_reader.h); a box or a named part that names no part is an error.
	bool read(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

// One stretch's surfaces: opaque, see-through, and the layers drawn through the ground.
struct TransitMeshes {
	MeshBuilder solid, glass, xray[TRANSIT_XRAY_LAYERS];
};

// Lays a box: `base` the middle of its bottom, `dir` along its length, `tint` when it takes the line's colour.
void transit_box(TransitMeshes &m, const TransitLook &look, int part, const godot::Vector3 &base, const V2 &dir, const godot::Vector3 &size, const godot::Color *tint = nullptr);

// The geometry reads a CityView: the live city (the run being laid, its caller holding the map) or a job's cut of what it stands on.
// A track is laid in stretches of look.run metres (a stretch a mesh of its own, so the view can leave the rest out): the box of each
// stretch's pieces on the map, from its points alone (what a job cuts the city by, grown by how far a piece reads the street).
std::vector<CutBox> transit_track_stretches(const TransitLook &look, const RailTrack &track);
// Stretch `stretch` of a track into its meshes.
void transit_track_stretch(const CityView &s, const TransitLook &look, const RailTrack &track, size_t stretch, TransitMeshes &out);
// A track whole into one mesh (the run being laid). Its pieces are the same as in stretches: each length of the track lays its own.
void transit_track_whole(const CityView &s, const TransitLook &look, const RailTrack &track, TransitMeshes &out);
// A stop's boxes. `track`: the track it stands on (null: none); `tint`: its line's colour (transit_line_colour).
void transit_stop_parts(const CityView &s, const TransitLook &look, const TransitStop &stop, const RailTrack *track, const godot::Color &tint, TransitMeshes &out);
// The colour of the first line of the plan that calls at `stop`, or the look's default.
godot::Color transit_line_colour(const TransitData &plan, const TransitLook &look, int stop);

} // namespace city
