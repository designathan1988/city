// Public transport as the script API: the commands (sim/commands/transit_commands.h) and the plan, for the tools and the panel.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/commands/transit_commands.h"

#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

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

Dictionary CityCore::transit_stop(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.stop", [&](CityState &s) { return transit_place_stop(s, p.x, p.z); });
}

Dictionary CityCore::transit_terminal(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.terminal", [&](CityState &s) { return transit_toggle_terminal(s, p.x, p.z); });
}

Dictionary CityCore::transit_station(const Vector3 &p, int mode) {
	return run_edit(*state_, "cmd.transit.station", [&](CityState &s) { return transit_place_station(s, p.x, p.z, mode_of(mode)); });
}

Dictionary CityCore::transit_track(const PackedVector3Array &points, int mode) {
	std::vector<V2> run;
	for (const Vector3 &q : points) run.push_back(V2(q.x, q.z));
	return run_edit(*state_, "cmd.transit.track", [&](CityState &s) { return transit_lay_track(s, run, mode_of(mode)); });
}

Dictionary CityCore::transit_line(const PackedInt32Array &stops) {
	std::vector<int> ids;
	for (int id : stops) ids.push_back(id);
	return run_edit(*state_, "cmd.transit.line", [&](CityState &s) { return transit_make_line(s, ids); });
}

Dictionary CityCore::transit_remove(const Vector3 &p) {
	return run_edit(*state_, "cmd.transit.remove", [&](CityState &s) { return transit_remove_at(s, p.x, p.z); });
}

Dictionary CityCore::transit_drop_line(int id) {
	return run_edit(*state_, "cmd.transit.remove", [&](CityState &s) { return transit_remove_line(s, id); });
}

Dictionary CityCore::transit_vehicles(int id, int vehicles) {
	return run_edit(*state_, "cmd.transit.vehicles", [&](CityState &s) { return transit_set_line_vehicles(s, id, vehicles); });
}

Dictionary CityCore::transit_plan() {
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

Dictionary CityCore::transit_run() {
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const TransitRun &run = state_->transit_run;
	int on_road = 0, trains = 0;
	for (const Bus &b : run.buses) on_road += b.vehicle >= 0 ? 1 : 0;
	for (const RailLine &r : run.rails) trains += int(r.trains.size());
	Dictionary d;
	d["buses"] = int(run.buses.size());
	d["on_road"] = on_road;
	d["stops_called"] = run.stops_called;
	d["trains"] = trains;
	d["trains_at_stations"] = run.trains_at_stations;
	for (const Bus &b : run.buses) {
		if (b.vehicle < 0) continue;
		const Vehicle &v = state_->traffic.vehicles[size_t(b.vehicle)];
		d["first_bus"] = Array::make(b.next, b.dwell, v.ds, v.s, v.v, v.bus_stop_at, v.conn, v.micro);
		break;
	}
	return d;
}

int CityCore::transit_pick(const Vector3 &p, int mode, bool any) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const TransitStop *q = stop_near(state_->transit, V2(p.x, p.z), state_->catalog.traffic.transit_pick_m, mode_of(mode), any);
	return q ? q->id : -1;
}

Vector3 CityCore::transit_snap(const Vector3 &p, int mode) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return on_ground(*state_, transit_snap_track(*state_, V2(p.x, p.z), mode_of(mode)));
}

} // namespace city
