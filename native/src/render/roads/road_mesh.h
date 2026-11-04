// The road mesher's pieces: segment cross-sections (road_segment.cpp), junction plates,
// kerb returns and road ends (road_junction.cpp), painted markings (road_markings.cpp). All of them append to one
// MeshBuilder per chunk with the vertex conventions of road_look.h.
#pragma once

#include "render/mesh_builder.h"
#include "render/roads/road_look.h"
#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

namespace city {

constexpr float STRAIGHT_TOL = 0.5f; // metres off its chord under which a segment is drawn as a straight one

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
void emit_quad(MeshBuilder &mb, const godot::Vector3 p[4], const godot::Vector2 uv[4], const godot::Vector2 uv2[4], const godot::Color cols[4],
		const godot::Vector3 &out);

void emit_segment(const RoadEmit &e, int sid);
void emit_markings(const RoadEmit &e, int sid);
void emit_node(const RoadEmit &e, int node);

// What holds a raised road up (road_structure.cpp).
void emit_structure(const RoadEmit &e, int sid);

// A kerbed edge along three lines, inner, back of the kerb and the lot's. `across` is where the line stands on the road's section.
void kerbed_line(const RoadEmit &e, const std::vector<godot::Vector3> &in, const std::vector<godot::Vector3> &bk, const std::vector<godot::Vector3> &out,
		float across, bool kerbed, const godot::Color &walk);

// The ground round the road (road_verge.cpp): from `edge`, a line along the paving's outer top, out along `away` (unit,
// flat) down to the terrain. emit_verge_node draws it round a node's corners and ends.
void verge_ribbon(const RoadEmit &e, const std::vector<godot::Vector3> &edge, const std::vector<godot::Vector3> &away);
void emit_verge_segment(const RoadEmit &e, int sid);
void emit_verge_node(const RoadEmit &e, int node);

} // namespace city
