// The Street tab as the script API (api/city_street.h): previews under the shared lock, commands through run_edit (sim/commands). A position is a
// ground point; results are Dictionaries with `ok`, `error` and `note` (strings.csv keys), so the tools show the core's words.
#include "api/city_street.h"

#include "render/render_world.h"
#include "sim/state/city_state.h"

#include "api/edit_result.h"
#include "sim/commands/street_commands.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

namespace {
Vector3 on_ground(const CityState &s, double x, double z) { return Vector3(float(x), s.terrain.height_at(float(x), float(z)), float(z)); }

std::vector<float> flat(const PackedVector3Array &pts) {
	std::vector<float> out;
	for (int i = 0; i < pts.size(); i++) {
		out.push_back(pts[i].x);
		out.push_back(pts[i].z);
	}
	return out;
}
} // namespace

Dictionary CityStreet::preview_item(const Vector3 &p, int kind, float reach) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready || kind < 0 || kind >= ITEM_KINDS) return d;
	const ItemSnap snap = snap_item(*state_, ItemKind(kind), p.x, p.z, reach, FurnitureRead::KEPT);
	d["ok"] = snap.ok;
	d["error"] = String(snap.reason.c_str());
	d["position"] = on_ground(*state_, snap.x, snap.z);
	return d;
}

Dictionary CityStreet::place_item(const Vector3 &p, int kind, int sign, const String &text, float reach) {
	return run_edit(*state_, "cmd.place.streetItem", [&](CityState &s) {
		return city::place_item(s, ItemKind(kind), SignType(sign), text.utf8().get_data(), p.x, p.z, reach);
	});
}

Dictionary CityStreet::place_item_row(const Vector3 &a, const Vector3 &b, int kind, float every_m) {
	return run_edit(*state_, "cmd.place.streetItem", [&](CityState &s) { return city::place_item_row(s, ItemKind(kind), a.x, a.z, b.x, b.z, every_m); });
}

Dictionary CityStreet::preview_barrier(const PackedVector3Array &pts, const Vector3 &cursor, int kind) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready) return d;
	double x = cursor.x, z = cursor.z;
	snap_barrier_point(*state_, BarrierKind(kind), x, z);
	std::vector<float> run = flat(pts);
	run.push_back(float(x));
	run.push_back(float(z));
	const std::string why = barrier_problem(*state_, BarrierKind(kind), run);
	d["position"] = on_ground(*state_, x, z);
	d["ok"] = why != "hint.barrier.road";
	d["error"] = String(why.c_str());
	return d;
}

Dictionary CityStreet::build_barrier(const PackedVector3Array &pts, int kind) {
	return run_edit(*state_, "cmd.build.wall", [&](CityState &s) { return city::build_barrier(s, BarrierKind(kind), flat(pts)); });
}

Dictionary CityStreet::remove_street(const Vector3 &p, float reach) {
	return run_edit(*state_, "cmd.demolish.item", [&](CityState &s) {
		EditResult r;
		r.ok = remove_street_at(s, p.x, p.z, reach);
		if (!r.ok) r.error = "hint.barrier.nothing";
		return r;
	});
}

Dictionary CityStreet::preview_pole_run(const Vector3 &a, const Vector3 &b, float reach, int lamps, int kind) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready) return d;
	const PoleRunPlan plan = plan_pole_run(*state_, a.x, a.z, b.x, b.z, reach, LampMode(lamps), PoleKind(kind));
	Array poles;
	for (const PlannedPole &p : plan.poles) {
		Dictionary one;
		one["position"] = on_ground(*state_, p.x, p.z);
		one["standing"] = p.existing >= 0;
		one["lamp"] = p.lamp;
		one["kind"] = int(p.kind);
		poles.push_back(one);
	}
	d["poles"] = poles;
	d["error"] = String(plan.refused.c_str());
	d["ok"] = plan.refused.empty() && !plan.poles.empty();
	d["from"] = on_ground(*state_, plan.from_x, plan.from_z);
	d["to"] = on_ground(*state_, plan.to_x, plan.to_z);
	return d;
}

Dictionary CityStreet::build_pole_run(const Vector3 &a, const Vector3 &b, float reach, int lamps, int kind) {
	PlannedPole last;
	bool strung = false;
	Vector3 at;
	Dictionary d = run_edit(*state_, "cmd.place.poleRun", [&](CityState &s) {
		EditResult r = city::build_pole_run(s, a.x, a.z, b.x, b.z, reach, LampMode(lamps), PoleKind(kind), &last);
		strung = r.ok;
		if (strung) at = on_ground(s, last.x, last.z); // under the edit's lock, as the preview reads the ground
		return r;
	});
	if (strung) d["last"] = at; // the last pole: where the tool goes on from, without planning the run again
	return d;
}

void CityStreet::show_pole_preview(const Array &poles) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::vector<PreviewPole> shown;
	for (int i = 0; i < poles.size(); i++) {
		const Dictionary p = poles[i];
		shown.push_back({ p["position"], p["lamp"], p["standing"], PoleKind(int(p["kind"])) });
	}
	render_->set_pole_preview(*state_, shown);
}

void CityStreet::set_road_walls(bool on) {
	map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::COMMAND);
	state_->new_road_flags = uint16_t(on ? state_->new_road_flags | SEG_CUT_WALLS : state_->new_road_flags & ~SEG_CUT_WALLS);
}

bool CityStreet::road_walls() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return (state_->new_road_flags & SEG_CUT_WALLS) != 0;
}

void CityStreet::set_road_height(float metres) {
	map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::COMMAND);
	state_->new_road_lift = metres;
}

float CityStreet::road_height() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->new_road_lift;
}

void CityStreet::set_road_furniture(int level) {
	map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::COMMAND);
	state_->new_road_flags = uint16_t((state_->new_road_flags & ~SEG_FURNITURE_MASK) | (uint16_t(level << SEG_FURNITURE_SHIFT) & SEG_FURNITURE_MASK));
}

int CityStreet::road_furniture() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return (state_->new_road_flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT;
}

Array CityStreet::sign_cells() { return render_->take_sign_cells(); }

// What stands along the streets, kit and player's alike, for a comparison per kilometre: the pieces of each FurnitureKind (`kinds`,
// in its order), the street length and the poles that carry a light; the lamps turned away from their street (`lamps_faced_away`) and
// the lighting columns standing on the face of their street's line (`line_face_columns`).
Dictionary CityStreet::furniture_census() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	PackedInt32Array kinds;
	kinds.resize(FURNITURE_KINDS);
	kinds.fill(0);
	double metres = 0, longest = 0, shortest = 0;
	int lit = 0, row_poles = 0, row_spans = 0, row_broken = 0, faced_away = 0, line_face_columns = 0;
	std::vector<FurnitureSpot> spots;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		metres += s.roads.segs[size_t(sid)].len;
		spots.clear();
		traffic::street_furniture_of(s, sid, FURNITURE_ALL, spots);
		int line_side = -1; // the face the segment's line stands on: its main row's own poles
		for (const FurnitureSpot &p : spots)
			if (p.kind == FurnitureKind::LAMP && p.row == 0 && p.corner < 0) line_side = p.side;
		for (const FurnitureSpot &p : spots) {
			if (p.kind != FurnitureKind::LAMP && p.kind != FurnitureKind::COLUMN) continue;
			faced_away += faces_road(p) ? 0 : 1;
			line_face_columns += p.row != 0 && line_side >= 0 && p.side == line_side ? 1 : 0;
		}
		for (const FurnitureSpot &p : spots) {
			kinds[int(p.kind)] += 1;
			lit += p.kind == FurnitureKind::LAMP && p.lit ? 1 : 0;
			if (p.kind != FurnitureKind::LAMP || p.row != 0) continue;
			row_poles++;
			if (p.strung < 0) continue;
			const FurnitureSpot *before = nullptr; // the pole its wires come from, standing on the same segment
			for (const FurnitureSpot &q : spots)
				if (q.kind == FurnitureKind::LAMP && q.row == 0 && q.station == p.strung) before = &q;
			if (!before) {
				row_broken++;
				continue;
			}
			row_spans++;
			double ax, az, ay, bx, bz, by;
			footway_point(s.roads, sid, before->v, before->u, ax, az, ay);
			footway_point(s.roads, sid, p.v, p.u, bx, bz, by);
			const double span = std::hypot(bx - ax, bz - az);
			longest = std::max(longest, span);
			shortest = row_spans == 1 ? span : std::min(shortest, span);
		}
	}
	Dictionary d;
	d["kinds"] = kinds;
	d["street_m"] = metres;
	d["lit"] = lit;
	d["row_poles"] = row_poles; // the main rows' poles (they carry the line)
	d["row_spans"] = row_spans; // the spans the layout strung between them
	d["row_broken"] = row_broken; // a pole strung to a pole that does not stand (a span the render cannot draw)
	d["row_longest_m"] = longest; // the longest of those spans, across
	d["row_shortest_m"] = shortest; // and the shortest
	d["lamps_faced_away"] = faced_away; // a lamp or column whose arm reaches over the lots, not its street (faces_road)
	d["line_face_columns"] = line_face_columns; // a lighting column on the face its street's line stands on, not across from it
	line_census(d); // the lines as one network: joints, corner poles and the spans across the junctions (under the same lock)
	return d;
}

PackedVector3Array CityStreet::street_poles() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	PackedVector3Array out;
	for (const UtilityPole &p : state_->street.poles.items)
		if (p.alive) out.push_back(on_ground(*state_, p.x, p.z));
	return out;
}

Dictionary CityStreet::street_counts() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const StreetWorks &w = state_->street;
	Dictionary d;
	d["items"] = w.items.alive;
	d["poles"] = w.poles.alive;
	int rural = 0;
	for (const UtilityPole &p : w.poles.items) rural += p.alive && p.kind == PoleKind::RURAL ? 1 : 0;
	d["rural_poles"] = rural;
	d["spans"] = w.spans.alive;
	d["barriers"] = w.barriers.alive;
	d["cuts"] = w.cuts.alive;
	d["instances"] = render_->street_instances();
	d["sign_generation"] = render_->sign_generation(); // a reset of the sign atlas, seen even when the new city has no plates
	d["sign_cells"] = render_->sign_cells();
	return d;
}

// Where each piece of segment `seg`'s furniture stands (tests and the look's queries): its kind, row, side, station, place in the
// segment frame (v along, u across) and in the world (sim/street/footway.h footway_point, without the kerb's rise).
Array CityStreet::furniture_spots(int seg) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Array out;
	if (seg < 0 || seg >= int(s.roads.segs.size()) || !s.roads.segs[size_t(seg)].alive) return out;
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, seg, FURNITURE_ALL, spots);
	for (const FurnitureSpot &p : spots) {
		double x, z, y;
		footway_point(s.roads, seg, p.v, p.u, x, z, y);
		Dictionary d;
		d["kind"] = int(p.kind);
		d["row"] = p.row;
		d["side"] = p.side;
		d["station"] = p.station;
		d["strung"] = p.strung; // a main row's pole: the station its wires come from (-1: the row starts here)
		d["row_ends"] = p.row_ends; // ROW_HEAD 1, ROW_TAIL 2
		d["v"] = p.v;
		d["u"] = p.u;
		d["position"] = Vector3(float(x), float(y), float(z));
		out.push_back(d);
	}
	return out;
}

void CityStreet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("preview_item", "position", "kind", "reach"), &CityStreet::preview_item);
	ClassDB::bind_method(D_METHOD("place_item", "position", "kind", "sign", "text", "reach"), &CityStreet::place_item);
	ClassDB::bind_method(D_METHOD("place_item_row", "from", "to", "kind", "every_m"), &CityStreet::place_item_row);
	ClassDB::bind_method(D_METHOD("preview_barrier", "points", "cursor", "kind"), &CityStreet::preview_barrier);
	ClassDB::bind_method(D_METHOD("build_barrier", "points", "kind"), &CityStreet::build_barrier);
	ClassDB::bind_method(D_METHOD("remove_street", "position", "reach"), &CityStreet::remove_street);
	ClassDB::bind_method(D_METHOD("preview_pole_run", "from", "to", "reach", "lamps", "kind"), &CityStreet::preview_pole_run);
	ClassDB::bind_method(D_METHOD("build_pole_run", "from", "to", "reach", "lamps", "kind"), &CityStreet::build_pole_run);
	ClassDB::bind_method(D_METHOD("show_pole_preview", "poles"), &CityStreet::show_pole_preview);
	ClassDB::bind_method(D_METHOD("set_road_furniture", "level"), &CityStreet::set_road_furniture);
	ClassDB::bind_method(D_METHOD("road_furniture"), &CityStreet::road_furniture);
	ClassDB::bind_method(D_METHOD("set_road_height", "metres"), &CityStreet::set_road_height);
	ClassDB::bind_method(D_METHOD("road_height"), &CityStreet::road_height);
	ClassDB::bind_method(D_METHOD("set_road_walls", "on"), &CityStreet::set_road_walls);
	ClassDB::bind_method(D_METHOD("road_walls"), &CityStreet::road_walls);
	ClassDB::bind_method(D_METHOD("sign_cells"), &CityStreet::sign_cells);
	ClassDB::bind_method(D_METHOD("street_counts"), &CityStreet::street_counts);
	ClassDB::bind_method(D_METHOD("street_poles"), &CityStreet::street_poles);
	ClassDB::bind_method(D_METHOD("furniture_census"), &CityStreet::furniture_census);
	ClassDB::bind_method(D_METHOD("furniture_spots", "seg"), &CityStreet::furniture_spots);
}

} // namespace city
