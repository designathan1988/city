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
	bool far = false; // the far level: the pieces' top faces alone, on a longer step
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
// All a chunk owns: its segments with their paint and its nodes.
void emit_chunk(const RoadEmit &e, int64_t chunk);

// A kerbed edge along three lines, inner, back of the kerb and the lot's. `across` is where the line stands on the road's section.
void kerbed_line(const RoadEmit &e, const std::vector<godot::Vector3> &in, const std::vector<godot::Vector3> &bk, const std::vector<godot::Vector3> &out,
		float across, bool kerbed, const godot::Color &walk);

// The ground round the road (road_verge.cpp): from `edge`, a line along the paving's outer top, out along `away` (unit,
// flat) down to the terrain. emit_verge_node draws it round a node's corners and ends.
void verge_ribbon(const RoadEmit &e, const std::vector<godot::Vector3> &edge, const std::vector<godot::Vector3> &away);
void emit_verge_segment(const RoadEmit &e, int sid);
void emit_verge_node(const RoadEmit &e, int node);
// What of a segment is not its surface: the deck, bents and parapets where it is raised, the portals where it goes under (road_structures.cpp).
void emit_structures(const RoadEmit &e, int sid);
godot::Color structure_concrete();
void structure_face(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &d, const godot::Color &col, const godot::Vector3 &out);
// The retaining walls of a cutting and the backfill behind them (road_walls.cpp); `walled_at` says where one stands, on side +1 or -1.
bool walled_at(const RoadEmit &e, const RoadSegment &g, const SegFrame &f, float side);
void emit_retaining_walls(const RoadEmit &e, int sid);
void emit_wall_backfill(const RoadEmit &e, int sid);

// How many stretches a segment's surface is drawn in between v0 and v1: one for a road straight and on one grade, else by the look's step.
int surface_steps(const RoadEmit &e, int sid, double v0, double v1);
// How far along its street a segment starts: a road is laid in pieces, and what runs along it does not start again at each.
double street_start(const RoadNetwork &net, int sid);

} // namespace city
