// Grid lines and their march over the land (cityGen.ts generateCity, cityField.ts blockLines), the generated city's
// rules for its graph (ranks, zones by the plan, sections by district noise); the cut ends taken back (dryCrossings,
// withoutSpurs).
#include "sim/citygen/road_gen.h"

#include "core/seeded.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
enum Level { AVENUE = 0, COLLECTOR = 1, LOCAL = 2 };
constexpr int RANK_EXPRESSWAY = 3, RANK_AVENUE_BIKE = 2;
constexpr uint32_t NOISE_N = 313, NOISE_M = 919; // the two noises' streams off the seed (any distinct words)

// The generated city's graph: the expressway outranks the avenue with a cycle track, which outranks the avenue; only the
// local street gives way; zones are the plan's; a street's section varies by district noise and its distance share.
class PlanRules : public road_gen::GraphRules {
public:
	PlanRules(const RoadsShape &k, uint32_t seed, const GenProfiles &prof, const CityPlan &plan) : K(k), seed_(seed), prof_(prof), plan_(plan) {}
	int rank(int p) const override { return p == prof_.expressway ? RANK_EXPRESSWAY : p == prof_.avenue_bike ? RANK_AVENUE_BIKE : p == prof_.avenue ? 1 : 0; }
	bool yields(int p) const override { return p == prof_.local; }
	int zone_beside(int p, double x, double z) const override { return p == prof_.expressway ? plan_.zones.none : zone_at(plan_, x, z, p != prof_.local); }
	int section(int p, int zone, double d, double x, double z) const override {
		double n = fbm2(x / K.district_scale_n, z / K.district_scale_n, seed_ + NOISE_N, 2), m = fbm2(x / K.noise_m_scale, z / K.noise_m_scale, seed_ + NOISE_M, 2);
		if (p == prof_.local) {
			if (zone == plan_.zones.commercial[1]) return n > K.avenue_share_n ? prof_.wide : n > K.urban_n ? prof_.urban : prof_.local;
			if (zone == plan_.zones.industrial) return prof_.concrete;
			if (zone == plan_.zones.mixed) return n > K.bike_n ? prof_.bike : n < -K.bike_n ? prof_.green : prof_.local;
			if (d > K.cobble_d && m > K.cobble_m) return prof_.cobble;
			return n > K.urban_local_n ? prof_.urban : p;
		}
		if (p == prof_.avenue) {
			if (d < K.bus_offset && m > K.bus_m) return prof_.bus_side;
			if (m < K.boulevard_m) return prof_.boulevard;
			if (n > K.av4_n) return prof_.av4;
		}
		return p;
	}

private:
	const RoadsShape &K; // growth.json shapes.roads
	uint32_t seed_;
	const GenProfiles &prof_;
	const CityPlan &plan_;
};

int level_of(const RoadsShape &K, int k, int n) {
	if (k == 0 || k == n) return AVENUE;
	if (n < K.avenue_every * 2) return k == n / 2 ? COLLECTOR : LOCAL;
	if (k % K.avenue_every == 0) return AVENUE;
	return k % (K.avenue_every / 2) == 0 ? COLLECTOR : LOCAL;
}

struct Line {
	int level;
	std::vector<double> pts; // x, z pairs
};

std::vector<Line> block_lines(const RoadsShape &K, double R) {
	int nx = std::max(2, int(std::floor(R * K.grid_span / K.block_along))), nz = std::max(2, int(std::floor(R * K.grid_span / K.block_across)));
	double x0 = -(nx * K.block_along) / 2, z0 = -(nz * K.block_across) / 2;
	std::vector<Line> out;
	for (int j = 0; j <= nz; j++) {
		Line l{ level_of(K, j, nz), {} };
		for (int i = 0; i <= nx * K.points_along; i++) { l.pts.push_back(x0 + i * K.block_along / K.points_along); l.pts.push_back(z0 + j * K.block_across); }
		out.push_back(std::move(l));
	}
	for (int i = 0; i <= nx; i++) {
		Line l{ level_of(K, i, nx), {} };
		for (int j = 0; j <= nz * K.points_across; j++) { l.pts.push_back(x0 + i * K.block_along); l.pts.push_back(z0 + j * K.block_across / K.points_across); }
		out.push_back(std::move(l));
	}
	return out;
}

// Walks a line's points; water becomes a short bridge (arterials) or a cut, a steep slope cuts. Each kept run is
// reduced to its two ends (the street is straight; its nodes are the crossings).
void march(const GenShapes &S, const Line &line, int profile, double half, TerrainGen &t, std::vector<road_gen::Piece> &out) {
	const RoadsShape &K = S.roads;
	bool bridge = line.level < LOCAL;
	double max_grade = line.level < LOCAL ? K.max_art_grade : K.max_local_grade;
	std::vector<double> cur;
	bool has_last = false;
	double lx = 0, lz = 0, wet = 0;
	auto flush = [&]() {
		if (cur.size() >= 4) out.push_back({ cur[0], cur[1], cur[cur.size() - 2], cur[cur.size() - 1], profile });
		cur.clear();
	};
	size_t n = line.pts.size() / 2;
	for (size_t i = 0; i < n; i++) {
		double x = line.pts[i * 2], z = line.pts[i * 2 + 1];
		if (std::max(std::fabs(x), std::fabs(z)) >= half - K.inside_margin) { flush(); has_last = false; continue; }
		if (!road_gen::land(S, t, x, z)) {
			if (!bridge || !has_last) { flush(); has_last = false; continue; }
			double px = i > 0 ? line.pts[(i - 1) * 2] : x, pz = i > 0 ? line.pts[(i - 1) * 2 + 1] : z;
			wet += std::hypot(x - px, z - pz);
			if (wet > K.max_wet) { flush(); has_last = false; wet = 0; }
			continue;
		}
		if (has_last && wet == 0) {
			double g = std::fabs(t.base_height(x, z) - t.base_height(lx, lz)) / std::max(1.0, std::hypot(x - lx, z - lz));
			if (g > max_grade) flush();
		}
		wet = 0;
		cur.push_back(x);
		cur.push_back(z);
		has_last = true;
		lx = x; lz = z;
	}
	flush();
}
} // namespace

bool road_gen::land(const GenShapes &S, TerrainGen &t, double x, double z) {
	const RoadsShape &K = S.roads;
	double h = t.base_height(x, z);
	return h > TerrainGen::OCEAN_Y + K.land_clear && t.water_level_at(x, z) < h - K.land_clear;
}

GeneratedRoads generate_roads(uint32_t seed, double map_half, double radius, TerrainGen &terrain, const GenProfiles &prof, const CityPlanDef &plan,
	const GenZones &zones, const GenShapes &S) {
	const RoadsShape &K = S.roads;
	double R = radius > 0 ? radius : map_half - K.edge_pad;
	std::vector<road_gen::Piece> pieces;
	for (const Line &l : block_lines(K, R)) {
		int profile = l.level == AVENUE ? prof.avenue : l.level == COLLECTOR ? prof.collector : prof.local;
		march(S, l, profile, map_half, terrain, pieces);
	}
	const CityPlan city = plan_city(seed & ~TerrainGen::FLAT_BIT, R, plan, zones);
	GeneratedRoads g = road_gen::build_graph(S, pieces, R * K.centre_share, terrain, prof.asph_right, PlanRules(K, seed, prof, city));
	return road_gen::without_spurs(road_gen::dry_crossings(S, g, terrain));
}

GeneratedRoads road_gen::dry_crossings(const GenShapes &S, const GeneratedRoads &r, TerrainGen &terrain) {
	int nn = r.node_count(), ns = r.seg_count();
	std::vector<int> deg(nn, 0);
	for (int s = 0; s < ns; s++) { deg[r.segs[s * SEG_FIELDS]]++; deg[r.segs[s * SEG_FIELDS + 1]]++; }
	auto wet = [&](int n) { return deg[n] > 2 && !land(S, terrain, r.nodes[n * 3], r.nodes[n * 3 + 1]); };
	GeneratedRoads out;
	out.nodes = r.nodes;
	out.centre_r = r.centre_r;
	for (int s = 0; s < ns; s++) {
		const int *seg = &r.segs[s * SEG_FIELDS];
		if (wet(seg[0]) || wet(seg[1])) continue;
		out.segs.insert(out.segs.end(), seg, seg + SEG_FIELDS);
	}
	return out;
}

GeneratedRoads road_gen::without_spurs(const GeneratedRoads &r) {
	int nn = r.node_count(), ns = r.seg_count();
	std::vector<int> deg(nn, 0);
	std::vector<uint8_t> gone(ns, 0);
	std::vector<std::vector<int>> at(nn);
	for (int s = 0; s < ns; s++) {
		int a = r.segs[s * SEG_FIELDS], b = r.segs[s * SEG_FIELDS + 1];
		deg[a]++; deg[b]++;
		at[a].push_back(s); at[b].push_back(s);
	}
	std::vector<int> ends;
	for (int n = 0; n < nn; n++)
		if (deg[n] == 1) ends.push_back(n);
	while (!ends.empty()) {
		int n = ends.back();
		ends.pop_back();
		if (deg[n] != 1) continue;
		int s = *std::find_if(at[n].begin(), at[n].end(), [&](int k) { return !gone[k]; });
		gone[s] = 1;
		int a = r.segs[s * SEG_FIELDS], b = r.segs[s * SEG_FIELDS + 1], o = a == n ? b : a;
		deg[n]--;
		deg[o]--;
		if (deg[o] == 1) ends.push_back(o);
	}
	std::vector<int> map(nn, -1);
	GeneratedRoads out;
	out.centre_r = r.centre_r;
	for (int n = 0; n < nn; n++) {
		if (deg[n] <= 0) continue;
		map[n] = out.node_count();
		out.nodes.insert(out.nodes.end(), { r.nodes[n * 3], r.nodes[n * 3 + 1], r.nodes[n * 3 + 2] });
	}
	for (int s = 0; s < ns; s++) {
		if (gone[s]) continue;
		const int *seg = &r.segs[s * SEG_FIELDS];
		out.segs.insert(out.segs.end(), { map[seg[0]], map[seg[1]], seg[2], seg[3], seg[4] });
	}
	return out;
}

} // namespace city
