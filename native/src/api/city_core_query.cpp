// Read-only views for the UI and the tests. Values are derived here from CityState; the UI never recomputes them.
#include "api/city_core.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float RAY_MAX_M = 30000.f;
constexpr float RAY_DIR_MIN = 1e-6f; // a ray with a shorter ground direction is vertical
constexpr float INSPECT_RADIUS_M = 30.f;
constexpr float NO_BOUND = 1e9f; // an empty city_bounds still has finite numbers
} // namespace

Dictionary CityCore::constants() const {
	Dictionary d;
	d["speed_paused"] = int(SimSpeed::PAUSED);
	d["speed_fastest"] = int(SimSpeed::FASTEST);
	d["minutes_per_day"] = MINUTES_PER_DAY;
	d["minutes_per_hour"] = MINUTES_PER_HOUR;
	d["zone_codes"] = ZONE_CODES;
	return d;
}

Dictionary CityCore::snapshot() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary d;
	d["ready"] = s.ready;
	if (!s.ready) return d;
	d["money"] = s.money;
	d["unlimited"] = s.unlimited_money;
	d["hour"] = s.clock.hour();
	d["minutes"] = s.clock.minutes.load();
	d["buildings"] = s.buildings.count;
	d["segments"] = s.roads.alive_segments();
	if (s.edit_revision != snap_edit_revision_) {
		int nodes = 0;
		for (const RoadNode &n : s.roads.nodes) nodes += n.alive ? 1 : 0;
		int zoned = 0;
		for (const RoadSegment &g : s.roads.segs) zoned += g.alive ? (g.zl != 0) + (g.zr != 0) : 0;
		int residents = 0, jobs = 0;
		const OccupancyDef &o = s.catalog.occupancy;
		const float per[4] = { o.m2_per_resident, o.m2_per_job_commercial, o.m2_per_job_industrial, o.m2_per_person_mixed };
		for (const Building &b : s.buildings.items) {
			if (!b.alive) continue;
			int r, j;
			BuildingStore::occupancy(b, o.floor_share, per, r, j);
			residents += r;
			jobs += j;
		}
		snap_edit_revision_ = s.edit_revision;
		snap_nodes_ = nodes;
		snap_zoned_ = zoned;
		snap_residents_ = residents;
		snap_jobs_ = jobs;
	}
	d["nodes"] = snap_nodes_;
	d["zoned_sides"] = snap_zoned_;
	d["grown"] = s.growth.grown;
	d["growth_cells"] = int(s.growth.cells.size());
	d["residents"] = snap_residents_;
	d["jobs"] = snap_jobs_;
	d["vehicles"] = s.traffic.vehicle_count;
	d["people"] = s.traffic.ped_count;
	d["trips_arrived"] = s.traffic.arrived;
	d["trips_lost"] = s.traffic.lost;
	d["speed"] = int(sim_.speed());
	d["unsaved"] = s.has_unsaved_work();
	d["edited"] = s.edited;
	d["can_undo"] = !s.history.undo_stack.empty();
	d["can_redo"] = !s.history.redo_stack.empty();
	d["undo_label"] = String(s.history.undo_label().c_str());
	d["redo_label"] = String(s.history.redo_label().c_str());
	d["map"] = s.map_index;
	d["seed"] = int64_t(s.seed & ~TerrainGen::FLAT_BIT); // the city's seed (the flag bit is the terrain's, city_gen.cpp)
	return d;
}

float CityCore::height_at(float x, float z) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->ready ? state_->terrain.height_at(x, z) : 0.f;
}

Dictionary CityCore::raycast(const Vector3 &origin, const Vector3 &dir) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	d["hit"] = false;
	if (!state_->ready) return d;
	V2 d2(dir.x, dir.z);
	float l2 = d2.len();
if (l2 < RAY_DIR_MIN && dir.y >= 0) return d;
	float hx, hy, hz;
	if (!state_->terrain.raycast(V2(origin.x, origin.z), origin.y, d2, dir.y, RAY_MAX_M, hx, hy, hz)) return d;
	d["hit"] = true;
	d["position"] = Vector3(hx, hy, hz);
	return d;
}

Dictionary CityCore::inspect(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	int best = -1;
	float bd = INSPECT_RADIUS_M;
	s.buildings.query(p.x - INSPECT_RADIUS_M, p.z - INSPECT_RADIUS_M, p.x + INSPECT_RADIUS_M, p.z + INSPECT_RADIUS_M, [&](int id) {
		const Building &b = s.buildings.items[id];
		float dd = std::hypot(b.x - p.x, b.z - p.z);
		if (dd < bd) { bd = dd; best = id; }
	});
	if (best >= 0) {
		const Building &b = s.buildings.items[best];
		d["kind"] = "building";
		d["id"] = best;
		d["floors"] = int(b.floors);
		d["zone"] = int(b.zone);
		d["fn"] = int(b.fn);
		return d;
	}
	double t, dist;
	int seg = s.roads.find_segment_near(p.x, p.z, INSPECT_RADIUS_M, t, dist);
	if (seg >= 0) {
		d["kind"] = "road";
		d["id"] = seg;
		d["profile"] = String(s.catalog.profile(s.roads.segs[seg].profile).name_key.c_str());
	}
	return d;
}

Dictionary CityCore::probe_building(int id) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	const Building &b = s.buildings.items[id];
	d["position"] = Vector3(b.x, s.terrain.height_at(b.x, b.z), b.z);
	d["w"] = b.w;
	d["d"] = b.d;
	d["rot"] = b.rot;
	d["floors"] = int(b.floors);
	d["zone"] = int(b.zone);
	d["fn"] = int(b.fn);
	return d;
}

Array CityCore::list_buildings() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array a;
	const auto &items = state_->buildings.items;
	for (int i = 0; i < int(items.size()); i++)
		if (items[i].alive) a.push_back(i);
	return a;
}

Dictionary CityCore::city_bounds() const {
	Dictionary d;
	float minx = NO_BOUND, minz = NO_BOUND, maxx = -NO_BOUND, maxz = -NO_BOUND;
	for (const RoadNode &n : state_->roads.nodes) {
		if (!n.alive) continue;
		minx = std::min(minx, float(n.x)); maxx = std::max(maxx, float(n.x));
		minz = std::min(minz, float(n.z)); maxz = std::max(maxz, float(n.z));
	}
	d["min"] = Vector3(minx, 0, minz);
	d["max"] = Vector3(maxx, 0, maxz);
	d["half"] = state_->map_half;
	return d;
}

} // namespace city
