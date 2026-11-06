// Read-only views for the UI and the tests. Values are derived here from CityState; the UI never recomputes them.
#include "api/city_core.h"

#include "render/render_layers.h"

#include "core/math.h"
#include "core/units.h"
#include "render/film_grade.h"
#include "sim/buildings/lot_access.h"
#include "sim/commands/interior_commands.h"
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
constexpr float NO_EDGE = 1e9f; // an empty city_bounds still has finite numbers
constexpr float HALF = 0.5f;

// The size of a body for the city statistics: storeys of its highest volume, the top of its walls (m), its footprint
// and its floor area (m2, every volume's outline x its storeys), its ground and upper storey heights and its style.
Dictionary body_probe(const b40::Body &body) {
	int top = 0;
	float footprint = 0, floor_area = 0;
	for (const b40::Volume &v : body.volumes) {
		top = std::max(top, v.top());
		float share = 1; // the outline's area as a share of its rectangle (shoelace)
		if (!v.outline.empty()) {
			float twice = 0;
			for (size_t i = 0; i < v.outline.size(); i++) {
				const b40::Point2 &a = v.outline[i], &b = v.outline[(i + 1) % v.outline.size()];
				twice += a.x * b.y - b.x * a.y;
			}
			share = std::fabs(twice) * HALF;
		}
		const float area = v.w * v.d * share;
		footprint += v.base == 0 ? area : 0;
		floor_area += area * float(v.storeys);
	}
	Dictionary d;
	d["storeys"] = top;
	d["wall_top"] = body.level_z(top);
	d["footprint"] = footprint;
	d["floor_area"] = floor_area;
	d["ground_h"] = body.ground_h;
	d["storey_h"] = body.storey_h;
	d["style"] = int(body.style);
	return d;
}

// What the lot of a record holds and how its traffic reaches it (tests: every house parks, every bay is reachable).
Dictionary lot_probe(const CityState &s, const Building &rec) {
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	LotAccess acc;
	lot_access(s.buildings.designs, body, b40::lot_frame(rec), car_half_width(s.catalog.traffic), lorry_half_width(s.catalog.traffic), acc);
	int trees = 0, car_gates = 0, docks = 0, garages = 0;
	for (const b40::LotElement &e : body.elements) trees += e.kind == b40::EL_TREE ? 1 : 0;
	for (const LotGate &g : acc.gates) car_gates += g.car ? 1 : 0;
	for (const LotBay &b : acc.bays) docks += b.dock ? 1 : 0;
	for (const b40::Volume &v : body.volumes) {
		auto it = v.bays.find(b40::bay_key(0, 0, 0));
		garages += v.keep && it != v.bays.end() && it->second == b40::BAY_GARAGE_DOOR ? 1 : 0;
	}
	Dictionary d;
	d["kind"] = int(body.kind);
	d["elements"] = int(body.elements.size());
	d["trees"] = trees;
	d["gates"] = int(acc.gates.size());
	d["car_gates"] = car_gates;
	d["bays"] = int(acc.bays.size()) - docks;
	d["docks"] = docks;
	d["reached"] = acc.reached();
	d["garages"] = garages;
	const b40::LotPlan &p = body.plan;
	d["plan"] = Array::make(int(p.front), p.front_depth, int(p.left.use), p.left.width, int(p.right.use), p.right.width, int(p.back), p.back_depth, p.rows);
	d["body"] = body_probe(body);
	return d;
}
} // namespace

Dictionary CityCore::constants() const {
	Dictionary d;
	d["speed_paused"] = int(SimSpeed::PAUSED);
	d["speed_fastest"] = int(SimSpeed::FASTEST);
	d["minutes_per_day"] = MINUTES_PER_DAY;
	d["minutes_per_hour"] = MINUTES_PER_HOUR;
	d["zone_codes"] = ZONE_CODES;
	Dictionary layers;
	for (const RenderLayerName &l : RENDER_LAYERS) layers[l.name] = int64_t(l.bit);
	d["render_layers"] = layers;
	return d;
}

TypedArray<Image> CityCore::film_grade(const Dictionary &grade) const {
	return film_grade_slices(grade);
}

Color CityCore::film_graded(const Dictionary &grade, const Color &colour) const {
	return city::film_graded(grade, colour);
}

// Under the map lock shared only: the fleet's numbers are the traffic's last publication (TrafficView) and the panels' levels
// are atomics, so the frame that reads the snapshot never waits for a traffic step; residents and jobs are the
// BuildingStore's running totals.
Dictionary CityCore::snapshot() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
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
	d["marking_style"] = s.marking_style.load();
	d["edit_revision"] = int64_t(s.edit_revision);
	d["player_zones"] = s.roads.player_sides;
	d["style"] = String(s.style.c_str());
	if (s.edit_revision != snap_.edit_revision) {
		int nodes = 0;
		for (const RoadNode &n : s.roads.nodes) nodes += n.alive ? 1 : 0;
		int zoned = 0;
		for (const RoadSegment &g : s.roads.segs) zoned += g.alive ? (g.zl != 0) + (g.zr != 0) : 0;
		snap_.edit_revision = s.edit_revision;
		snap_.nodes = nodes;
		snap_.zoned = zoned;
	}
	d["nodes"] = snap_.nodes;
	d["zoned_sides"] = snap_.zoned;
	d["grown"] = s.growth.grown;
	d["growth_cells"] = int(s.growth.cells.size());
	d["residents"] = s.buildings.residents;
	d["jobs"] = s.buildings.jobs;
	d["transit_revision"] = int64_t(s.transit_revision); // views that list the transport plan read it again only when it moves
	const TrafficView &fleet = state_->traffic_views.acquire();
	d["vehicles"] = fleet.vehicles;
	d["people"] = fleet.people;
	d["trips_arrived"] = fleet.arrived;
	d["trips_lost"] = fleet.lost;
	d["traffic_level"] = s.traffic_levels.traffic.load();
	d["people_level"] = s.traffic_levels.people.load();
	d["demand"] = s.traffic_levels.demand.load();
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
	// What the player sees under the ray (the 4.0's worldAtScreen): the ground, or a road deck the ray meets before it.
	float hx, hy, hz;
	const bool ground = state_->terrain.raycast(V2(origin.x, origin.z), origin.y, d2, dir.y, RAY_MAX_M, hx, hy, hz);
	double t_ground = RAY_MAX_M;
	if (ground) t_ground = l2 >= RAY_DIR_MIN ? V2(hx - origin.x, hz - origin.z).len() / l2 : (origin.y - hy) / -dir.y;
	double t_deck;
	if (state_->roads.ray_deck(origin.x, origin.y, origin.z, dir.x, dir.y, dir.z, t_ground, t_deck)) {
		hx = float(origin.x + dir.x * t_deck);
		hy = float(origin.y + dir.y * t_deck);
		hz = float(origin.z + dir.z * t_deck);
	} else if (!ground) {
		return d;
	}
	d["hit"] = true;
	d["position"] = Vector3(hx, hy, hz);
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
	d["lot"] = lot_probe(s, b);
	const InteriorFocus body = interior_focus(s, id);
	d["focus"] = Vector3(body.at.x, s.terrain.height_at(body.at.x, body.at.y), body.at.y);
	d["extent"] = body.extent;
	return d;
}

// What the bodies of one kind are made of, on average: to hold a city against the 4.0's records.
Dictionary CityCore::body_census(int kind) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	static constexpr const char *ELEMENTS[] = { "pavement", "slab", "pillar", "canopy", "awning", "wall", "fence", "railing", "hedge", "gate", "car_gate", "planter", "bench",
		"bollard", "lamp", "bin", "ac", "surface", "stall", "dock", "drain", "tree", "shrub", "flowers", "pool", "lounger", "table", "swing", "shed", "line", "pallet", "cage",
		"stair", "ramp", "rocks", "parking", "clock" };
	int n = 0, volumes = 0;
	double foot = 0, lot_w = 0, lot_d = 0, main = 0;
	std::vector<int> elements(sizeof(ELEMENTS) / sizeof(ELEMENTS[0]));
	Dictionary covers; // the ground the lot lays, by colour: its area
	for (const Building &rec : s.buildings.items) {
		if (!rec.alive) continue;
		b40::Body body;
		b40::record_body(s.buildings.designs, rec, body);
		if (int(body.kind) != kind) continue;
		n++;
		lot_w += body.plan.W;
		lot_d += body.plan.D;
		double largest = 0;
		for (const b40::Volume &v : body.volumes) {
			volumes++;
			foot += double(v.w) * v.d;
			largest = std::max(largest, double(v.w) * v.d);
		}
		main += largest;
		for (const b40::LotElement &e : body.elements) {
			if (size_t(e.kind) < elements.size()) elements[size_t(e.kind)]++;
			if (e.kind == b40::EL_SURFACE && e.paint >= 0 && size_t(e.paint) < body.paints.size()) {
				const String tone = String::num_int64(int64_t(body.paints[size_t(e.paint)].rgb), 16);
				covers[tone] = double(covers.get(tone, 0.0)) + double(e.w) * e.d;
			}
		}
	}
	Dictionary d, per;
	const double each = 1.0 / std::max(n, 1);
	for (const Variant &tone : covers.keys()) covers[tone] = double(covers[tone]) * each;
	d["cover_m2"] = covers;
	d["n"] = n;
	d["volumes"] = volumes * each;
	d["foot_m2"] = foot * each;
	d["largest_m2"] = main * each;
	d["lot_w"] = lot_w * each;
	d["lot_d"] = lot_d * each;
	for (size_t i = 0; i < elements.size(); i++)
		if (elements[i] > 0) per[ELEMENTS[i]] = elements[i] * each;
	d["elements"] = per;
	return d;
}

// The network by road and by junction: metres of each profile, nodes by their arms.
Dictionary CityCore::road_census() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const RoadNetwork &net = state_->roads;
	Dictionary roads, arms, d;
	for (const RoadSegment &g : net.segs) {
		if (!g.alive) continue;
		const String id = net.cat->profile(g.profile).id.c_str();
		roads[id] = double(roads.get(id, 0.0)) + g.len;
	}
	for (const RoadNode &n : net.nodes)
		if (n.alive) arms[int(n.segs.size())] = int(arms.get(int(n.segs.size()), 0)) + 1;
	d["metres"] = roads;
	d["arms"] = arms;
	Array open; // what has no building: where, how big, what function, how many trees and surfaces
	for (const Building &rec : state_->buildings.items) {
		if (!rec.alive || (rec.fn != FN_SQUARE && rec.fn != FN_PARK)) continue;
		b40::Body body;
		b40::record_body(state_->buildings.designs, rec, body);
		int trees = 0, covers = 0;
		for (const b40::LotElement &e : body.elements) {
			trees += e.kind == b40::EL_TREE ? 1 : 0;
			covers += e.kind == b40::EL_SURFACE ? 1 : 0;
		}
		open.push_back(Array::make(int(rec.fn), Vector3(rec.x, 0, rec.z), rec.w, rec.d, trees, covers));
	}
	d["open"] = open;
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
	float minx = NO_EDGE, minz = NO_EDGE, maxx = -NO_EDGE, maxz = -NO_EDGE;
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
