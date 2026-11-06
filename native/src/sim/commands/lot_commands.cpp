// The Zones > Lots tab (Roadcraft editor/lotTool.ts, world/lots.ts): lots drawn, moved, bent, cut, joined and zoned. Each edit replaces the book through
// the journal; the cells it touches are read again, so what grows there follows.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "core/grid.h"
#include "sim/lots/lot_book.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double CORNER_REACH_M = 0.8, SIDE_REACH_M = 1.2, ROAD_LOOK_M = 8;

void bounds(const std::vector<LotPoint> &q, double &x0, double &z0, double &x1, double &z1) {
	for (const LotPoint &p : q) {
		x0 = std::min(x0, p.x);
		z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x);
		z1 = std::max(z1, p.z);
	}
}

// The book replaced, and the land of `rings` read again.
EditResult commit(CityState &s, LotBook next, const std::vector<std::vector<LotPoint>> &rings) {
	double x0 = 1e18, z0 = 1e18, x1 = -1e18, z1 = -1e18;
	for (const std::vector<LotPoint> &q : rings) bounds(q, x0, z0, x1, z1);
	j_lots(s, std::move(next));
	for (int j = int(std::floor(z0 / CHUNK)); j <= int(std::floor(z1 / CHUNK)); j++)
		for (int i = int(std::floor(x0 / CHUNK)); i <= int(std::floor(x1 / CHUNK)); i++) s.log.lot_cells.push_back(cell_key(i, j));
	finish_edit(s, x0, z0, x1, z1);
	EditResult r;
	r.ok = true;
	return r;
}

EditResult refused(const char *why) {
	EditResult r;
	r.error = why;
	return r;
}

// A lot stands on land: no road runs through it.
bool on_land(const CityState &s, const std::vector<LotPoint> &q) {
	double x0 = 1e18, z0 = 1e18, x1 = -1e18, z1 = -1e18;
	bounds(q, x0, z0, x1, z1);
	bool clear = true;
	s.roads.for_segments_near(x0, z0, x1, z1, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const int steps = std::max(2, int(g.len / ROAD_LOOK_M));
		for (int k = 0; k <= steps && clear; k++) {
			double x = 0, z = 0;
			g.q.at(double(k) / steps, x, z);
			if (inside_lot({ x, z }, q)) clear = false;
		}
	});
	return clear;
}

bool free_of_lots(const LotBook &book, const std::vector<LotPoint> &q) {
	for (const PlayerLot &l : book.lots)
		if (lots_overlap(q, l.corners)) return false;
	return true;
}

// What stands on a ring goes with it.
void raze(CityState &s, const std::vector<LotPoint> &q, EditResult &r) {
	double x0 = 1e18, z0 = 1e18, x1 = -1e18, z1 = -1e18;
	bounds(q, x0, z0, x1, z1);
	std::vector<int> ids;
	s.buildings.query(float(x0), float(z0), float(x1), float(z1), [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (inside_lot({ b.x, b.z }, q)) ids.push_back(id);
	});
	for (int id : ids) {
		j_remove_building(s, id);
		r.buildings_removed++;
	}
}

EditResult add_lot(CityState &s, const std::vector<LotPoint> &ring) {
	if (!big_enough(ring)) return refused("hint.lot.small");
	const std::vector<LotPoint> corners = facing_corners(ring);
	if (!on_land(s, corners)) return refused("hint.lot.road");
	if (!free_of_lots(s.lot_book, corners)) return refused("hint.lot.overlap");
	LotBook next = s.lot_book;
	PlayerLot lot;
	lot.id = next.next_id++;
	lot.corners = corners;
	next.lots.push_back(lot);
	EditResult r = commit(s, std::move(next), { corners });
	r.placed = lot.id;
	return r;
}
} // namespace

EditResult add_lot_rect(CityState &s, const LotPoint &a, const LotPoint &b, double angle) { return add_lot(s, lot_rect(a, b, angle)); }

EditResult add_lot_polygon(CityState &s, const std::vector<LotPoint> &points, int front) {
	if (points.size() < 3) return refused("hint.lot.small");
	std::vector<LotPoint> ring(points);
	std::rotate(ring.begin(), ring.begin() + std::ptrdiff_t(size_t(front) % ring.size()), ring.end());
	return add_lot(s, ring);
}

EditResult move_lot_corner(CityState &s, const LotPoint &from, const LotPoint &to) {
	LotBook next = s.lot_book;
	std::vector<std::vector<LotPoint>> touched;
	for (PlayerLot &l : next.lots) {
		bool moved = false;
		std::vector<LotPoint> before = l.corners;
		for (LotPoint &q : l.corners)
			if (lot_point_near(q, from, CORNER_REACH_M)) {
				q = to;
				moved = true;
			}
		if (moved) {
			touched.push_back(before);
			touched.push_back(l.corners);
		}
	}
	return touched.empty() ? refused("hint.lot.pick") : commit(s, std::move(next), touched);
}

EditResult curve_lot_side(CityState &s, const LotPoint &a, const LotPoint &b, const LotPoint &through) {
	LotBook next = s.lot_book;
	std::vector<std::vector<LotPoint>> touched;
	for (PlayerLot &l : next.lots) {
		std::vector<LotPoint> &q = l.corners;
		for (size_t k = 0; k < q.size(); k++) {
			const LotPoint p = q[k], r = q[(k + 1) % q.size()];
			const bool same = lot_point_near(p, a, SIDE_REACH_M) && lot_point_near(r, b, SIDE_REACH_M);
			if (!same && !(lot_point_near(p, b, SIDE_REACH_M) && lot_point_near(r, a, SIDE_REACH_M))) continue;
			std::vector<LotPoint> bend = curved_side(a, b, through);
			if (!same) std::reverse(bend.begin(), bend.end());
			q.insert(q.begin() + std::ptrdiff_t(k + 1), bend.begin(), bend.end());
			touched.push_back(q);
			break;
		}
	}
	return touched.empty() ? refused("hint.lot.pick") : commit(s, std::move(next), touched);
}

EditResult set_lot_front(CityState &s, int id, int side) {
	LotBook next = s.lot_book;
	PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	const int n = int(l->corners.size()), k = ((side % n) + n) % n;
	if (k == 0) return refused("hint.lot.same");
	EditResult razed;
	raze(s, l->corners, razed); // the building faced the old front
	std::rotate(l->corners.begin(), l->corners.begin() + k, l->corners.end());
	const std::vector<LotPoint> ring = l->corners;
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult split_lot(CityState &s, int id, const LotCut &cut) {
	LotBook next = s.lot_book;
	const PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<std::vector<LotPoint>> pieces = cut_lot(l->corners, cut);
	if (pieces.size() < 2) return refused("hint.lot.small");
	const PlayerLot old = *l;
	EditResult razed;
	raze(s, old.corners, razed);
	next.lots.erase(std::remove_if(next.lots.begin(), next.lots.end(), [&](const PlayerLot &x) { return x.id == id; }), next.lots.end());
	for (const std::vector<LotPoint> &piece : pieces) {
		PlayerLot made;
		made.id = next.next_id++;
		made.corners = piece;
		made.zone = old.zone;
		next.lots.push_back(made);
	}
	EditResult r = commit(s, std::move(next), { old.corners });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult join_lots(CityState &s, int a, int b) {
	LotBook next = s.lot_book;
	const PlayerLot *la = next.find(a), *lb = next.find(b);
	if (!la || !lb || a == b) return refused("hint.lot.pick");
	const std::vector<LotPoint> ring = joined_lots(la->corners, lb->corners);
	if (ring.empty()) return refused("hint.lot.apart");
	const PlayerLot first = *la, second = *lb;
	EditResult razed;
	raze(s, first.corners, razed);
	raze(s, second.corners, razed);
	next.lots.erase(std::remove_if(next.lots.begin(), next.lots.end(), [&](const PlayerLot &x) { return x.id == a || x.id == b; }), next.lots.end());
	PlayerLot made;
	made.id = next.next_id++;
	made.corners = ring;
	made.zone = first.zone;
	next.lots.push_back(made);
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult delete_lot(CityState &s, int id) {
	LotBook next = s.lot_book;
	const PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<LotPoint> ring = l->corners;
	EditResult razed;
	raze(s, ring, razed);
	next.lots.erase(std::remove_if(next.lots.begin(), next.lots.end(), [&](const PlayerLot &x) { return x.id == id; }), next.lots.end());
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult zone_lot(CityState &s, int id, int zone) {
	LotBook next = s.lot_book;
	PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	if (l->zone == zone) return refused("tools.toast.zoneSame");
	EditResult razed;
	if (l->zone != 0) raze(s, l->corners, razed); // what grew for the old zone goes
	l->zone = zone;
	const std::vector<LotPoint> ring = l->corners;
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

} // namespace city
