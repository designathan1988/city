// park_sample: where the capture looks at the parked side of the traffic, clear of tall buildings so a camera high over it
// stands in nothing (project/tests/visual/capture_traffic.gd); camera_clear: whether a capture may stand at a point.
#include "api/city_core.h"

#include "sim/buildings/b40/record.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float CLEAR_M = 25.f; // no tall building this near a target
constexpr int TALL_FLOORS = 4;
constexpr float TREE_SPREAD_M = 3.f; // a street tree seen from a capture's eye: the crown of the medium trees the town plants (6 to 9 m tall)

bool clear_of_towers(const CityState &s, float x, float z) {
	bool clear = true;
	s.buildings.query(x - CLEAR_M, z - CLEAR_M, x + CLEAR_M, z + CLEAR_M, [&](int id) { clear = clear && s.buildings.items[size_t(id)].floors <= TALL_FLOORS; });
	return clear;
}

// No lamp, signal post, bin, bench or street tree within `radius` of the point.
bool clear_of_furniture(const CityState &s, const Vector3 &at, float radius) {
	const float reach = radius + TREE_SPREAD_M;
	bool clear = true;
	std::vector<FurnitureSpot> spots;
	s.roads.for_segments_near(at.x - reach, at.z - reach, at.x + reach, at.z + reach, [&](int seg) {
		spots.clear();
		traffic::street_furniture_of(s, seg, FURNITURE_ALL, spots);
		for (const FurnitureSpot &f : spots) {
			Pose p;
			traffic::seg_point(s, seg, float(f.v), f.u, p);
			const float room = radius + (f.kind == FurnitureKind::TREE ? TREE_SPREAD_M : 0.f);
			clear = clear && (p.x - at.x) * (p.x - at.x) + (p.z - at.z) * (p.z - at.z) >= room * room;
		}
	});
	return clear;
}

void bays_to(const CityState &s, Dictionary &d) {
	for (const Bay &b : s.traffic.park.bays) {
		if (!b.alive || !clear_of_towers(s, b.at.x, b.at.y)) continue;
		if (!b.dock && !b.kerb) continue; // a lot's: lot_to
		const char *key = b.dock ? "dock" : "kerb";
		if ((b.has_row || b.dock) && !d.has(key)) d[key] = Vector3(b.at.x, b.y, b.at.y);
	}
}

// The lot with the most cars standing in the open: every house has a bay, most of them under a roof the camera cannot see past.
void lot_to(const CityState &s, Dictionary &d) {
	const ParkingState &p = s.traffic.park;
	int best = 0, best_id = -1, at = -1;
	for (const auto &[id, src] : p.lots) {
		int rows = 0, first = -1;
		for (int b : src.bays) {
			if (!p.bays[size_t(b)].has_row) continue;
			rows++;
			first = first < 0 ? b : first;
		}
		if (first < 0 || rows < best || (rows == best && id > best_id)) continue;
		const Bay &bay = p.bays[size_t(first)];
		if (!clear_of_towers(s, bay.at.x, bay.at.y)) continue;
		best = rows;
		best_id = id;
		at = first;
	}
	if (at < 0) return;
	const Bay &bay = p.bays[size_t(at)];
	d["lot"] = Vector3(bay.at.x, bay.y, bay.at.y);
	d["lot_rows"] = best;
}

void vehicles_to(const CityState &s, Dictionary &d) {
	const TrafficState &t = s.traffic;
	for (const Vehicle &v : t.vehicles) {
		if (!v.alive || !clear_of_towers(s, v.cur.x, v.cur.z)) continue;
		const Vector3 at(v.cur.x, v.cur.y, v.cur.z);
		if (v.off && !d.has("moving")) d["moving"] = at;
		if (v.type != VehicleType::BICYCLE || !v.micro || v.ds < 0) continue;
		const bool band = t.g.ds[size_t(v.ds)].cycle_shift != 0;
		if (band && !bool(d.get("bike_in_band", false))) {
			d["bike_band"] = at;
			d["bike_band_dir"] = Vector3(v.cur.fx, 0, v.cur.fz);
			d["bike_in_band"] = true;
		} else if (!band && !d.has("bike")) d["bike"] = at;
	}
}

// The middle of the first stretch of cycle lane (the road stands open over it, whatever is built beside), and how many there are.
void band_to(const CityState &s, Dictionary &d) {
	const LaneGraph &g = s.traffic.g;
	int bands = 0;
	for (int ds = 0; ds < int(g.ds.size()); ds++) {
		const DirSeg &lane = g.ds[size_t(ds)];
		if (!lane.ok || lane.cycle_shift == 0) continue;
		bands++;
		Pose p;
		traffic::lane_pose(s, ds, 0, lane.len * 0.5f, p);
		if (!d.has("band")) d["band"] = Vector3(p.x, p.y, p.z);
	}
	d["band_lanes"] = bands;
}
} // namespace

Dictionary CityCore::park_sample() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary d;
	bays_to(s, d);
	lot_to(s, d);
	vehicles_to(s, d);
	band_to(s, d);
	const ParkingState &pk = s.traffic.park;
	d["lorries"] = int(pk.lorries.size());
	d["lorries_sent"] = pk.lorries_sent;
	d["lorries_docked"] = pk.lorries_docked;
	d["lorries_left"] = pk.lorries_left;
	Array calls;
	for (int n : pk.lorry_calls) calls.push_back(n);
	d["lorry_calls"] = calls; // no dock, planning, no way, no truck, no route, sent
	int trucks = 0, ways = 0, planned = 0, gates = 0;
	double slowest = 0;
	Array reached;
	reached.resize(traffic::YARD_REACHES);
	reached.fill(0);
	for (int id : s.traffic.micro_list) trucks += s.traffic.vehicles[size_t(id)].type == VehicleType::TRUCK ? 1 : 0;
	for (const auto &y : pk.yards) {
		if (!y.second.job->done.load()) continue;
		gates += int(y.second.job->plan.gates.size());
		slowest = std::max(slowest, y.second.job->plan.planned_ms);
		for (const traffic::DockWays &w : y.second.job->plan.docks) {
			planned++;
			ways += w.gate >= 0 ? 1 : 0;
			reached[w.reached] = int(reached[w.reached]) + 1;
		}
	}
	d["trucks_near"] = trucks;
	d["docks_planned"] = planned;
	d["docks_with_way"] = ways;
	d["docks_reached"] = reached; // traffic::YardReach
	d["yards"] = int(pk.yards.size());
	d["yard_gates"] = gates;
	d["yard_slowest_ms"] = slowest;
	for (const Lorry &l : pk.lorries)
		if (l.phase != LorryPhase::COMING) d["lorry"] = Vector3(s.traffic.vehicles[size_t(l.id)].cur.x, s.traffic.vehicles[size_t(l.id)].cur.y, s.traffic.vehicles[size_t(l.id)].cur.z);
	return d;
}

int CityCore::dock_count() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	int docks = 0;
	b40::Body body;
	for (const Building &rec : s.buildings.items) {
		if (!rec.alive) continue;
		b40::record_body(s.buildings.designs, rec, body);
		for (const b40::LotElement &e : body.elements) docks += e.kind == b40::EL_DOCK ? 1 : 0;
	}
	return docks;
}

Variant CityCore::kerb_busy() {
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const TrafficState &t = state_->traffic;
	for (const KerbStop &k : t.kerb_stops)
		if (k.vehicle >= 0 && k.phase == KerbPhase::TRANSFER && k.ped >= 0) {
			const Pose &q = t.peds[size_t(k.ped)].cur;
			return Vector3(q.x, q.y, q.z);
		}
	return Variant();
}

bool CityCore::camera_clear(const Vector3 &at, float radius, bool vehicles) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	bool clear = true;
	s.buildings.query(at.x - radius, at.z - radius, at.x + radius, at.z + radius, [&](int) { clear = false; });
	clear = clear && clear_of_furniture(s, at, radius);
	for (const Vehicle &v : s.traffic.vehicles) {
		if (!clear || !vehicles) break;
		if (!v.alive || !v.micro) continue;
		const float reach = s.catalog.traffic.classes[int(v.type)].length * 0.5f + radius;
		const float dx = v.cur.x - at.x, dz = v.cur.z - at.z;
		clear = dx * dx + dz * dz >= reach * reach;
	}
	const float room = s.catalog.traffic.classes[int(VehicleType::PICKUP)].length * 0.5f + radius; // the longest car standing in a bay
	for (int b : s.traffic.park.rows) {
		if (!clear) break;
		const V2 &q = s.traffic.park.bays[size_t(b)].at;
		clear = (q.x - at.x) * (q.x - at.x) + (q.y - at.z) * (q.y - at.z) >= room * room;
	}
	return clear;
}

} // namespace city
