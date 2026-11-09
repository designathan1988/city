// The brush the road's paint is laid with, in the segment's own coordinates (u across, positive to the left looking from node a
// to node b; v metres along from node a): a rectangle that follows the road's surface, and a stroke along a polyline. The lines,
// crossings and parking (road_markings.cpp) and the words, symbols, arrows and islands on the floor (road_legends.cpp) share it.
#pragma once

#include "render/roads/road_mesh.h"

#include <godot_cpp/variant/vector2.hpp>

#include <vector>

namespace city {

struct RoadPainter {
	const RoadEmit &e;
	int sid;
	LampRow lamp;

	// The lamp pool a point of the paint lies in (UV2), none where no lamp lights the road.
	godot::Vector2 pool(float u, float at) const;
	// A rectangle u0..u1 across, va..vb metres along from node a, on the stations that keep it on the road's surface.
	void rect(float u0, float u1, double va, double vb, const godot::Color &c) const;
	// A line `width` wide through `points` (x = u, y = v), one quad from each point to the next.
	void stroke(const std::vector<godot::Vector2> &points, float width, const godot::Color &c) const;
};

// What is painted on the floor of segment `sid` beyond the lines: the turn arrows of its lanes, the words of its bus lanes between
// l0 and l1, and before each stop line (end a when `ca`, end b when `cb`) the word or symbol of the leg's rule.
void floor_paint(const RoadPainter &pt, const Profile &p, double v0, double v1, bool ca, bool cb, double l0, double l1);
// The paint of a node's own plate: a mini-roundabout's island.
void emit_node_paint(const RoadEmit &e, int node);

} // namespace city
