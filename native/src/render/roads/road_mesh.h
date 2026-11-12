// The road mesher's pieces: segment cross-sections (road_segment.cpp), junction plates,
// kerb returns and road ends (road_junction.cpp), painted markings (road_markings.cpp). All of them append to one
// MeshBuilder per chunk with the vertex conventions of road_look.h.
#pragma once

#include "render/mesh_builder.h"
#include "render/roads/kerb_drop.h"
#include "render/roads/road_look.h"
#include "sim/roads/street_furniture.h"
#include "sim/state/city_view.h"

namespace city {

constexpr float STRAIGHT_TOL = 0.5f; // metres off its chord under which a segment is drawn as a straight one

struct RoadEmit {
	const CityView s; // the city the pieces read: a job's cut of it, or the live one of a reader that holds the map
	const StreetLook &L;
	MeshBuilder &mb;
	bool far = false; // the far level: the pieces' top faces alone, on a longer step
};

// Position on the centre line at arc length `at` (height = RoadNetwork::seg_y), unit forward and left vectors.
struct SegFrame {
	godot::Vector3 c, fwd, left;
};
SegFrame seg_frame(const CityView &s, int sid, double at);

// A node whose mouths get crossings, stop lines and dropped kerbs (three or more arms, not a roundabout or rails).
bool crossing_node(const CityView &s, int node);
bool crosswalk_at(const CityView &s, int node, int seg); // a crosswalk over this arm there; a crossing between junctions has a stop line on both

// Four corners with their own uv (texture metres), flat normal facing `out`.
void emit_quad(MeshBuilder &mb, const godot::Vector3 p[4], const godot::Vector2 uv[4], const godot::Color &col, const godot::Vector3 &out);
void emit_quad(MeshBuilder &mb, const godot::Vector3 p[4], const godot::Vector2 uv[4], const godot::Color cols[4], const godot::Vector3 &out);

void emit_segment(const RoadEmit &e, int sid);
void emit_markings(const RoadEmit &e, int sid);
void emit_node(const RoadEmit &e, int node);
// All a chunk owns: its segments with their paint and its nodes.
void emit_chunk(const RoadEmit &e, int64_t chunk);

// How a segment's surface is laid (road_segment.cpp), arcs from its node a: its carriageway's mouths v0 and v1, where each side's
// footway starts and ends (index STREET_RIGHT its -u, STREET_LEFT its +u: at the carriageway's mouths, or on past them where a
// junction's corner runs the footway on, sim/roads/junction_corner.h), its lowered kerbs, its island's noses (v0 and v1 when it has
// none) and every arc it is drawn at; each mouth and nose is one of those arcs.
struct SurfaceLayout {
	double v0 = 0, v1 = 0, lo = 0, hi = 0;
	double walk_from[2] = { 0, 0 }, walk_to[2] = { 0, 0 };
	KerbDrops drops;
	std::vector<double> arcs;
};
SurfaceLayout surface_layout(const RoadEmit &e, int sid);
// The section across one side's footway at arc `at` (side STREET_RIGHT or STREET_LEFT): from the kerb stone's back out to the outer
// edge, each piece's edge (u across the road) with its top's height there over the centre line (kerb_drop.h).
struct SectionPoint {
	float u = 0, h = 0;
};
std::vector<SectionPoint> footway_section(const RoadEmit &e, int sid, const SurfaceLayout &lay, int side, double at);
// Where each side's footway starts (`from`) and ends (`to`) along segment `sid`, as the corners of its two nodes say (road_corner.cpp).
void footway_mouths(const RoadEmit &e, int sid, double from[2], double to[2]);
// The junction corners of the city as the probe counts them (CityProbes::road_overlaps): how many, of which back (junction_corner.h
// CornerBack), whose outline is not a simple polygon, whose drawn footway's triangles leave part of it out, and the first such.
struct CornerCensus {
	int corners = 0, crossed = 0, holed = 0, kinds[3] = { 0, 0, 0 };
	int first_node = -1;
	std::vector<godot::Vector3> first_ring;
};
void corner_census(const RoadEmit &e, int node, CornerCensus &c);

// The kerb along a corner's or a turning bulb's kerb line `in`, its stone's back `stone`: its face from the carriageway up to `top`,
// and with `has_stone` the stone's top and its step down to the footway at `walk` (road_kerb.cpp).
void kerb_band(const RoadEmit &e, const std::vector<godot::Vector3> &in, const std::vector<godot::Vector3> &stone, float top, float walk, bool has_stone);
// A footway's patch, the polygon `ring` (its points at their heights) cut into triangles, worn by each point's distance from its inner
// side `inner` (by the kerb) and from its back `back`.
void footway_patch(const RoadEmit &e, const std::vector<godot::Vector3> &ring, const std::vector<godot::Vector3> &inner, const std::vector<godot::Vector3> &back);
// The face down from a footway's back to under the ground, turned away from its inner side.
void footway_skirt(const RoadEmit &e, const std::vector<godot::Vector3> &back, const std::vector<godot::Vector3> &inner, const godot::Color &walk);

// The ground round the road (road_verge.cpp): from `edge`, a line along the paving's outer top, out along `away` (unit,
// flat) down to the terrain. emit_verge_node draws it round a node's corners and ends.
void verge_ribbon(const RoadEmit &e, const std::vector<godot::Vector3> &edge, const std::vector<godot::Vector3> &away);
void emit_verge_segment(const RoadEmit &e, int sid);
void emit_verge_node(const RoadEmit &e, int node);
// What of a segment is not its surface: the deck, bents and parapets where it is raised, the portals where it goes under (road_structures.cpp).
void emit_structures(const RoadEmit &e, int sid);
// How far past a segment's casing its structures read the ground and the roads (a portal's wings and the ground before and
// behind them, a column's flared foot), and a stayed bridge's legs: what a job's cut holds round the segment (road_cut.h).
float structure_reach(const Tuning &tn);
float stays_reach();
godot::Color structure_concrete();
void structure_face(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &d, const godot::Color &col, const godot::Vector3 &out);
// The retaining walls of a cutting and the backfill behind them (road_walls.cpp); `walled_at` says where one stands, on side +1 or -1.
bool walled_at(const RoadEmit &e, const RoadSegment &g, const SegFrame &f, float side);
void emit_retaining_walls(const RoadEmit &e, int sid);
void emit_wall_backfill(const RoadEmit &e, int sid);

// How many stretches a segment's surface is drawn in between v0 and v1: one for a road straight and on one grade, else by the look's step.
int surface_steps(const RoadEmit &e, int sid, double v0, double v1);
// The towers and cables of a long bridge (road_stays.cpp), on the stretch [v0, v1] of a segment; `stayed`: a street held by them stands on no bents.
bool stayed(const RoadEmit &e, int sid);
void emit_stays(const RoadEmit &e, int sid, double v0, double v1, float casing);

} // namespace city
