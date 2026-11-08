// Zones > Lots as API of script: the lots the player draws, as edits and as outlines on the ground.
#include "api/city_core.h"
#include "sim/commands/lot_commands.h"
#include "api/edit_result.h"
#include "sim/lots/lot_book.h"
#include "sim/citygen/lot_land.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

static constexpr double SHORTEST_SIDE2_M2 = 1e-6; // a side this short has no direction
static constexpr double SIDE_INSIDE_M = 1e-3; // of two lots sharing a side, the one the point is in counts as nearer

namespace {

XZ point(const Vector3 &v) { return { v.x, v.z }; }

PackedVector3Array on_ground(const CityState &s, const std::vector<XZ> &ring) {
	PackedVector3Array out;
	for (const XZ &p : ring) out.push_back(Vector3(float(p.x), s.terrain.height_at(float(p.x), float(p.z)), float(p.z)));
	return out;
}

LotCut cut_of(int kind, int parts, const Vector3 &a, const Vector3 &b) {
	LotCut c;
	c.kind = LotCutKind(kind);
	c.parts = parts;
	c.a = point(a);
	c.b = point(b);
	return c;
}
} // namespace

Dictionary CityCore::lot_add_rect(const Vector3 &a, const Vector3 &b, double angle) {
	return run_edit(*state_, "cmd.place.lot", [&](CityState &s) { return add_lot_rect(s, point(a), point(b), angle); });
}

Dictionary CityCore::lot_add_polygon(const PackedVector3Array &points) {
	std::vector<XZ> ring;
	for (const Vector3 &p : points) ring.push_back(point(p));
	return run_edit(*state_, "cmd.place.lot", [&](CityState &s) { return add_lot_polygon(s, ring); });
}

Dictionary CityCore::lot_move_corner(const Vector3 &from, const Vector3 &to) {
	return run_edit(*state_, "cmd.lot.edit", [&](CityState &s) { return move_lot_corner(s, point(from), point(to)); });
}

Dictionary CityCore::lot_curve_side(const Vector3 &a, const Vector3 &b, const Vector3 &through) {
	return run_edit(*state_, "cmd.lot.edit", [&](CityState &s) { return curve_lot_side(s, point(a), point(b), point(through)); });
}

Dictionary CityCore::lot_set_front(int id, int side) {
	return run_edit(*state_, "cmd.lot.edit", [&](CityState &s) { return set_lot_front(s, id, side); });
}

Dictionary CityCore::lot_split(int id, int kind, int parts, const Vector3 &a, const Vector3 &b) {
	return run_edit(*state_, "cmd.lot.split", [&](CityState &s) { return split_lot(s, id, cut_of(kind, parts, a, b)); });
}

Dictionary CityCore::lot_join(int a, int b) {
	return run_edit(*state_, "cmd.lot.join", [&](CityState &s) { return join_lots(s, a, b); });
}

Dictionary CityCore::lot_delete(int id) {
	return run_edit(*state_, "cmd.lot.delete", [&](CityState &s) { return delete_lot(s, id); });
}

Dictionary CityCore::lot_zone(int id, int zone) {
	return run_edit(*state_, "cmd.place.zone", [&](CityState &s) { return zone_lot(s, id, zone); });
}

Dictionary CityCore::lot_take(const Vector3 &p) {
	return run_edit(*state_, "cmd.place.lot", [&](CityState &s) { return take_lot_at(s, point(p)); });
}

Dictionary CityCore::lot_erase_path(const PackedVector3Array &points) {
	std::vector<XZ> dabs;
	for (const Vector3 &p : points) dabs.push_back({ p.x, p.z });
	return run_edit(*state_, "cmd.lot.delete", [&](CityState &s) { return erase_lots_path(s, dabs); });
}

// The way the nearest street runs at a point: a lot drawn there is square to it.
double CityCore::lot_angle_at(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	double best = s.catalog.tune.lot_angle_look_m * s.catalog.tune.lot_angle_look_m, angle = 0;
	s.roads.for_segments_near(p.x - s.catalog.tune.lot_angle_look_m, p.z - s.catalog.tune.lot_angle_look_m, p.x + s.catalog.tune.lot_angle_look_m, p.z + s.catalog.tune.lot_angle_look_m, [&](int seg) {
		const Quad &q = s.roads.segs[size_t(seg)].q;
		const double dx = q.x1 - q.x0, dz = q.z1 - q.z0, len2 = std::max(dx * dx + dz * dz, SHORTEST_SIDE2_M2);
		const double t = std::clamp(((p.x - q.x0) * dx + (p.z - q.z0) * dz) / len2, 0.0, 1.0);
		const double ex = q.x0 + dx * t - p.x, ez = q.z0 + dz * t - p.z, d = ex * ex + ez * ez;
		if (d < best) {
			best = d;
			angle = std::atan2(dz, dx);
		}
	});
	return angle;
}

// The rectangle being dragged as it will be made: cut back to the footways, on the ground.
PackedVector3Array CityCore::lot_land(const Vector3 &a, const Vector3 &b, double angle) {
	PackedVector3Array out;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const std::vector<XZ> rect = lot_rect(state_->catalog.growth.shapes, { a.x, a.z }, { b.x, b.z }, angle);
	if (rect.empty()) return out;
	for (const XZ &p : lot_on_land(state_->catalog.growth.shapes, state_->roads, rect)) out.push_back(Vector3(float(p.x), state_->terrain.height_at(float(p.x), float(p.z)), float(p.z)));
	return out;
}

// A lot's point by the streets and the other lots (lotSnapper): `apart`, when finite, the corner being dragged.
Vector3 CityCore::lot_snap_point(const Vector3 &p, double reach, const Vector3 &apart) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const XZ from = { apart.x, apart.z };
	const LotSnap hit = lot_snap(state_->catalog.growth.shapes, state_->lot_book, state_->roads, { p.x, p.z }, reach, 0, apart.is_finite() ? &from : nullptr);
	return Vector3(float(hit.p.x), state_->terrain.height_at(float(hit.p.x), float(hit.p.z)), float(hit.p.z));
}

// The lots the streets propose in the cell of a point, cut for `zone` (works are cut deeper and wider), each a ring on the
// ground, past what the player already holds there.
Array CityCore::lot_proposals(const Vector3 &p, int zone) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Array out;
	for (const Lot &l : *LotCache::proposed(s, chunk_of(p.x, p.z), cut_for_works(s.catalog, zone))) {
		if (s.lot_book.at(l.x, l.z) != 0) continue;
		out.push_back(on_ground(s, city::lot_ring(l)));
	}
	return out;
}

// The lot under p as the zone brush takes it (paint_zone_path): the player's lot there, else the land lot (a zoned lot of the
// streets', or with `zone` the lot proposed there cut for it; zone 0 lays no lot): {points, cover, zone, held}, empty on none.
// `cover` is its ground in triangles laid on the relief, cut on the relief's grid as the zone layer cuts a lot (lot_cover), for the
// brush to light it.
Dictionary CityCore::lot_under(const Vector3 &p, int zone) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	std::vector<XZ> ring;
	if (const PlayerLot *l = s.lot_book.find(s.lot_book.at(p.x, p.z))) {
		ring = l->corners;
		d["zone"] = l->zone;
		d["held"] = true;
	} else {
		LandLot land;
		if (!land_lot_at(s, { p.x, p.z }, zone == 0 ? -1 : int(cut_for_works(s.catalog, zone)), land)) return d;
		ring = std::move(land.ring);
		d["zone"] = land.zone;
		d["held"] = false;
	}
	d["points"] = on_ground(s, ring);
	const double corner = s.terrain.x_of(0);
	d["cover"] = on_ground(s, lot_cover(ring, s.terrain.spacing, { corner, corner }));
	return d;
}

int CityCore::lot_at(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->lot_book.at(p.x, p.z);
}

// Every lot as {id, zone, points}, the whole book (the tests read it; the tools ask near a point: lot_at, lot_corner_near, lot_side_near).
Array CityCore::lots() {
	Array out;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	for (const PlayerLot &l : state_->lot_book.all()) {
		Dictionary d;
		d["id"] = l.id;
		d["zone"] = l.zone;
		d["points"] = on_ground(*state_, l.corners);
		out.push_back(d);
	}
	return out;
}

// The corner of any of the player's lots nearest p within `reach` metres, or INF.
Vector3 CityCore::lot_corner_near(const Vector3 &p, double reach) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Vector3 best(INFINITY, INFINITY, INFINITY);
	for (int i : s.lot_book.near(p.x - reach, p.z - reach, p.x + reach, p.z + reach))
		for (const XZ &q : s.lot_book.all()[size_t(i)].corners) {
			const double d = std::hypot(q.x - p.x, q.z - p.z);
			if (d >= reach) continue;
			reach = d;
			best = Vector3(float(q.x), s.terrain.height_at(float(q.x), float(q.z)), float(q.z));
		}
	return best;
}

// The side of the player's lots nearest p within `reach` metres (of two lots sharing it, the one p is in): {id, side, a, b}, empty
// on none.
Dictionary CityCore::lot_side_near(const Vector3 &p, double reach) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const XZ at = point(p);
	Dictionary d;
	double best = reach;
	for (int i : s.lot_book.near(p.x - reach, p.z - reach, p.x + reach, p.z + reach)) {
		const PlayerLot &l = s.lot_book.all()[size_t(i)];
		const bool in = inside_lot(at, l.corners);
		for (size_t k = 0; k < l.corners.size(); k++) {
			const XZ &a = l.corners[k], &b = l.corners[(k + 1) % l.corners.size()];
			const double dx = b.x - a.x, dz = b.z - a.z, len2 = std::max(dx * dx + dz * dz, SHORTEST_SIDE2_M2);
			const double t = std::clamp(((at.x - a.x) * dx + (at.z - a.z) * dz) / len2, 0.0, 1.0);
			const double e = std::hypot(a.x + dx * t - at.x, a.z + dz * t - at.z) - (in ? SIDE_INSIDE_M : 0);
			if (e >= best) continue;
			best = e;
			d["id"] = l.id;
			d["side"] = int(k);
			d["a"] = Vector3(float(a.x), s.terrain.height_at(float(a.x), float(a.z)), float(a.z));
			d["b"] = Vector3(float(b.x), s.terrain.height_at(float(b.x), float(b.z)), float(b.z));
		}
	}
	return d;
}

// One of the player's lots as a ring on the ground, empty on none.
PackedVector3Array CityCore::lot_ring(int id) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const PlayerLot *l = state_->lot_book.find(id);
	return l ? on_ground(*state_, l->corners) : PackedVector3Array();
}

// The lines a cut would run along, in pairs of points.
PackedVector3Array CityCore::lot_cut_lines(int id, int kind, int parts, const Vector3 &a, const Vector3 &b) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const PlayerLot *l = state_->lot_book.find(id);
	return l ? on_ground(*state_, cut_lines(state_->catalog.growth.shapes, l->corners, cut_of(kind, parts, a, b))) : PackedVector3Array();
}

} // namespace city
