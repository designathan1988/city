// The Interior tab as script commands: the cores through a building and each floor's furniture (sim/commands/body_furnish.cpp).
#include "api/body_api.h"
#include "api/city_builder.h"
#include "sim/buildings/interior/interior.h"
#include "sim/buildings/interior/tables.h"

using namespace godot;

namespace city {

// What a floor of the record is furnished from.
static FloorEdit floor_of(const CityState &s, const b40::Body &b, int id, int level) {
	const Building &rec = s.buildings.items[id];
	return { &s.buildings.designs, b40::purpose_of(b, rec.fn, rec.seed), level };
}

Dictionary CityBuilder::body_add_core(int id, const Vector3 &at, int kind) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 p = lot_local(f, at);
		return op_add_core(b, t, p.x, p.y, kind);
	});
}

Dictionary CityBuilder::body_remove_core(int id, const Vector3 &at) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &, const b40::LotFrame &f) {
		const V2 p = lot_local(f, at);
		return op_remove_core(b, p.x, p.y);
	});
}

Dictionary CityBuilder::body_put_piece(int id, int level, const String &kind, const Vector3 &at, double angle) {
	int found = -1;
	for (int k = 0; k < b40::FK_COUNT; k++)
		if (kind == b40::FURNITURE_NAMES[k]) found = k;
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 p = lot_local(f, at);
		return op_put_piece(b, t, floor_of(*state_, b, id, level), found, p.x, p.y, float(angle));
	});
}

Dictionary CityBuilder::body_move_piece(int id, int level, int index, const Vector3 &at) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &t, const b40::LotFrame &f) {
		const V2 p = lot_local(f, at);
		return op_move_piece(b, t, floor_of(*state_, b, id, level), index, p.x, p.y);
	});
}

Dictionary CityBuilder::body_turn_piece(int id, int level, int index) {
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &) { return op_turn_piece(b, floor_of(*state_, b, id, level), index); });
}

Dictionary CityBuilder::body_remove_piece(int id, int level, const Vector3 &at) {
	return edit_of_body_in_lot(*state_, id, [&](b40::Body &b, const Tuning &, const b40::LotFrame &f) {
		const V2 p = lot_local(f, at);
		return op_remove_piece(b, floor_of(*state_, b, id, level), p.x, p.y);
	});
}

int CityBuilder::body_piece_at(int id, int level, const Vector3 &at) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return -1;
	b40::Body body;
	b40::record_body(s.buildings.designs, s.buildings.items[id], body);
	const V2 p = lot_point(s, id, at);
	return piece_at(body, floor_of(s, body, id, level), p.x, p.y);
}

Vector3 CityBuilder::body_piece_size(const String &kind) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	for (int k = 0; k < b40::FK_COUNT; k++)
		if (kind == b40::FURNITURE_NAMES[k]) {
			const float *size = state_->buildings.designs.interior.size[k];
			return Vector3(size[0], size[1], size[2]);
		}
	return Vector3();
}

double CityBuilder::body_floor_y(int id, int level) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return 0;
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	const b40::LotFrame f = b40::lot_frame(rec);
	return b40::lot_floor(body, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); }) + body.level_z(level);
}

} // namespace city
