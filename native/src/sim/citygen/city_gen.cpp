// The generated-city pipeline (Roadcraft app/cityFlows.ts:67-134).
#include "sim/citygen/city_gen.h"

#include "sim/commands/road_grade_sync.h"

#include "sim/buildings/growth.h"
#include "sim/street/street_names.h"
#include "sim/citygen/road_gen.h"
#include "sim/citygen/squares.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>

namespace city {

namespace {
GenProfiles gen_profiles(const Catalog &c) {
	GenProfiles g;
	g.local = c.profile_id("cityLocal"); // the three sections of the 4.0's cities (world/cityGen)
	g.avenue = c.profile_id("cityAvenue");
	g.avenue_bike = c.profile_id("avenueWithCycleTrack");
	g.expressway = c.profile_id("expressway");
	g.collector = c.profile_id("cityUrban");
	g.wide = c.profile_id("wideSidewalks");
	g.urban = c.profile_id("cityUrban");
	g.cobble = c.profile_id("cobblestoneStreet");
	g.green = c.profile_id("streetWithMedian");
	g.concrete = c.profile_id("concreteRoad");
	g.bike = c.profile_id("streetWithBikeLane");
	g.bus_side = c.profile_id("curbsideBusLane");
	g.boulevard = c.profile_id("boulevard");
	g.av4 = c.profile_id("cityAvenue");
	for (const Profile &p : c.profiles) g.asph_right.push_back(p.asph_right);
	return g;
}

void insert_roads(CityState &s, const GeneratedRoads &r) {
	std::vector<int> ids(r.node_count());
	for (int n = 0; n < r.node_count(); n++) ids[n] = s.roads.add_node(r.nodes[n * 3], r.nodes[n * 3 + 1], r.nodes[n * 3 + 2]);
	for (int k = 0; k < r.seg_count(); k++) {
		const int *g = &r.segs[k * SEG_FIELDS];
		int a = ids[g[0]], b = ids[g[1]];
		const RoadNode &na = s.roads.nodes[a], &nb = s.roads.nodes[b];
		const Quad q = Quad::straight(na.x, na.z, nb.x, nb.z);
		// A street across water is a bridge (road_grade_sync.h); its height is solved with the rest (generate_map).
		s.roads.add_segment(a, b, q, g[2], g[3], g[4], crosses_water(s, q) ? with_structure(SEG_GENERATED, Structure::BRIDGE) : SEG_GENERATED);
	}
	// Only the generator's own dead ends turn in a bulb: a road demolished later leaves a straight cut.
	for (int n : ids) s.roads.nodes[n].plain = s.roads.nodes[n].segs.size() != 1;
	s.roads.settle_shapes();
}

} // namespace

void mark_all_lot_cells(CityState &s) {
	int cells = int(std::lround(s.map_half * 2 / CHUNK)), first = -cells / 2;
	for (int j = first; j < first + cells; j++)
		for (int i = first; i < first + cells; i++) s.log.lot_cells.push_back(cell_key(i, j));
}

std::unique_ptr<TerrainGen> found_land(CityState &s, int map_index, uint32_t exact_seed, bool generate, int tiles) {
	s.transit = TransitData(); // a new map has no public transport; a save reads its own after this
	s.transit_revision++;
	const Catalog &c = s.catalog;
	const MapDef &map = c.maps[size_t(map_index) < c.maps.size() ? size_t(map_index) : 0];
	s.map_index = map_index;
	s.map_tiles = tiles > 0 ? tiles : map.tiles;
	s.map_half = float(s.map_tiles * c.tune.world_tile_m / 2);
	s.seed = exact_seed;
	s.roads.init(&s.catalog, &s.log);
	s.street.clear();
	s.lot_book.clear();
	s.buildings.init(&s.log, &s.catalog.occupancy);
	s.lots.clear();
	s.growth.clear();
	s.damage.clear(); // what blows left on the last map is not on this one
	s.history.reset(c.tune);
	s.touch(); // another map: what is cached by the revision (the camera's building volumes) goes with the old one
	s.edit_revision++; // another map is another network: what follows the edits (the snapshot's counts, the views' lists) reads it again
	s.edited = false; // a city just made or opened holds no edit of the player's yet
	s.money = c.tune.economy_start_money;
	s.unlimited_money = false;
	s.clock.minutes.store(0);
	s.clock.day_shift = c.tune.sim_day_start_min;
	s.terrain.init(s.map_tiles, c.tune.world_tile_m, c.tune.world_height_spacing_m, s.seed, &s.log, &s.catalog.tune, s.jobs.get());
	s.nature.ground.init(s.terrain.N, s.terrain.spacing, &c.nature.paint);
	s.nature.reset(default_weather(c.nature.weather), c.nature.ecology.default_biome, s.seed & ~TerrainGen::FLAT_BIT);
	if (!generate) return nullptr;
	auto gen = std::make_unique<TerrainGen>(s.seed, s.map_half, c.tune.world_water_spacing_m);
	s.terrain.fill(*gen, c.tune.world_water_spacing_m);
	return gen;
}

// Every street of the generated city takes its name, as the Roadcraft's does.
void name_streets(CityState &s) {
	std::vector<int> segs;
	for (size_t i = 0; i < s.roads.segs.size(); i++) segs.push_back(int(i));
	for (const StreetName &n : names_for(s.roads, s.street, std::move(segs))) {
		StreetItem it;
		it.kind = ItemKind::STREETNAME;
		it.x = n.x;
		it.z = n.z;
		it.text = n.text;
		s.street.add_item(it);
	}
}

CityGenReport generate_map(CityState &s, int map_index, uint32_t seed, bool with_city) {
	auto t0 = std::chrono::steady_clock::now();
	const Catalog &c = s.catalog;
	const MapDef &map = c.maps[size_t(map_index) < c.maps.size() ? size_t(map_index) : 0];
	bool medium = map.medium && with_city;
	// the generator that filled the land lays the streets on it: its river and lakes are planned once
	const std::unique_ptr<TerrainGen> gen = found_land(s, map_index, medium ? (seed | TerrainGen::FLAT_BIT) : (seed & ~TerrainGen::FLAT_BIT));
	CityGenReport rep;
	if (with_city) {
		GeneratedRoads r = generate_roads(s.seed, s.map_half, map.city_radius_m, *gen, gen_profiles(c), c.growth.plan);
		insert_roads(s, r);
		s.roads.grade.everything(); // every street solved on the land, and the ground shaped to them
		sync_grade(s);
		mark_all_lot_cells(s);
		s.lots.process_dirty(s);
		rep.lots = int(s.lots.total());
		rep.squares = plan_squares(s, seed);
		rep.buildings = fill_all(s);
		name_streets(s);
		settle_name_plates(s.street, s.roads, s.log); // the plates of the names given, on the roads as they are now settled
		rep.nodes = r.node_count();
		rep.segments = r.seg_count();
	}
	s.ready = true;
	s.saved_revision = s.revision;
	rep.seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
	return rep;
}

} // namespace city
