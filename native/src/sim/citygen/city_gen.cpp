// The generated-city pipeline (Roadcraft app/cityFlows.ts:67-134).
#include "sim/citygen/city_gen.h"

#include "sim/buildings/growth.h"
#include "sim/citygen/road_gen.h"
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
	g.local = c.profile_id("streetWithParking");
	g.avenue = c.profile_id("avenue");
	g.avenue_bike = c.profile_id("avenueWithCycleTrack");
	g.expressway = c.profile_id("expressway");
	g.collector = c.profile_id("urbanStreet");
	g.wide = c.profile_id("wideSidewalks");
	g.urban = c.profile_id("urbanStreet");
	g.cobble = c.profile_id("cobblestoneStreet");
	g.green = c.profile_id("streetWithMedian");
	g.concrete = c.profile_id("concreteRoad");
	g.bike = c.profile_id("streetWithBikeLane");
	g.bus_side = c.profile_id("curbsideBusLane");
	g.boulevard = c.profile_id("boulevard");
	g.av4 = c.profile_id("avenue4Lanes");
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
		s.roads.add_segment(a, b, Quad::straight(na.x, na.z, nb.x, nb.z), g[2], g[3], g[4], SEG_FREE);
	}
	for (int n : ids) s.roads.refresh_node(n);
}

// Grade line (Roadcraft roads/construction/execute.ts): a straight generated street whose linear height strays from
// the smoothed ground by more than GRADE_TOL_M is halved at a node on the ground, up to GRADE_DEPTH times, so the
// polyline of node heights follows the land; what is left is evened out by the terrain modifiers below.
constexpr double GRADE_SAMPLE_M = 6, GRADE_TOL_M = 0.8, GRADE_MIN_PIECE_M = 24, GROUND_PROBE_M = 5;
constexpr int GRADE_DEPTH = 3;
// Terrain corridors: sampled every CONFORM_STEP_M along the curve, flat over the road plus FLAT_PAD_SAMPLES height
// samples (so no terrain triangle straddles a kerb), embankment / cut slopes up to CONFORM_MAX_RUN_M long.
constexpr double CONFORM_STEP_M = 8;
constexpr float FLAT_PAD_SAMPLES = 1.f, CONFORM_MAX_RUN_M = 40.f;
constexpr int JUNCTION_DEGREE = 3, GROUND_PROBES = 5;

double smooth_ground(const Heightfield &t, double x, double z) {
	static constexpr double PROBES[GROUND_PROBES][2] = { { 0, 0 }, { GROUND_PROBE_M, 0 }, { -GROUND_PROBE_M, 0 }, { 0, GROUND_PROBE_M }, { 0, -GROUND_PROBE_M } };
	double h = 0;
	for (const auto &d : PROBES) h += t.base_height_at(float(x + d[0]), float(z + d[1]));
	return h / GROUND_PROBES;
}

void split_by_grade(CityState &s, int sid, int depth, std::vector<int> &touched) {
	const RoadSegment &g = s.roads.segs[sid];
	if (depth >= GRADE_DEPTH || g.len < 2 * GRADE_MIN_PIECE_M) return;
	double worst = 0;
	for (double at = GRADE_SAMPLE_M; at < g.len; at += GRADE_SAMPLE_M) {
		double x, z;
		g.q.at(t_at_length(g.table, at), x, z);
		worst = std::max(worst, std::fabs(smooth_ground(s.terrain, x, z) - s.roads.seg_y(sid, at)));
	}
	double t = t_at_length(g.table, g.len / 2), mx, mz;
	g.q.at(t, mx, mz);
	if (worst < GRADE_TOL_M || s.terrain.is_water(float(mx), float(mz), 0.f)) return;
	std::array<Quad, 2> half = split_quad(g.q, t);
	int a = g.a, b = g.b, prof = g.profile, zl = g.zl, zr = g.zr;
	uint16_t flags = g.flags;
	s.roads.remove_segment(sid);
	int m = s.roads.add_node(mx, mz, std::max(TerrainGen::OCEAN_Y, smooth_ground(s.terrain, mx, mz)));
	int s1 = s.roads.add_segment(a, m, half[0], prof, zl, zr, flags), s2 = s.roads.add_segment(m, b, half[1], prof, zl, zr, flags);
	touched.insert(touched.end(), { a, b, m });
	split_by_grade(s, s1, depth + 1, touched);
	split_by_grade(s, s2, depth + 1, touched);
}

void follow_ground(CityState &s) {
	std::vector<int> ids, touched;
	for (size_t i = 0; i < s.roads.segs.size(); i++)
		if (s.roads.segs[i].alive) ids.push_back(int(i));
	for (int sid : ids) split_by_grade(s, sid, 0, touched);
	for (int n : touched) s.roads.refresh_node(n);
}

// The land under each street follows it (Roadcraft jobs/conform.ts): a non-destructive terrain modifier per segment
// along its curve and a round pad under every junction's plate and corners.
void conform_terrain(CityState &s) {
	float pad = s.terrain.spacing * FLAT_PAD_SAMPLES;
	for (size_t i = 0; i < s.roads.segs.size(); i++) {
		const RoadSegment &g = s.roads.segs[i];
		if (!g.alive) continue;
		std::vector<float> pts;
		int n = std::max(1, int(std::ceil(g.len / CONFORM_STEP_M)));
		for (int k = 0; k <= n; k++) {
			double at = g.len * k / n, x, z;
			g.q.at(t_at_length(g.table, at), x, z);
			pts.insert(pts.end(), { float(x), float(s.roads.seg_y(int(i), at)), float(z) });
		}
		s.terrain.add_modifier(pts, s.catalog.profile(g.profile).half() + pad, CONFORM_MAX_RUN_M);
	}
	for (size_t ni = 0; ni < s.roads.nodes.size(); ni++) {
		const RoadNode &nd = s.roads.nodes[ni];
		if (!nd.alive || (int(nd.segs.size()) < JUNCTION_DEGREE && !nd.bulb)) continue;
		double r = nd.bulb ? BULB_R + BULB_WALK : 0;
		for (int sid : nd.segs) {
			const RoadSegment &g = s.roads.segs[sid];
			double back = g.a == int(ni) ? g.retreat_a : g.retreat_b;
			r = std::max(r, std::hypot(back, double(s.catalog.profile(g.profile).half())));
		}
		s.terrain.add_modifier({ float(nd.x), float(nd.y), float(nd.z) }, float(r) + pad, CONFORM_MAX_RUN_M);
	}
}

} // namespace

void mark_all_lot_cells(CityState &s) {
	int cells = int(std::lround(s.map_half * 2 / CHUNK)), first = -cells / 2;
	for (int j = first; j < first + cells; j++)
		for (int i = first; i < first + cells; i++) s.log.lot_cells.push_back(cell_key(i, j));
}

void found_land(CityState &s, int map_index, uint32_t exact_seed) {
	const Catalog &c = s.catalog;
	const MapDef &map = c.maps[size_t(map_index) < c.maps.size() ? size_t(map_index) : 0];
	s.map_index = map_index;
	s.map_half = float(map.tiles * c.tune.world_tile_m / 2);
	s.seed = exact_seed;
	s.roads.init(&s.catalog, &s.log);
	s.buildings.init(&s.log);
	s.lots.clear();
	s.growth.clear();
	s.history.reset(c.tune);
	s.edited = false; // a city just made or opened holds no edit of the player's yet
	s.money = c.tune.economy_start_money;
	s.unlimited_money = false;
	s.clock.minutes.store(0);
	s.clock.day_shift = c.tune.sim_day_start_min;
	s.terrain.init(map.tiles, c.tune.world_tile_m, c.tune.world_height_spacing_m, s.seed, &s.log, &s.catalog.tune);
	s.nature.ground.init(s.terrain.N, s.terrain.spacing, &c.nature.paint);
	s.nature.reset(default_weather(c.nature.weather), c.nature.ecology.default_biome, s.seed & ~TerrainGen::FLAT_BIT);
	TerrainGen gen(s.seed, s.map_half, c.tune.world_water_spacing_m);
	s.terrain.fill(gen, c.tune.world_water_spacing_m);
}

CityGenReport generate_map(CityState &s, int map_index, uint32_t seed, bool with_city) {
	auto t0 = std::chrono::steady_clock::now();
	const Catalog &c = s.catalog;
	const MapDef &map = c.maps[size_t(map_index) < c.maps.size() ? size_t(map_index) : 0];
	bool medium = map.medium && with_city;
	found_land(s, map_index, medium ? (seed | TerrainGen::FLAT_BIT) : (seed & ~TerrainGen::FLAT_BIT));
	TerrainGen gen(s.seed, s.map_half, c.tune.world_water_spacing_m);
	CityGenReport rep;
	if (with_city) {
		GeneratedRoads r = generate_roads(s.seed, s.map_half, medium, gen, gen_profiles(c));
		insert_roads(s, r);
		follow_ground(s);
		conform_terrain(s);
		mark_all_lot_cells(s);
		s.lots.process_dirty(s);
		rep.lots = int(s.lots.total());
		rep.buildings = fill_all(s);
		rep.nodes = r.node_count();
		rep.segments = r.seg_count();
	}
	s.ready = true;
	s.saved_revision = s.revision;
	rep.seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
	return rep;
}

} // namespace city
