// The lots the player draws (Roadcraft world/lots.ts, lotRecord.ts): land of any shape, its first side the front on the street, its corners in
// the order a building reads them. They are saved and stand beside the lots the streets propose (lot_cache.h), which give way to them.
#pragma once

#include <vector>

namespace city {

struct LotPoint {
	double x = 0, z = 0;
};

struct PlayerLot {
	int id = 0;
	std::vector<LotPoint> corners; // front start, front end, then round: the back on the left of the front
	int zone = 0; // 0: not zoned
};

// Where a building can stand on a lot: the middle of its rectangle, its width along the front, its depth and which way it faces.
struct LotStand {
	double x = 0, z = 0, w = 0, d = 0, rot = 0;
};

struct LotBook {
	std::vector<PlayerLot> lots;
	int next_id = 1;
	void clear() {
		lots.clear();
		next_id = 1;
	}
	bool empty() const { return lots.empty(); }
	PlayerLot *find(int id);
	const PlayerLot *find(int id) const;
	// The lot a point is in; 0 on none.
	int at(double x, double z) const;
};

// How a lot is cut: across the front, parallel to it, or along a line of two points.
enum class LotCutKind { ACROSS, ALONG, LINE };
struct LotCut {
	LotCutKind kind = LotCutKind::ACROSS;
	int parts = 2;
	LotPoint a, b;
};

double lot_area(const std::vector<LotPoint> &ring);
bool inside_lot(const LotPoint &p, const std::vector<LotPoint> &ring);
// The ring turned so its inside lies on the left of every side, the first side kept the front.
std::vector<LotPoint> facing_corners(const std::vector<LotPoint> &ring);
LotStand lot_stand(const std::vector<LotPoint> &ring);
// The rectangle from a to b square to `angle`, empty when under the least lot.
std::vector<LotPoint> lot_rect(const LotPoint &a, const LotPoint &b, double angle);
// The lines a cut runs along, in pairs of points: what the tool shows.
std::vector<LotPoint> cut_lines(const std::vector<LotPoint> &ring, const LotCut &cut);
// The pieces of a ring after a cut, each with its front; fewer than two when nothing worth a lot is left on a side.
std::vector<std::vector<LotPoint>> cut_lot(const std::vector<LotPoint> &ring, const LotCut &cut);
// Two rings that share a side as one, the first's front kept; empty when they share none.
std::vector<LotPoint> joined_lots(const std::vector<LotPoint> &a, const std::vector<LotPoint> &b);
// Whether a ring lies deep in another (a corner or the middle of one well inside the other).
bool lots_overlap(const std::vector<LotPoint> &a, const std::vector<LotPoint> &b);
bool big_enough(const std::vector<LotPoint> &ring);
// The ring in triangles by ear clipping, three indices each: a lot may be concave.
std::vector<int> lot_triangles(const std::vector<LotPoint> &ring);
// A piece of a cut lot with its front: the side along the old front fa -> fb, or the side nearest it.
std::vector<LotPoint> piece_with_front(const std::vector<LotPoint> &piece, const LotPoint &fa, const LotPoint &fb);
// The side from a to b bent through `through`: the points between them, a quadratic curve.
std::vector<LotPoint> curved_side(const LotPoint &a, const LotPoint &b, const LotPoint &through);
// What the corners near `at` take: the points move to `to`.
bool lot_point_near(const LotPoint &p, const LotPoint &q, double reach);

} // namespace city
