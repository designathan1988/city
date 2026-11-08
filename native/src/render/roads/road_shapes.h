// The shapes two road jobs both draw from, made once for the city's revision (render/kept_by_revision.h) instead of in each job: a
// node's ends and the kerb line of each corner between them (the road jobs pave and kerb it),
// and a segment's runs of retaining wall on each side (the structure job builds them, the backfill job covers the ground behind them).
#pragma once

#include "render/kept_by_revision.h"
#include "render/roads/street_shapes.h"
#include "sim/catalog/profile_types.h"
#include "sim/roads/geometry.h"

#include <godot_cpp/variant/vector3.hpp>

#include <cstddef>
#include <memory>
#include <vector>

namespace city {

// An arm's mouth at a node.
struct RoadEnd {
	godot::Vector3 p, d; // mouth centre, outward unit direction
	Arm arm;
	double at = 0; // setback
	const Profile *prof = nullptr;
	bool at_a = true;
};

// A node's ends round it by angle; for a plate, each corner between neighbours: its kerb line (`in`, from an end's left to the next
// one's right) and its outer line (`out`, sharpened to the tip where the two footways meet; `tip` its index, 0 for none).
struct NodeShape {
	struct Corner {
		std::vector<godot::Vector3> in, out;
		size_t tip = 0;
	};
	std::vector<RoadEnd> ends;
	std::vector<Corner> corners;
};

// A section of a retaining wall: its foot on the casing's edge, the way out of the road, the road's height and the ground it holds.
struct WallSection {
	godot::Vector3 at, out;
	float deck = 0, top = 0;
};
// A segment's runs of wall on the right (side -1: index 0) and the left (side +1: index 1), each as its sections.
struct SegWalls {
	std::vector<std::vector<WallSection>> runs[2];
};

// A point across an end's mouth: `u` to its left.
inline godot::Vector3 side(const RoadEnd &e, double u) { return e.p + godot::Vector3(e.d.z, 0, -e.d.x) * float(u); }
// The line `w` off a kerb line from a to b, away from the node (wa at a's end, wb at b's).
std::vector<godot::Vector3> outside(const std::vector<godot::Vector3> &line, const RoadEnd &a, const RoadEnd &b, float wa, float wb);
// Whether a node of several arms is drawn as a plate: one of its mouths stands back from it.
bool is_plate(const NodeOutlineShape &K, const std::vector<RoadEnd> &ends);
constexpr double FILLET_SLACK = 0.05; // how far short of its fillet's trim an end may stand and still turn its corner on it

struct RoadEmit;

struct RoadShapes {
	static constexpr size_t NODES_KEPT = 16384, SEGMENTS_KEPT = 16384; // each counted as one
	RoadShapes() {
		nodes.set_budget(NODES_KEPT);
		walls.set_budget(SEGMENTS_KEPT);
	}
	KeptByRevision<NodeShape> nodes;
	KeptByRevision<SegWalls> walls;
};

// The node's shape for the city's revision: its ends, and for a plate each corner's kerb and outer lines.
std::shared_ptr<const NodeShape> node_shape(const RoadEmit &e, int n);

} // namespace city
