// The geometry of the lots on the ground (x, z): a ring's area and inside, where a building stands on it, the rectangle a drag draws,
// cuts, joins, overlaps, triangles and curved sides. Plain rings: the book of the player's lots (lot_book.h) and the tools read them.
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

struct GenShapes; // the generator's shapes (sim/catalog/gen_shapes.h, reached through the state): the least lot and the lot rings' rules

// Where a building can stand on a lot: the middle of its rectangle, its width along the front, its depth and which way it faces.
struct LotStand {
	double x = 0, z = 0, w = 0, d = 0, rot = 0;
};

// How a lot is cut: across the front, parallel to it, or along a line of two points.
enum class LotCutKind { ACROSS, ALONG, LINE };
struct LotCut {
	LotCutKind kind = LotCutKind::ACROSS;
	int parts = 2;
	XZ a, b;
};

double lot_area(const std::vector<XZ> &ring);
bool inside_lot(const XZ &p, const std::vector<XZ> &ring);
// The ring turned so its inside lies on the left of every side, the first side kept the front.
std::vector<XZ> facing_corners(const std::vector<XZ> &ring);
LotStand lot_stand(const std::vector<XZ> &ring);
// Whether a polygon lies wholly in a ring: its corners inside and no side of one crossing a side of the other (a ring may be concave).
bool ring_holds(const std::vector<XZ> &ring, const std::vector<XZ> &shape);
// The rectangle from a to b square to `angle`, empty when under the least lot.
std::vector<XZ> lot_rect(const GenShapes &S, const XZ &a, const XZ &b, double angle);
// The lines a cut runs along, in pairs of points: what the tool shows.
std::vector<XZ> cut_lines(const GenShapes &S, const std::vector<XZ> &ring, const LotCut &cut);
// The pieces of a ring after a cut, each with its front; fewer than two when nothing worth a lot is left on a side.
std::vector<std::vector<XZ>> cut_lot(const GenShapes &S, const std::vector<XZ> &ring, const LotCut &cut);
// Two rings that share a side as one, the first's front kept; empty when they share none.
std::vector<XZ> joined_lots(const GenShapes &S, const std::vector<XZ> &a, const std::vector<XZ> &b);
// Whether a ring lies deep in another (a corner or the middle of one well inside the other).
bool lots_overlap(const GenShapes &S, const std::vector<XZ> &a, const std::vector<XZ> &b);
bool big_enough(const GenShapes &S, const std::vector<XZ> &ring);
// The ring in triangles by ear clipping, three indices each: a lot may be concave.
std::vector<int> lot_triangles(const std::vector<XZ> &ring);
// The ring in triangles cut to the cells of a square grid `cell` metres a side with a corner at `origin`, three points each. Cut on
// the relief's own grid (Heightfield: the terrain's mesh has a vertex at each of its samples) and laid on it point by point, a lot's
// colour has its corners on the terrain's and is flat only within one of its cells.
std::vector<XZ> lot_cover(const std::vector<XZ> &ring, double cell, const XZ &origin);
// A piece of a cut lot with its front: the side along the old front fa -> fb, or the side nearest it.
std::vector<XZ> piece_with_front(const GenShapes &S, const std::vector<XZ> &piece, const XZ &fa, const XZ &fb);
// The side from a to b bent through `through`: the points between them, a quadratic curve.
std::vector<XZ> curved_side(const XZ &a, const XZ &b, const XZ &through);
// What the corners near `at` take: the points move to `to`.
bool lot_point_near(const XZ &p, const XZ &q, double reach);

} // namespace city
