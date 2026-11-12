// Lot cells recomputed from the streets around them, in parallel (each cell is a pure function of its streets).
#include "sim/lots/lot_cache.h"

#include "sim/citygen/road_gen.h"
#include "core/units.h"
#include <limits>
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
constexpr uint32_t PLAYER_LOT_SALT = 0x10755eedu;

// The lots the streets propose give way to those the player drew, and a zoned one of the player's whose middle is in the cell joins them.
std::vector<Lot> with_player_lots(const CityState &s, std::vector<Lot> lots, int64_t key) {
	if (s.lot_book.empty()) return lots;
	lots.erase(std::remove_if(lots.begin(), lots.end(), [&](const Lot &l) { return s.lot_book.at(l.x, l.z) != 0; }), lots.end());
	for (int id : s.lot_book.stands_in(key)) {
		const PlayerLot &p = *s.lot_book.find(id);
		if (p.zone == 0) continue;
		const LotStand &st = s.lot_book.geometry(id)->stand;
		Lot l;
		l.x = st.x;
		l.z = st.z;
		l.w = st.w;
		l.d = st.d;
		l.rot = st.rot;
		l.zone = p.zone;
		l.seed = uint32_t(p.id) * PLAYER_LOT_SALT;
		lots.push_back(l);
	}
	return lots;
}
} // namespace

// The lots of a cell from the streets; `every_side`: as if each side of each road were zoned, the proposal the Lots tab offers on land nobody cut yet.
static std::vector<Lot> lots_of_cell(const CityState &s, int64_t key, int every_side) {
	double x0 = key_x(key) * double(CHUNK), z0 = key_y(key) * double(CHUNK);
	LotGraph g;
	g.raster = &s.lot_raster;
	const double bytes = std::max(0.0, double(s.catalog.tune.sim_lot_raster_cache_mb)) * BYTES_PER_MIB;
	if (std::isfinite(bytes)) g.raster_bytes = bytes >= double(std::numeric_limits<size_t>::max()) ? std::numeric_limits<size_t>::max() : size_t(bytes);
	std::unordered_map<int, int> map;
	auto node = [&](int n) {
		auto it = map.find(n);
		if (it != map.end()) return it->second;
		const RoadNode &nd = s.roads.nodes[n];
		int id = g.nodes();
		map[n] = id;
		g.nx.push_back(nd.x);
		g.nz.push_back(nd.z);
		g.bulb.push_back(nd.bulb ? 1 : 0);
		g.joint.push_back(nd.joint ? 1 : 0);
		return id;
	};
	s.roads.for_segments_near(x0 - LOT_LOOK, z0 - LOT_LOOK, x0 + CHUNK + LOT_LOOK, z0 + CHUNK + LOT_LOOK, [&](int sid) {
		const RoadSegment &seg = s.roads.segs[sid];
		g.quad.push_back(seg.q);
		g.prepared_arcs.push_back(&seg.lot_table);
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
	return build_lots(g, s.catalog.profiles, ground, x0, z0);
}

std::vector<Lot> LotCache::compute(const CityState &s, int64_t key) { return with_player_lots(s, lots_of_cell(s, key, 0), key); }

std::vector<Lot> LotCache::proposal(const CityState &s, int64_t key) { return lots_of_cell(s, key, ZONE_RES_LOW); }

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
		s.growth.invalidate_cell(cells[i]); // includes cells whose last lot disappeared
	}
}

} // namespace city
