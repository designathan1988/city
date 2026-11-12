// The lots the player draws (Roadcraft world/lots.ts, lotRecord.ts): land of any shape, its first side the front on the street, its corners in
// the order a building reads them. They are saved and stand beside the lots the streets propose (lot_cache.h), which give way to them.
#pragma once

#include "core/table_delta.h"
#include "core/bounds_index.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct LotPoint {
	double x = 0, z = 0;
	bool operator==(const LotPoint &o) const { return x == o.x && z == o.z; }
};

struct PlayerLot {
	int id = 0;
	std::vector<LotPoint> corners; // front start, front end, then round: the back on the left of the front
	int zone = 0; // 0: not zoned
	bool operator==(const PlayerLot &o) const { return id == o.id && zone == o.zone && corners == o.corners; }
};

// Where a building can stand on a lot: the middle of its rectangle, its width along the front, its depth and which way it faces.
struct LotStand {
	double x = 0, z = 0, w = 0, d = 0, rot = 0;
};

struct LotGeometry {
	Bounds2 bounds;
	LotStand stand;
	std::vector<int> triangles;
};

class LotBook {
public:
	int next_id = 1;
	void clear();
	bool empty() const { return lots_.empty(); }
	const std::vector<PlayerLot> &records() const { return lots_; }
	const PlayerLot *find(int id) const;
	const LotGeometry *geometry(int id) const;
	void restore(std::vector<PlayerLot> lots, int next);
	void apply(const TableDelta<PlayerLot> &change, bool forward);
	std::vector<int> overlapping(const Bounds2 &bounds) const; // record order, same tie precedence as the book
	std::vector<int> stands_in(int64_t cell) const;
	// The lot a point is in; 0 on none.
	int at(double x, double z) const;

private:
	std::vector<PlayerLot> lots_;
	std::unordered_map<int, size_t> positions_;
	std::unordered_map<int, LotGeometry> geometry_;
	std::unordered_map<int64_t, std::vector<int>> stand_cells_;
	BoundsIndex bounds_;
	void prepare(const PlayerLot &lot);
	void forget(int id);
	void order(std::vector<int> &ids) const;
};

struct LotsChange {
	int next_before = 1, next_after = 1;
	TableDelta<PlayerLot> lots;
	explicit LotsChange(int next = 1) : next_before(next), next_after(next) {}
	bool empty() const { return lots.empty() && next_before == next_after; }
	void apply(LotBook &book, bool forward) const {
		book.apply(lots, forward);
		book.next_id = forward ? next_after : next_before;
	}
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
