// park_sample: where the capture looks at the parked side of the traffic, clear of tall buildings so a camera high over it
// stands in nothing (project/tests/visual/capture_traffic.gd); camera_clear: whether a capture may stand at a point.
#include "api/state_locks.h"
#include "api/city_probes.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/growth.h"
#include "sim/traffic/traffic_system.h"
#include "sim/traffic/yard.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <limits>
#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float CLEAR_M = 25.f; // no tall building this near a target
constexpr int TALL_FLOORS = 4;
constexpr float ROOF_M = 3.f; // a pitched roof over a volume's last storey
constexpr float FURNITURE_TOP_M = 10.f; // nothing on a footway reaches a camera higher: a tree of 6 to 9 m, a lamp
constexpr float TREE_SPREAD_M = 3.f; // a street tree seen from a capture's eye: the crown of the medium trees the town plants (6 to 9 m tall)

bool clear_of_towers(const CityState &s, float x, float z) {
	bool clear = true;
	s.buildings.query(x - CLEAR_M, z - CLEAR_M, x + CLEAR_M, z + CLEAR_M, [&](int id) { clear = clear && s.buildings.items[size_t(id)].floors <= TALL_FLOORS; });
	return clear;
}

// Nothing of the lot reaches `up` metres over the ground within `radius` of the point: a volume with its roof, an element, a tree. A
// lawn under the camera is no wall.
bool clear_of_lot(const CityState &s, const Building &rec, const V2 &p, float radius, float up) {
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	const b40::LotFrame f = b40::lot_frame(rec);
	const V2 d = p - f.o;
	const float x = d.dot(f.e0), y = d.dot(f.e1);
	const auto inside = [&](float x0, float y0, float x1, float y1) { return x >= x0 - radius && x <= x1 + radius && y >= y0 - radius && y <= y1 + radius; };
	for (const b40::Volume &v : body.volumes)
		if (body.level_z(v.top()) + ROOF_M >= up && inside(v.x, v.y, v.x + v.w, v.y + v.d)) return false;
	for (const b40::LotElement &e : body.elements)
		if (e.z + e.h >= up && inside(e.x - e.w * 0.5f, e.y - e.d * 0.5f, e.x + e.w * 0.5f, e.y + e.d * 0.5f)) return false;
	std::vector<b40::LotTree> trees;
	b40::lot_trees(body, f, trees);
	for (const b40::LotTree &t : trees)
		if (t.height >= up && (t.at - p).len() <= radius + t.crown) return false;
	return true;
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

// One yard's plan (traffic::YardCost and its docks), by building: its docks and gates, how many have a way and how far each dock's
// ways got (traffic::YardReach), the time by part, and each kind of search (traffic::YardSearch) as [ran, found, distance cells,
// distance ms, poses expanded, expanding ms, clearance cells read, Reeds-Shepp lengths, poses queued, pages of headings taken].
Dictionary yard_entry(int id, const traffic::YardPlan &p) {
	const traffic::YardCost &c = p.cost;
	Dictionary y;
	y["building"] = id;
	y["docks"] = int(p.docks.size());
	y["gates"] = int(p.gates.size());
	int ways = 0;
	Array reached;
	reached.resize(traffic::YARD_REACHES);
	reached.fill(0);
	for (const traffic::DockWays &w : p.docks) {
		ways += w.gate >= 0 ? 1 : 0;
		reached[w.reached] = int(reached[w.reached]) + 1;
	}
	y["with_way"] = ways;
	y["reached"] = reached;
	y["total_ms"] = c.total_ms;
	y["field_ms"] = c.field_ms;
	y["dock_ms"] = c.dock_ms;
	y["ways_ms"] = c.ways_ms;
	y["slice_ms"] = c.slice_ms;
	y["slice_stage"] = c.slice_stage;
	Array kinds;
	for (int k = 0; k < traffic::YARD_SEARCHES; k++) {
		const traffic::SearchCost &s = c.search[k];
		Array row;
		for (const Variant &v : { Variant(c.searches[k]), Variant(c.found[k]), Variant(s.holonomic), Variant(s.holonomic_ms), Variant(s.expanded), Variant(s.expand_ms),
				 Variant(s.looks), Variant(s.rs), Variant(s.queued), Variant(s.pages) })
			row.push_back(v);
		kinds.push_back(row);
	}
	y["searches"] = kinds;
	return y;
}

// The yards planned so far, by building.
Array yard_costs(const ParkingState &pk) {
	std::vector<int> ids;
	for (const auto &y : pk.yards) ids.push_back(y.first);
	std::sort(ids.begin(), ids.end());
	Array out;
	for (int id : ids) out.push_back(yard_entry(id, *pk.yards.at(id)));
	return out;
}
} // namespace

// The bench of the yard planner: every building's yard planned to the end now, from the city as it stands, none of it kept. The same
// saved city gives every version of the planner the same yards to plan (the game's own planning follows the ticks, so its yards
// change with how fast it plans).
Array CityProbes::plan_all_yards() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex); // the lots read are the traffic's, as the step reads them
	traffic::read_lots_with_docks(*state_);
	const CityState &s = *state_;
	Array out;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		traffic::YardInput in;
		if (!traffic::yard_input(s, id, in)) continue;
		traffic::YardPlanner planner(std::move(in));
		while (!planner.advance(int(s.catalog.traffic.lorry_plan_work))) {} // a tick's budget a call, as the game plans
		out.push_back(yard_entry(id, planner.plan));
	}
	return out;
}

Dictionary CityProbes::park_sample() {
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
	Array reached;
	reached.resize(traffic::YARD_REACHES);
	reached.fill(0);
	for (int id : s.traffic.micro_list) trucks += s.traffic.vehicles[size_t(id)].type == VehicleType::TRUCK ? 1 : 0;
	for (const auto &y : pk.yards) {
		gates += int(y.second->gates.size());
		for (const traffic::DockWays &w : y.second->docks) {
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
	d["yard_slowest_ms"] = pk.plan_slowest_ms;
	d["yard_tick_ms"] = pk.plan_tick_ms;
	d["yard_costs"] = yard_costs(pk);
	for (const Lorry &l : pk.lorries)
		if (l.phase != LorryPhase::COMING) d["lorry"] = Vector3(s.traffic.vehicles[size_t(l.id)].cur.x, s.traffic.vehicles[size_t(l.id)].cur.y, s.traffic.vehicles[size_t(l.id)].cur.z);
	return d;
}

Dictionary CityProbes::look_twins(float reach_m, float alike) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex); // a look not yet read is kept
	CityState &s = *state_;
	int pairs = 0, twins = 0;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &b = s.buildings.items[size_t(id)];
		if (!b.alive || b.fn != FN_NONE) continue;
		int nearest = -1;
		float best = reach_m;
		s.buildings.query(b.x - reach_m, b.z - reach_m, b.x + reach_m, b.z + reach_m, [&](int other) {
			const Building &o = s.buildings.items[size_t(other)];
			const float d = std::hypot(o.x - b.x, o.z - b.z);
			if (other != id && o.fn == FN_NONE && d < best) {
				best = d;
				nearest = other;
			}
		});
		if (nearest < 0) continue;
		pairs++;
		twins += look_distance(look_of(s, b), look_of(s, s.buildings.items[size_t(nearest)])) < alike ? 1 : 0;
	}
	Dictionary d;
	d["pairs"] = pairs;
	d["twins"] = twins;
	return d;
}

int CityProbes::dock_count() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	int docks = 0;
	for (const Building &rec : s.buildings.items) {
		if (!rec.alive) continue;
		for (const b40::LotElement &e : b40::record_body(s.buildings.designs, rec)->elements) docks += e.kind == b40::EL_DOCK ? 1 : 0;
	}
	return docks;
}

Variant CityProbes::kerb_busy() {
	const TrafficRead lk(*state_);
	const TrafficState &t = state_->traffic;
	for (const KerbStop &k : t.kerb_stops)
		if (k.vehicle >= 0 && k.phase == KerbPhase::TRANSFER && k.ped >= 0) {
			const Pose &q = t.peds[size_t(k.ped)].cur;
			return Vector3(q.x, q.y, q.z);
		}
	return Variant();
}

bool CityProbes::camera_clear(const Vector3 &at, float radius, bool vehicles) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	bool clear = true;
	const float up = at.y - s.terrain.height_at(at.x, at.z);
	s.buildings.query(at.x - radius, at.z - radius, at.x + radius, at.z + radius, [&](int id) { clear = clear && clear_of_lot(s, s.buildings.items[size_t(id)], V2(at.x, at.z), radius, up); });
	clear = clear && (up > FURNITURE_TOP_M || clear_of_furniture(s, at, radius));
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
