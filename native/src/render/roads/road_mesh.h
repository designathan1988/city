// The road mesher's pieces: segment cross-sections (road_segment.cpp), junction plates,
// kerb returns and dead ends (road_junction.cpp), painted markings (road_markings.cpp). All of them append to one
// MeshBuilder per chunk with the vertex conventions of road_look.h.
#pragma once

#include "render/mesh_builder.h"
#include "render/roads/road_look.h"
#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

namespace city {

struct RoadEmit {
	const CityState &s;
	const StreetLook &L;
	MeshBuilder &mb;
};

// Position on the centre line at arc length `at` (height = RoadNetwork::seg_y), unit forward and left vectors.
struct SegFrame {
	godot::Vector3 c, fwd, left;
};
SegFrame seg_frame(const CityState &s, int sid, double at);

// A node whose mouths get crossings, stop lines and dropped kerbs (three or more arms, not a roundabout or rails).
bool crossing_node(const CityState &s, int node);

// Four corners with their own uv (texture metres) and uv2 (lamp pool), flat normal facing `out`.
void emit_quad(MeshBuilder &mb, const godot::Vector3 p[4], const godot::Vector2 uv[4], const godot::Vector2 uv2[4], const godot::Color &col,
		const godot::Vector3 &out);

void emit_segment(const RoadEmit &e, int sid);
void emit_markings(const RoadEmit &e, int sid);
void emit_node(const RoadEmit &e, int node);

} // namespace city
