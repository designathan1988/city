// The road mesher's pieces: segment cross-sections (road_segment.cpp), junction plates,
// kerb returns and road ends (road_junction.cpp), painted markings (road_markings.cpp). All of them append to one
// MeshBuilder per chunk with the vertex conventions of road_look.h.
#pragma once

#include "render/mesh_builder.h"
#include "render/roads/road_look.h"
#include "render/roads/road_shapes.h"
#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

namespace city {

struct RoadEmit {
	const CityState &s;
	const StreetLook &L;
	MeshBuilder &mb;
	RoadShapes &shapes; // the nodes' and walls' shapes the road jobs share (road_shapes.h)
	bool far = false; // the far level: the pieces' top faces alone, on a longer step
};

// Position on the centre line at arc length `at` (height = RoadNetwork::seg_y), unit forward and left vectors.
struct SegFrame {
	godot::Vector3 c, fwd, left;
};
SegFrame seg_frame(const CityState &s, int sid, double at);

// A node whose mouths get crossings, stop lines and dropped kerbs (three or more arms, not a roundabout or rails).

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

// The arcs a segment's surface is drawn at between v0 and v1, both included: as few as keep the mesh within the level's surface
// error of the true road (StreetLook::surface_error, far_surface_error), in plan, height and across the section; two for a road
// straight and on one grade. The surface, its verge and its paint stand on these.
std::vector<double> segment_stations(const RoadEmit &e, int sid, double v0, double v1);
std::vector<double> segment_stations(const CityState &s, const StreetLook &L, bool far, int sid, double v0, double v1);
// The most a mesh on stations `at` stands off the true surface, read densely between them (the stations' own proof).
double stations_error(const CityState &s, int sid, const std::vector<double> &at);
// The towers and cables of a long bridge (road_stays.cpp), on the stretch [v0, v1] of a segment; `stayed`: a street held by them stands on no bents.
bool stayed(const RoadEmit &e, int sid);
void emit_stays(const RoadEmit &e, int sid, double v0, double v1, float casing);

} // namespace city
