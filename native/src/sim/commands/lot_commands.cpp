// The Zones > Lots tab: lots drawn, moved, bent, cut, joined and zoned. Each command journals only changed records;
// the cells it touches are read again, so what grows there follows.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/lot_draft.h"
#include "sim/commands/road_edit.h"
#include "core/grid.h"
#include "sim/lots/lot_book.h"
#include "sim/citygen/lot_land.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

static constexpr double FAR_OFF_M = 1e18; // a box before its first point

namespace {
constexpr double CORNER_TAKE_M = 0.8, SIDE_REACH_M = 1.2;

void bounds(const std::vector<LotPoint> &q, double &x0, double &z0, double &x1, double &z1) {
	for (const LotPoint &p : q) {
		x0 = std::min(x0, p.x);
		z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x);
		z1 = std::max(z1, p.z);
	}
}

// Only changed records committed, and the land of `rings` read again.
EditResult commit(CityState &s, LotDraft next, const std::vector<std::vector<LotPoint>> &rings) {
	LotsChange change = next.take();
	if (change.empty()) { EditResult r; r.ok = true; return r; }
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
	for (const std::vector<LotPoint> &q : rings) bounds(q, x0, z0, x1, z1);
	j_lots(s, std::move(change));
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

bool free_of_lots(const LotBook &book, const std::vector<LotPoint> &q) {
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
	bounds(q, x0, z0, x1, z1);
	for (int id : book.overlapping({ x0, z0, x1, z1 }))
		if (lots_overlap(q, book.find(id)->corners)) return false;
	return true;
}

// What stands on a ring goes with it.
void raze(CityState &s, const std::vector<LotPoint> &q, EditResult &r) {
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
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
	std::vector<LotPoint> land = lot_on_land(s.roads, ring);
	if (land.empty()) return refused("hint.lot.road");
	std::rotate(land.begin(), land.begin() + lot_front_side(s.roads, land), land.end());
	const std::vector<LotPoint> corners = facing_corners(land);
	if (!free_of_lots(s.lot_book, corners)) return refused("hint.lot.overlap");
	LotDraft next(s.lot_book);
	PlayerLot lot;
	lot.id = next.next_id++;
	lot.corners = corners;
	next.add(lot);
	EditResult r = commit(s, std::move(next), { corners });
	r.placed = lot.id;
	return r;
}
} // namespace

EditResult add_lot_rect(CityState &s, const LotPoint &a, const LotPoint &b, double angle) { return add_lot(s, lot_rect(a, b, angle)); }

EditResult add_lot_polygon(CityState &s, const std::vector<LotPoint> &points) { return add_lot(s, points); }

void cut_lots_by_roads(CityState &s, double minx, double minz, double maxx, double maxz) {
	LotDraft next(s.lot_book);
	std::vector<std::vector<LotPoint>> touched;
	for (int id : s.lot_book.overlapping({ minx, minz, maxx, maxz })) {
		const PlayerLot &lot = *s.lot_book.find(id);
		bool crossed = false;
		const std::vector<std::vector<LotPoint>> pieces = lot_pieces(s.roads, lot.corners, crossed);
		if (!crossed) continue;
		EditResult razed;
		raze(s, lot.corners, razed);
		next.remove(lot.id);
		for (const std::vector<LotPoint> &piece : pieces) {
			PlayerLot made;
			made.id = next.next_id++;
			made.corners = facing_corners(piece_with_front(piece, lot.corners[0], lot.corners[1]));
			made.zone = lot.zone;
			next.add(made);
		}
		touched.push_back(lot.corners);
	}
	if (!touched.empty()) commit(s, std::move(next), touched);
}

EditResult move_lot_corner(CityState &s, const LotPoint &from, const LotPoint &to) {
	LotDraft next(s.lot_book);
	std::vector<std::vector<LotPoint>> touched;
	for (int id : s.lot_book.overlapping({ from.x - CORNER_TAKE_M, from.z - CORNER_TAKE_M, from.x + CORNER_TAKE_M, from.z + CORNER_TAKE_M })) {
		const PlayerLot &original = *s.lot_book.find(id);
		if (std::none_of(original.corners.begin(), original.corners.end(), [&](const LotPoint &q) { return lot_point_near(q, from, CORNER_TAKE_M); })) continue;
		PlayerLot &l = *next.find(original.id);
		bool moved = false;
		std::vector<LotPoint> before = l.corners;
		for (LotPoint &q : l.corners)
			if (lot_point_near(q, from, CORNER_TAKE_M)) {
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
	LotDraft next(s.lot_book);
	std::vector<std::vector<LotPoint>> touched;
	for (int id : s.lot_book.overlapping({ std::min(a.x, b.x) - SIDE_REACH_M, std::min(a.z, b.z) - SIDE_REACH_M,
			std::max(a.x, b.x) + SIDE_REACH_M, std::max(a.z, b.z) + SIDE_REACH_M })) {
		const PlayerLot &original = *s.lot_book.find(id);
		for (size_t k = 0; k < original.corners.size(); k++) {
			const LotPoint p = original.corners[k], r = original.corners[(k + 1) % original.corners.size()];
			const bool same = lot_point_near(p, a, SIDE_REACH_M) && lot_point_near(r, b, SIDE_REACH_M);
			if (!same && !(lot_point_near(p, b, SIDE_REACH_M) && lot_point_near(r, a, SIDE_REACH_M))) continue;
			std::vector<LotPoint> &q = next.find(original.id)->corners;
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
	LotDraft next(s.lot_book);
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
	LotDraft next(s.lot_book);
	const PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<std::vector<LotPoint>> pieces = cut_lot(l->corners, cut);
	if (pieces.size() < 2) return refused("hint.lot.small");
	const PlayerLot old = *l;
	EditResult razed;
	raze(s, old.corners, razed);
	next.remove(id);
	for (const std::vector<LotPoint> &piece : pieces) {
		PlayerLot made;
		made.id = next.next_id++;
		made.corners = piece;
		made.zone = old.zone;
		next.add(made);
	}
	EditResult r = commit(s, std::move(next), { old.corners });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult join_lots(CityState &s, int a, int b) {
	LotDraft next(s.lot_book);
	const PlayerLot *la = next.find(a), *lb = next.find(b);
	if (!la || !lb || a == b) return refused("hint.lot.pick");
	const std::vector<LotPoint> ring = joined_lots(la->corners, lb->corners);
	if (ring.empty()) return refused("hint.lot.apart");
	const PlayerLot first = *la, second = *lb;
	EditResult razed;
	raze(s, first.corners, razed);
	raze(s, second.corners, razed);
	next.remove(a);
	next.remove(b);
	PlayerLot made;
	made.id = next.next_id++;
	made.corners = ring;
	made.zone = first.zone;
	next.add(made);
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult delete_lot(CityState &s, int id) {
	LotDraft next(s.lot_book);
	const PlayerLot *l = next.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<LotPoint> ring = l->corners;
	EditResult razed;
	raze(s, ring, razed);
	next.remove(id);
	EditResult r = commit(s, std::move(next), { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult zone_lot(CityState &s, int id, int zone) {
	LotDraft next(s.lot_book);
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
