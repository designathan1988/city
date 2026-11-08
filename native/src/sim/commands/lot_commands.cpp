// The Zones > Lots tab (Roadcraft editor/lotTool.ts, world/lots.ts): lots drawn, moved, bent, cut, joined and zoned. Each edit changes
// the lots it touches through the journal, one record at a time; the cells it touches are read again, so what grows there follows.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
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

void bounds(const std::vector<XZ> &q, double &x0, double &z0, double &x1, double &z1) {
	for (const XZ &p : q) {
		x0 = std::min(x0, p.x);
		z0 = std::min(z0, p.z);
		x1 = std::max(x1, p.x);
		z1 = std::max(z1, p.z);
	}
}

void mark_box(CityState &s, double x0, double z0, double x1, double z1) {
	for_chunks(x0, z0, x1, z1, [&](int64_t key) { s.log.lot_cells.push_back(key); });
}

// The land of `rings` read again (the book was changed record by record before); the command ends in EditHistory::end.
EditResult commit(CityState &s, const std::vector<std::vector<XZ>> &rings) {
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
	for (const std::vector<XZ> &q : rings) bounds(q, x0, z0, x1, z1);
	mark_box(s, x0, z0, x1, z1);
	EditResult r;
	r.ok = true;
	return r;
}

EditResult refused(const char *why) {
	EditResult r;
	r.error = why;
	return r;
}

bool free_of_lots(const LotBook &book, const std::vector<XZ> &q) {
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
	bounds(q, x0, z0, x1, z1);
	for (int i : book.near(x0, z0, x1, z1)) // two rings deep in each other share their bounds
		if (lots_overlap(q, book.all()[size_t(i)].corners)) return false;
	return true;
}

// What stands on a ring goes with it.
void raze(CityState &s, const std::vector<XZ> &q, EditResult &r) {
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

// A new lot of `zone` on `corners`: its id, the book's next.
int add_piece(CityState &s, const std::vector<XZ> &corners, int zone) {
	PlayerLot made;
	made.corners = corners;
	made.zone = zone;
	return j_add_lot(s, std::move(made));
}

EditResult add_lot(CityState &s, const std::vector<XZ> &ring) {
	if (!big_enough(ring)) return refused("hint.lot.small");
	std::vector<XZ> land = lot_on_land(s.roads, ring);
	if (land.empty()) return refused("hint.lot.road");
	std::rotate(land.begin(), land.begin() + lot_front_side(s.roads, land), land.end());
	const std::vector<XZ> corners = facing_corners(land);
	if (!free_of_lots(s.lot_book, corners)) return refused("hint.lot.overlap");
	const int id = add_piece(s, corners, 0);
	EditResult r = commit(s, { corners });
	r.placed = id;
	return r;
}
} // namespace

void mark_lot_cells(CityState &s, const std::vector<XZ> &ring) {
	double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
	bounds(ring, x0, z0, x1, z1);
	mark_box(s, x0, z0, x1, z1);
}

EditResult add_lot_rect(CityState &s, const XZ &a, const XZ &b, double angle) { return add_lot(s, lot_rect(a, b, angle)); }

EditResult add_lot_polygon(CityState &s, const std::vector<XZ> &points) { return add_lot(s, points); }

void cut_lots_by_roads(CityState &s, double minx, double minz, double maxx, double maxz) {
	struct Crossed {
		PlayerLot lot;
		std::vector<std::vector<XZ>> pieces;
	};
	std::vector<Crossed> crossed; // only the lots a road crosses are copied, before the book changes
	for (int i : s.lot_book.near(minx, minz, maxx, maxz)) {
		const PlayerLot &lot = s.lot_book.all()[size_t(i)];
		double x0 = FAR_OFF_M, z0 = FAR_OFF_M, x1 = -FAR_OFF_M, z1 = -FAR_OFF_M;
		bounds(lot.corners, x0, z0, x1, z1);
		if (x1 < minx || x0 > maxx || z1 < minz || z0 > maxz) continue;
		bool cut = false;
		std::vector<std::vector<XZ>> pieces = lot_pieces(s.roads, lot.corners, cut);
		if (cut) crossed.push_back({ lot, std::move(pieces) });
	}
	if (crossed.empty()) return;
	std::vector<std::vector<XZ>> touched;
	for (const Crossed &c : crossed) {
		EditResult razed;
		raze(s, c.lot.corners, razed);
		j_remove_lot(s, c.lot.id);
		for (const std::vector<XZ> &piece : c.pieces)
			add_piece(s, facing_corners(piece_with_front(piece, c.lot.corners[0], c.lot.corners[1])), c.lot.zone);
		touched.push_back(c.lot.corners);
	}
	commit(s, touched);
}

EditResult move_lot_corner(CityState &s, const XZ &from, const XZ &to) {
	const Tuning &tn = s.catalog.tune;
	std::vector<PlayerLot> moved;
	std::vector<std::vector<XZ>> touched;
	for (int i : s.lot_book.near(from.x - tn.lot_corner_take_m, from.z - tn.lot_corner_take_m, from.x + tn.lot_corner_take_m, from.z + tn.lot_corner_take_m)) {
		const PlayerLot &l = s.lot_book.all()[size_t(i)];
		PlayerLot after = l;
		bool any = false;
		for (XZ &q : after.corners)
			if (lot_point_near(q, from, tn.lot_corner_take_m)) {
				q = to;
				any = true;
			}
		if (!any) continue;
		touched.push_back(l.corners);
		touched.push_back(after.corners);
		moved.push_back(std::move(after));
	}
	if (moved.empty()) return refused("hint.lot.pick");
	for (const PlayerLot &l : moved) j_edit_lot(s, l.id, l);
	return commit(s, touched);
}

EditResult curve_lot_side(CityState &s, const XZ &a, const XZ &b, const XZ &through) {
	const Tuning &tn = s.catalog.tune;
	std::vector<PlayerLot> bent;
	std::vector<std::vector<XZ>> touched;
	// the side's two ends are a and b, either way round: a lot it is on has a corner by a
	for (int i : s.lot_book.near(a.x - tn.lot_side_reach_m, a.z - tn.lot_side_reach_m, a.x + tn.lot_side_reach_m, a.z + tn.lot_side_reach_m)) {
		const PlayerLot &l = s.lot_book.all()[size_t(i)];
		const std::vector<XZ> &q = l.corners;
		for (size_t k = 0; k < q.size(); k++) {
			const XZ p = q[k], r = q[(k + 1) % q.size()];
			const bool same = lot_point_near(p, a, tn.lot_side_reach_m) && lot_point_near(r, b, tn.lot_side_reach_m);
			if (!same && !(lot_point_near(p, b, tn.lot_side_reach_m) && lot_point_near(r, a, tn.lot_side_reach_m))) continue;
			std::vector<XZ> bend = curved_side(a, b, through);
			if (!same) std::reverse(bend.begin(), bend.end());
			PlayerLot after = l;
			after.corners.insert(after.corners.begin() + std::ptrdiff_t(k + 1), bend.begin(), bend.end());
			touched.push_back(after.corners);
			bent.push_back(std::move(after));
			break;
		}
	}
	if (bent.empty()) return refused("hint.lot.pick");
	for (const PlayerLot &l : bent) j_edit_lot(s, l.id, l);
	return commit(s, touched);
}

EditResult set_lot_front(CityState &s, int id, int side) {
	const PlayerLot *found = s.lot_book.find(id);
	if (!found) return refused("hint.lot.pick");
	PlayerLot l = *found;
	const int n = int(l.corners.size()), k = ((side % n) + n) % n;
	if (k == 0) return refused("hint.lot.same");
	EditResult razed;
	raze(s, l.corners, razed); // the building faced the old front
	std::rotate(l.corners.begin(), l.corners.begin() + k, l.corners.end());
	j_edit_lot(s, id, l);
	EditResult r = commit(s, { l.corners });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult split_lot(CityState &s, int id, const LotCut &cut) {
	const PlayerLot *l = s.lot_book.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<std::vector<XZ>> pieces = cut_lot(l->corners, cut);
	if (pieces.size() < 2) return refused("hint.lot.small");
	const PlayerLot old = *l;
	EditResult razed;
	raze(s, old.corners, razed);
	j_remove_lot(s, id);
	for (const std::vector<XZ> &piece : pieces) add_piece(s, piece, old.zone);
	EditResult r = commit(s, { old.corners });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult join_lots(CityState &s, int a, int b) {
	const PlayerLot *la = s.lot_book.find(a), *lb = s.lot_book.find(b);
	if (!la || !lb || a == b) return refused("hint.lot.pick");
	const std::vector<XZ> ring = joined_lots(la->corners, lb->corners);
	if (ring.empty()) return refused("hint.lot.apart");
	const PlayerLot first = *la, second = *lb;
	EditResult razed;
	raze(s, first.corners, razed);
	raze(s, second.corners, razed);
	j_remove_lot(s, a);
	j_remove_lot(s, b);
	add_piece(s, ring, first.zone);
	EditResult r = commit(s, { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult delete_lot(CityState &s, int id) {
	const PlayerLot *l = s.lot_book.find(id);
	if (!l) return refused("hint.lot.pick");
	const std::vector<XZ> ring = l->corners;
	EditResult razed;
	raze(s, ring, razed);
	j_remove_lot(s, id);
	EditResult r = commit(s, { ring });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

EditResult zone_lot(CityState &s, int id, int zone) {
	const PlayerLot *found = s.lot_book.find(id);
	if (!found) return refused("hint.lot.pick");
	if (found->zone == zone) return refused("tools.toast.zoneSame");
	PlayerLot l = *found;
	EditResult razed;
	if (l.zone != 0) raze(s, l.corners, razed); // what grew for the old zone goes
	l.zone = zone;
	j_edit_lot(s, id, l);
	EditResult r = commit(s, { l.corners });
	r.buildings_removed = razed.buildings_removed;
	return r;
}

} // namespace city
