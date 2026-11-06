// Zones > Lots as API of script: the lots the player draws, as edits and as outlines on the ground.
#include "api/city_core.h"
#include "api/edit_result.h"
#include "sim/lots/lot_book.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr double LOT_ANGLE_LOOK_M = 80;

LotPoint point(const Vector3 &v) { return { v.x, v.z }; }

PackedVector3Array on_ground(const CityState &s, const std::vector<LotPoint> &ring) {
	PackedVector3Array out;
	for (const LotPoint &p : ring) out.push_back(Vector3(float(p.x), s.terrain.height_at(float(p.x), float(p.z)), float(p.z)));
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

Dictionary CityCore::lot_add_polygon(const PackedVector3Array &points, int front) {
	std::vector<LotPoint> ring;
	for (const Vector3 &p : points) ring.push_back(point(p));
	return run_edit(*state_, "cmd.place.lot", [&](CityState &s) { return add_lot_polygon(s, ring, front); });
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

// The way the nearest street runs at a point: a lot drawn there is square to it.
double CityCore::lot_angle_at(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	double best = LOT_ANGLE_LOOK_M * LOT_ANGLE_LOOK_M, angle = 0;
	s.roads.for_segments_near(p.x - LOT_ANGLE_LOOK_M, p.z - LOT_ANGLE_LOOK_M, p.x + LOT_ANGLE_LOOK_M, p.z + LOT_ANGLE_LOOK_M, [&](int seg) {
		const Quad &q = s.roads.segs[size_t(seg)].q;
		const double dx = q.x1 - q.x0, dz = q.z1 - q.z0, len2 = std::max(dx * dx + dz * dz, 1e-6);
		const double t = std::clamp(((p.x - q.x0) * dx + (p.z - q.z0) * dz) / len2, 0.0, 1.0);
		const double ex = q.x0 + dx * t - p.x, ez = q.z0 + dz * t - p.z, d = ex * ex + ez * ez;
		if (d < best) {
			best = d;
			angle = std::atan2(dz, dx);
		}
	});
	return angle;
}

int CityCore::lot_at(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->lot_book.at(p.x, p.z);
}

// Every lot as {id, zone, points}: the book is the player's own, a few hundred at most.
Array CityCore::lots() {
	Array out;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	for (const PlayerLot &l : state_->lot_book.lots) {
		Dictionary d;
		d["id"] = l.id;
		d["zone"] = l.zone;
		d["points"] = on_ground(*state_, l.corners);
		out.push_back(d);
	}
	return out;
}

// The lines a cut would run along, in pairs of points.
PackedVector3Array CityCore::lot_cut_lines(int id, int kind, int parts, const Vector3 &a, const Vector3 &b) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const PlayerLot *l = state_->lot_book.find(id);
	return l ? on_ground(*state_, cut_lines(l->corners, cut_of(kind, parts, a, b))) : PackedVector3Array();
}

} // namespace city
