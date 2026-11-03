// Grid lines and their march over the land (cityGen.ts generateCity, cityField.ts blockLines); the cut ends taken
// back (dryCrossings, withoutSpurs).
#include "sim/citygen/road_gen.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double BLOCK_ALONG = 120, BLOCK_ACROSS = 72, GRID_SPAN = 1.7, CENTRE_SHARE = 0.85;
constexpr int AVENUE_EVERY = 4, POINTS_ALONG = 4, POINTS_ACROSS = 2;
constexpr double MEDIUM_R = 1100, EDGE_PAD = 60, INSIDE_MARGIN = 380, LAND_CLEAR = 1.2, MAX_WET = 320;
constexpr double MAX_LOCAL_GRADE = 0.11, MAX_ART_GRADE = 0.08;
enum Level { AVENUE = 0, COLLECTOR = 1, LOCAL = 2 };

int level_of(int k, int n) {
	if (k == 0 || k == n) return AVENUE;
	if (n < AVENUE_EVERY * 2) return k == n / 2 ? COLLECTOR : LOCAL;
	if (k % AVENUE_EVERY == 0) return AVENUE;
	return k % (AVENUE_EVERY / 2) == 0 ? COLLECTOR : LOCAL;
}

struct Line {
	int level;
	std::vector<double> pts; // x, z pairs
};

std::vector<Line> block_lines(double R) {
	int nx = std::max(2, int(std::floor(R * GRID_SPAN / BLOCK_ALONG))), nz = std::max(2, int(std::floor(R * GRID_SPAN / BLOCK_ACROSS)));
	double x0 = -(nx * BLOCK_ALONG) / 2, z0 = -(nz * BLOCK_ACROSS) / 2;
	std::vector<Line> out;
	for (int j = 0; j <= nz; j++) {
		Line l{ level_of(j, nz), {} };
		for (int i = 0; i <= nx * POINTS_ALONG; i++) { l.pts.push_back(x0 + i * BLOCK_ALONG / POINTS_ALONG); l.pts.push_back(z0 + j * BLOCK_ACROSS); }
		out.push_back(std::move(l));
	}
	for (int i = 0; i <= nx; i++) {
		Line l{ level_of(i, nx), {} };
		for (int j = 0; j <= nz * POINTS_ACROSS; j++) { l.pts.push_back(x0 + i * BLOCK_ALONG); l.pts.push_back(z0 + j * BLOCK_ACROSS / POINTS_ACROSS); }
		out.push_back(std::move(l));
	}
	return out;
}

// Walks a line's points; water becomes a short bridge (arterials) or a cut, a steep slope cuts. Each kept run is
// reduced to its two ends (the street is straight; its nodes are the crossings).
void march(const Line &line, int profile, double half, TerrainGen &t, std::vector<road_gen::Piece> &out) {
	bool bridge = line.level < LOCAL;
	double max_grade = line.level < LOCAL ? MAX_ART_GRADE : MAX_LOCAL_GRADE;
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
		if (std::max(std::fabs(x), std::fabs(z)) >= half - INSIDE_MARGIN) { flush(); has_last = false; continue; }
		if (!road_gen::land(t, x, z)) {
			if (!bridge || !has_last) { flush(); has_last = false; continue; }
			double px = i > 0 ? line.pts[(i - 1) * 2] : x, pz = i > 0 ? line.pts[(i - 1) * 2 + 1] : z;
			wet += std::hypot(x - px, z - pz);
			if (wet > MAX_WET) { flush(); has_last = false; wet = 0; }
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

bool road_gen::land(TerrainGen &t, double x, double z) {
	double h = t.base_height(x, z);
	return h > TerrainGen::OCEAN_Y + LAND_CLEAR && t.water_level_at(x, z) < h - LAND_CLEAR;
}

GeneratedRoads generate_roads(uint32_t seed, double map_half, bool medium, TerrainGen &terrain, const GenProfiles &prof) {
	double R = medium ? MEDIUM_R : map_half - EDGE_PAD;
	std::vector<road_gen::Piece> pieces;
	for (const Line &l : block_lines(R)) {
		int profile = l.level == AVENUE ? prof.avenue : l.level == COLLECTOR ? prof.collector : prof.local;
		march(l, profile, map_half, terrain, pieces);
	}
	GeneratedRoads g = road_gen::build_graph(pieces, R * CENTRE_SHARE, seed, terrain, prof);
	return road_gen::without_spurs(road_gen::dry_crossings(g, terrain));
}

GeneratedRoads road_gen::dry_crossings(const GeneratedRoads &r, TerrainGen &terrain) {
	int nn = r.node_count(), ns = r.seg_count();
	std::vector<int> deg(nn, 0);
	for (int s = 0; s < ns; s++) { deg[r.segs[s * SEG_FIELDS]]++; deg[r.segs[s * SEG_FIELDS + 1]]++; }
	auto wet = [&](int n) { return deg[n] > 2 && !land(terrain, r.nodes[n * 3], r.nodes[n * 3 + 1]); };
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
