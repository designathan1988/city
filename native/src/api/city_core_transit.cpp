// Public transport as the script API: the commands (sim/commands/transit_commands.h) and the plan, for the tools and the panel.
#include "api/city_transit.h"

#include "api/edit_result.h"
#include "sim/commands/transit_commands.h"
#include "sim/transit/transit_ground.h"
#include "sim/state/city_state.h"
#include "render/render_world.h"

#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace city {

namespace {
TransitMode mode_of(int m) { return TransitMode(m); }

Vector3 on_ground(const CityState &s, const V2 &p) { return Vector3(p.x, s.terrain.height_at(p.x, p.y), p.y); }

Dictionary stop_dict(const CityState &s, const TransitStop &q) {
	Dictionary d;
	d["id"] = q.id;
	d["mode"] = int(q.mode);
	d["at"] = on_ground(s, q.at);
	d["terminal"] = q.terminal;
	V2 along, out;
	transit_stop_frame(s, q, along, out);
	d["along"] = Vector3(along.x, 0, along.y);
	d["out"] = Vector3(out.x, 0, out.y);
	return d;
}

Dictionary line_dict(const TransitLine &l) {
	Dictionary d;
	PackedInt32Array stops;
	for (int id : l.stops) stops.push_back(id);
	d["id"] = l.id;
	d["mode"] = int(l.mode);
	d["stops"] = stops;
	d["colour"] = int64_t(l.colour);
	d["vehicles"] = l.vehicles;
	d["name"] = String(l.name.c_str());
	return d;
}
} // namespace

Dictionary CityTransit::transit_stop(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.stop", [&](CityState &s) { return transit_place_stop(s, p.x, p.z); });
}

Dictionary CityTransit::transit_terminal(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.terminal", [&](CityState &s) { return transit_toggle_terminal(s, p.x, p.z); });
}

Dictionary CityTransit::transit_station(const Vector3 &p, int mode) {
	return run_edit(*state_, "cmd.transit.station", [&](CityState &s) { return transit_place_station(s, p.x, p.z, mode_of(mode)); });
}

Dictionary CityTransit::transit_track(const PackedVector3Array &points, int mode) {
	std::vector<V2> run;
	for (const Vector3 &q : points) run.push_back(V2(q.x, q.z));
	return run_edit(*state_, "cmd.transit.track", [&](CityState &s) { return transit_lay_track(s, run, mode_of(mode)); });
}

Dictionary CityTransit::transit_line(const PackedInt32Array &stops) {
	std::vector<int> ids;
	for (int id : stops) ids.push_back(id);
	return run_edit(*state_, "cmd.transit.line", [&](CityState &s) { return transit_make_line(s, ids); });
}

Dictionary CityTransit::transit_remove(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.remove", [&](CityState &s) { return transit_remove_at(s, p.x, p.z); });
}

Dictionary CityTransit::transit_drop_line(int id) {
	return run_edit(*state_, "cmd.transit.remove", [&](CityState &s) { return transit_remove_line(s, id); });
}

Dictionary CityTransit::transit_vehicles(int id, int vehicles) {
	return run_edit(*state_, "cmd.transit.vehicles", [&](CityState &s) { return transit_set_line_vehicles(s, id, vehicles); });
}

Dictionary CityTransit::transit_plan() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Array stops, lines;
	for (const TransitStop &q : s.transit.stops) stops.push_back(stop_dict(s, q));
	for (const TransitLine &l : s.transit.lines) lines.push_back(line_dict(l));
	Dictionary d;
	d["stops"] = stops;
	d["lines"] = lines;
	d["tracks"] = int(s.transit.tracks.size());
	return d;
}

Dictionary CityTransit::transit_run() {
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const TransitRun &run = state_->transit_run;
	int on_road = 0, trains = 0;
	for (const Bus &b : run.buses) on_road += b.vehicle >= 0 ? 1 : 0;
	for (const RailLine &r : run.rails) trains += int(r.trains.size());
	Dictionary d;
	d["buses"] = int(run.buses.size());
	d["on_road"] = on_road;
	d["stops_called"] = run.stops_called;
	d["boarded"] = state_->traffic.bus_boarded;
	d["alighted"] = state_->traffic.bus_alighted;
	d["trains"] = trains;
	d["trains_at_stations"] = run.trains_at_stations;
	int crossings = 0, held = 0;
	for (const RailLine &r : run.rails) crossings += int(r.crossings.size());
	for (const Vehicle &v : state_->traffic.vehicles) {
		if (!v.alive || !v.micro || v.conn >= 0) continue;
		const float gap = transit::crossing_gap(*state_, v.ds * MAX_LANES + v.lane, v.s);
		held += gap >= 0 && gap < state_->catalog.traffic.look_ahead_m ? 1 : 0;
	}
	d["crossings"] = crossings; // lanes crossed at grade
	d["held_at_crossing"] = held; // vehicles with a closed crossing ahead of them
	for (const Bus &b : run.buses) {
		if (b.vehicle < 0) continue;
		const Vehicle &v = state_->traffic.vehicles[size_t(b.vehicle)];
		d["first_bus"] = Array::make(b.next, b.dwell, v.ds, v.s, v.v, v.bus_stop_at, v.conn, v.micro);
		d["bus_at"] = Vector3(v.cur.x, v.cur.y, v.cur.z);
		d["bus_heading"] = Vector3(v.cur.fx, 0, v.cur.fz);
		break;
	}
	PackedVector3Array cars; // every car of every train on the surface, each line's trains running apart
	for (const RailLine &r : run.rails) {
		if (r.metro) continue;
		for (const Train &t : r.trains)
			for (const PoseTrail &car : t.cars) cars.push_back(Vector3(car.at[POSE_TRAIL - 1].x, car.at[POSE_TRAIL - 1].y, car.at[POSE_TRAIL - 1].z));
	}
	d["train_cars"] = cars;
	for (const RailLine &r : run.rails) {
		if (r.metro || r.trains.empty() || r.trains[0].cars.empty()) continue;
		const Train &t = r.trains[0];
		const Pose &head = t.cars[0].at[POSE_TRAIL - 1];
		d["train_at"] = Vector3(head.x, head.y, head.z);
		d["train_heading"] = Vector3(head.fx, 0, head.fz);
		d["train_speed"] = t.v;
		break;
	}
	return d;
}

int CityTransit::transit_pick(const Vector3 &p, int mode, bool any) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const TransitStop *q = stop_near(state_->transit, V2(p.x, p.z), state_->catalog.traffic.transit_pick_m, mode_of(mode), any);
	return q ? q->id : -1;
}

Dictionary CityTransit::transit_footway(const Vector3 &p, float reach) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	TransitStop q;
	if (!transit_find_footway(s, V2(p.x, p.z), reach, q.at, q.seg)) return d;
	V2 along, out;
	transit_stop_frame(s, q, along, out);
	d["at"] = on_ground(s, q.at);
	d["along"] = Vector3(along.x, 0, along.y);
	d["out"] = Vector3(out.x, 0, out.y);
	return d;
}

void CityTransit::transit_xray(bool on) { render_->transit_xray(on); }

void CityTransit::transit_preview(const PackedVector3Array &points, int mode) {
	std::vector<V2> run;
	for (const Vector3 &q : points) run.push_back(V2(q.x, q.z));
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	render_->transit_preview(*state_, run, mode_of(mode));
}

Vector3 CityTransit::transit_snap(const Vector3 &p, int mode) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return on_ground(*state_, transit_snap_track(*state_, V2(p.x, p.z), mode_of(mode)));
}

void CityTransit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("transit_stop", "position"), &CityTransit::transit_stop);
	ClassDB::bind_method(D_METHOD("transit_terminal", "position"), &CityTransit::transit_terminal);
	ClassDB::bind_method(D_METHOD("transit_station", "position", "mode"), &CityTransit::transit_station);
	ClassDB::bind_method(D_METHOD("transit_track", "points", "mode"), &CityTransit::transit_track);
	ClassDB::bind_method(D_METHOD("transit_line", "stops"), &CityTransit::transit_line);
	ClassDB::bind_method(D_METHOD("transit_remove", "position"), &CityTransit::transit_remove);
	ClassDB::bind_method(D_METHOD("transit_drop_line", "id"), &CityTransit::transit_drop_line);
	ClassDB::bind_method(D_METHOD("transit_vehicles", "id", "vehicles"), &CityTransit::transit_vehicles);
	ClassDB::bind_method(D_METHOD("transit_plan"), &CityTransit::transit_plan);
	ClassDB::bind_method(D_METHOD("transit_run"), &CityTransit::transit_run);
	ClassDB::bind_method(D_METHOD("transit_footway", "position", "reach"), &CityTransit::transit_footway);
	ClassDB::bind_method(D_METHOD("transit_xray", "on"), &CityTransit::transit_xray);
	ClassDB::bind_method(D_METHOD("transit_preview", "points", "mode"), &CityTransit::transit_preview);
	ClassDB::bind_method(D_METHOD("transit_pick", "position", "mode", "any"), &CityTransit::transit_pick);
	ClassDB::bind_method(D_METHOD("transit_snap", "position", "mode"), &CityTransit::transit_snap);
}

} // namespace city
