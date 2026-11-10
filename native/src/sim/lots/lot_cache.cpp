// Lot cells recomputed from the streets around them, in parallel (each cell is a pure function of its streets).
#include "sim/lots/lot_cache.h"

#include "sim/citygen/road_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <thread>

namespace city {

namespace {
constexpr double LOT_LOOK = 160; // streets this far past the cell can shape its lots (blocks reach MARGIN past it)
constexpr size_t LOT_SERIAL_MAX = 4; // below this, the pool's hand-off costs more than the serial path
} // namespace

namespace {
constexpr uint32_t PLAYER_LOT_SALT = 0x10755eedu; // the player's lots' stream of a lot's id (any word)

// The lots the streets propose give way to those the player drew, and a zoned one of the player's whose middle is in the cell joins them.
std::vector<Lot> with_player_lots(std::vector<Lot> lots, int64_t key, const LotBook &players) {
	if (players.empty()) return lots;
	lots.erase(std::remove_if(lots.begin(), lots.end(), [&](const Lot &l) { return players.at(l.x, l.z) != 0; }), lots.end());
	players.zoned_in(key, [&](const PlayerLot &p, const LotStand &st) {
		Lot l;
		l.x = st.x;
		l.z = st.z;
		l.w = st.w;
		l.d = st.d;
		l.rot = st.rot;
		l.zone = p.zone;
		l.seed = uint32_t(p.id) * PLAYER_LOT_SALT;
		lots.push_back(l);
	});
	return lots;
}
} // namespace

// The designs of their own the lots grow, for the lots' cutter (sim/citygen lot_gen.h GrownPlans): by zone code the plans of its
// kind's designs, and the one a lot grows, asked as its building will be made (b40 Data::grown_pick_at), so the lot cut for a design
// is the lot that grows it.
static GrownPlans grown_plans(const CityState &s) {
	const b40::Data &designs = s.buildings.designs;
	GrownPlans out;
	out.by_zone.resize(ZONE_CODES);
	std::vector<int> place(designs.grown_designs.size(), -1); // each design's place among its kind's, as by_zone lists them
	for (int kind = 0; kind < b40::KIND_COUNT; kind++) {
		int next = 0;
		for (size_t i = 0; i < designs.grown_designs.size(); i++)
			if (designs.grown_designs[i].kind == b40::Kind(kind)) place[i] = next++;
	}
	for (int code = 0; code < ZONE_CODES; code++) {
		const b40::Kind kind = designs.kind_of_zone(code);
		if (designs.grown_share[kind] <= 0) continue;
		for (const b40::GrownDesign &g : designs.grown_designs)
			if (g.kind == kind) out.by_zone[size_t(code)].push_back({ double(g.body->plan.W), double(g.body->plan.D) });
	}
	out.pick = [&designs, place](const Lot &l) {
		const int pick = designs.grown_pick_at(float(l.x), float(l.z), float(l.d), float(l.rot), l.zone);
		return pick < 0 ? -1 : place[size_t(pick)];
	};
	return out;
}

// The lots of a cell from the streets; `every_side`: as if each side of each road were zoned, the proposal the Lots tab offers on land nobody cut yet.
static std::vector<Lot> lots_of_cell(const CityState &s, int64_t key, int every_side) {
	double x0 = key_x(key) * double(CHUNK), z0 = key_y(key) * double(CHUNK);
	LotGraph g;
	std::unordered_map<int, int> map;
	auto node = [&](int n) {
		auto it = map.find(n);
		if (it != map.end()) return it->second;
		const RoadNode &nd = s.roads.nodes[n];
		int id = g.nodes();
		map[n] = id;
		g.nx.push_back(nd.x);
		g.nz.push_back(nd.z);
		const RoadSegment &end = s.roads.segs[size_t(nd.segs.front())];
		g.bulb.push_back(nd.bulb ? bulb_round(s.catalog.tune, arm_of(end.q, s.roads.profile_of(nd.segs.front()), end.a == n ? 0 : 1)) : 0.0);
		g.joint.push_back(nd.joint ? 1 : 0);
		return id;
	};
	s.roads.for_segments_near(x0 - LOT_LOOK, z0 - LOT_LOOK, x0 + CHUNK + LOT_LOOK, z0 + CHUNK + LOT_LOOK, [&](int sid) {
		const RoadSegment &seg = s.roads.segs[sid];
		g.quad.push_back(seg.q);
		g.sa.push_back(node(seg.a));
		g.sb.push_back(node(seg.b));
		g.profile.push_back(seg.profile);
		g.zl.push_back(every_side != 0 ? uint8_t(every_side) : seg.zl);
		g.zr.push_back(every_side != 0 ? uint8_t(every_side) : seg.zr);
	});
	if (g.segs() == 0) return {};
	LotGround ground;
	ground.height = [&s](double x, double z) { return double(s.terrain.base_height_at(float(x), float(z))); };
	ground.water = [&s](double x, double z) { return double(s.terrain.water_at(float(x), float(z))); };
	if (const ScenicCity *city = s.catalog.scenic_of(s.map_index)) ground.closed = [city](double x, double z) { return city->in_park(V2(float(x), float(z))); };
	return build_lots(s.catalog.growth.shapes, g, s.catalog.profiles, s.catalog.gen_zones, grown_plans(s), ground, x0, z0);
}

std::vector<Lot> LotCache::compute(const CityState &s, int64_t key) { return with_player_lots(lots_of_cell(s, key, 0), key, s.lot_book); }

std::vector<Lot> LotCache::proposal(const CityState &s, int64_t key) { return lots_of_cell(s, key, s.catalog.gen_zones.residential[0]); } // a proposal is a house's

void LotCache::process_dirty(CityState &s) {
	std::vector<int64_t> cells = s.log.lot_cells;
	s.log.lot_cells.clear();
	ChangeLog::dedup(cells);
	if (cells.empty()) return;
	std::vector<std::vector<Lot>> results(cells.size());
	if (cells.size() < LOT_SERIAL_MAX) {
		for (size_t i = 0; i < cells.size(); i++) results[i] = compute(s, cells[i]);
	} else if (s.jobs) {
		s.jobs->parallel_for(cells.size(), [&](size_t i) { results[i] = compute(s, cells[i]); });
	} else {
		for (size_t i = 0; i < cells.size(); i++) results[i] = compute(s, cells[i]);
	}
	for (size_t i = 0; i < cells.size(); i++) {
		if (results[i].empty()) by_cell.erase(cells[i]);
		else by_cell[cells[i]] = std::move(results[i]);
	}
}

} // namespace city
