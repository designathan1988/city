// The planar graph of the generated streets (cityGen.ts buildGraph): crossings by 96 m buckets, nodes merged within
// 7 m, short edges collapsed, at most 6 links per node, small pieces dropped, zones by the distance to the centre,
// profile variety by district noise, long edges split into <= 100 m segments, then tidied.
#include "sim/citygen/road_gen.h"

#include "core/seeded.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace city {

namespace {
constexpr double BUCKET = 96, MERGE = 7, COLLAPSE = 16, PARALLEL_COS = 0.93, HUG_DIST = 26, GEN_SEG_LEN = 100;
constexpr int64_t KEY_OFF = 2000, KEY_ROW = 4096, PAIR = 1000000;
constexpr int MAX_LINKS = 6, MIN_COMPONENT = 30, RANK_EXPRESSWAY = 3, RANK_AVENUE_BIKE = 2;
constexpr double NO_ROOM = 1e300;
constexpr int GROUND_PROBES = 5;
constexpr double NODE_PROBE = 8, NODE_MIN_Y = 1.5, CROSS_EPS = 1e-6, PARALLEL_DEN = 1e-9;
constexpr double D_COM_HIGH = 0.08, D_COM = 0.16, D_COM_AV = 0.35, D_MIX = 0.32, D_MIX_AV = 0.7;
constexpr double D_CITY = 1.05, D_LOW = 0.75, IND_NEAR = 800, IND_NOISE = 0.05, OUTSKIRT_RES = 0.5;
constexpr uint32_t ZONE_RNG_MUL = 31, ZONE_RNG_ADD = 7, NOISE_N = 313, NOISE_M = 919, NOISE_IND = 77;
constexpr double NOISE_N_SCALE = 650, NOISE_M_SCALE = 1400, NOISE_IND_SCALE = 900;
constexpr double WIDE_N = 0.45, URBAN_N = 0.1, BIKE_N = 0.25, COBBLE_D = 0.9, COBBLE_M = 0.2, URBAN_LOCAL_N = 0.3;
constexpr double BUS_OFFSET = 0.5, BUS_M = 0.15, BOULEVARD_M = -0.3, AV4_N = 0.35;

int64_t key(int64_t i, int64_t j) { return (i + KEY_OFF) * KEY_ROW + (j + KEY_OFF); }

// Insertion-ordered map (JavaScript Map semantics): the order decides node and segment numbering.
struct EdgeMap {
	std::vector<std::pair<int64_t, int>> items;
	std::unordered_map<int64_t, size_t> at;
	void set(int64_t k, int v) {
		auto it = at.find(k);
		if (it == at.end()) { at[k] = items.size(); items.push_back({ k, v }); }
		else items[it->second].second = v;
	}
	const int *get(int64_t k) const { auto it = at.find(k); return it == at.end() ? nullptr : &items[it->second].second; }
};

class GraphBuilder {
public:
	GraphBuilder(const std::vector<road_gen::Piece> &p, uint32_t seed, TerrainGen &t, const GenProfiles &prof) :
			pieces_(p), seed_(seed), terrain_(t), prof_(prof) {}

	GeneratedRoads build(double centre_r) {
		bucket_pieces();
		mark_hugging_locals();
		find_cuts();
		make_edges();
		collapse_short();
		cap_links();
		drop_small_components();
		node_heights();
		zone_and_split(centre_r);
		road_gen::tidy_graph(nx_, nz_, ny_, segs_, strength_, terrain_, prof_);
		return compact(centre_r);
	}

private:
	const std::vector<road_gen::Piece> &pieces_;
	uint32_t seed_;
	TerrainGen &terrain_;
	const GenProfiles &prof_;
	std::unordered_map<int64_t, std::vector<int>> buckets_;
	std::vector<int64_t> bucket_order_;
	std::vector<uint8_t> dead_;
	std::vector<std::vector<double>> cuts_;
	std::vector<double> nx_, nz_, ny_;
	std::unordered_map<int64_t, std::vector<int>> grid_;
	EdgeMap edges_;
	std::vector<int> root_, segs_, strength_;

	int rank(int p) const { return p == prof_.expressway ? RANK_EXPRESSWAY : p == prof_.avenue_bike ? RANK_AVENUE_BIKE : p == prof_.avenue ? 1 : 0; }

	void bucket_pieces() {
		for (size_t s = 0; s < pieces_.size(); s++) {
			const road_gen::Piece &p = pieces_[s];
			int i0 = int(std::floor(std::min(p.x0, p.x1) / BUCKET)), i1 = int(std::floor(std::max(p.x0, p.x1) / BUCKET));
			int j0 = int(std::floor(std::min(p.z0, p.z1) / BUCKET)), j1 = int(std::floor(std::max(p.z0, p.z1) / BUCKET));
			for (int j = j0; j <= j1; j++)
				for (int i = i0; i <= i1; i++) {
					auto &l = buckets_[key(i, j)];
					if (l.empty()) bucket_order_.push_back(key(i, j));
					l.push_back(int(s));
				}
		}
	}

	// A local street hugging and nearly parallel to an arterial goes (no overlapping roads).
	void mark_hugging_locals() {
		dead_.assign(pieces_.size(), 0);
		for (int64_t bk : bucket_order_) {
			const auto &list = buckets_[bk];
			for (int s : list) {
				const road_gen::Piece &a = pieces_[s];
				if (a.profile != prof_.local || dead_[s]) continue;
				double mx = (a.x0 + a.x1) / 2, mz = (a.z0 + a.z1) / 2, ls = std::hypot(a.x1 - a.x0, a.z1 - a.z0);
				if (ls == 0) ls = 1;
				for (int t : list) {
					const road_gen::Piece &b = pieces_[t];
					if (b.profile == prof_.local) continue;
					double dx = b.x1 - b.x0, dz = b.z1 - b.z0, lt = std::hypot(dx, dz);
					if (lt == 0) lt = 1;
					if (std::fabs(((a.x1 - a.x0) * dx + (a.z1 - a.z0) * dz) / (ls * lt)) < PARALLEL_COS) continue;
					double u = std::max(0.0, std::min(1.0, ((mx - b.x0) * dx + (mz - b.z0) * dz) / (lt * lt)));
					if (std::hypot(mx - (b.x0 + dx * u), mz - (b.z0 + dz * u)) < HUG_DIST) { dead_[s] = 1; break; }
				}
			}
		}
	}

	void find_cuts() {
		size_t n = pieces_.size();
		cuts_.assign(n, {});
		for (size_t s = 0; s < n; s++)
			if (!dead_[s]) cuts_[s] = { 0, 1 };
		std::unordered_map<int64_t, bool> seen;
		for (int64_t bk : bucket_order_) {
			const auto &list = buckets_[bk];
			for (size_t a = 0; a < list.size(); a++)
				for (size_t b = a + 1; b < list.size(); b++) {
					int s = list[a], t = list[b];
					if (dead_[s] || dead_[t]) continue;
					int64_t pk = s < t ? int64_t(s) * int64_t(n) + t : int64_t(t) * int64_t(n) + s;
					if (!seen.emplace(pk, true).second) continue;
					const road_gen::Piece &P = pieces_[s], &Q = pieces_[t];
					double dx1 = P.x1 - P.x0, dz1 = P.z1 - P.z0, dx2 = Q.x1 - Q.x0, dz2 = Q.z1 - Q.z0;
					double den = dx1 * dz2 - dz1 * dx2;
					if (std::fabs(den) < PARALLEL_DEN) continue;
					double wx = Q.x0 - P.x0, wz = Q.z0 - P.z0;
					double u = (wx * dz2 - wz * dx2) / den, v = (wx * dz1 - wz * dx1) / den;
					if (u > CROSS_EPS && u < 1 - CROSS_EPS && v > CROSS_EPS && v < 1 - CROSS_EPS) { cuts_[s].push_back(u); cuts_[t].push_back(v); }
				}
		}
	}

	int node_at(double x, double z) {
		int i = int(std::floor(x / MERGE)), j = int(std::floor(z / MERGE));
		for (int dj = -1; dj <= 1; dj++)
			for (int di = -1; di <= 1; di++) {
				auto it = grid_.find(key(i + di, j + dj));
				if (it == grid_.end()) continue;
				for (int id : it->second)
					if (std::hypot(nx_[id] - x, nz_[id] - z) < MERGE) return id;
			}
		int id = int(nx_.size());
		nx_.push_back(x);
		nz_.push_back(z);
		grid_[key(i, j)].push_back(id);
		return id;
	}

	void make_edges() {
		for (size_t s = 0; s < pieces_.size(); s++) {
			std::sort(cuts_[s].begin(), cuts_[s].end());
			const road_gen::Piece &p = pieces_[s];
			int prev = -1;
			for (double u : cuts_[s]) {
				int id = node_at(p.x0 + (p.x1 - p.x0) * u, p.z0 + (p.z1 - p.z0) * u);
				if (prev >= 0 && prev != id) {
					int64_t k = prev < id ? int64_t(prev) * PAIR + id : int64_t(id) * PAIR + prev;
					const int *old = edges_.get(k);
					if (!old || rank(p.profile) > rank(*old)) edges_.set(k, p.profile);
				}
				prev = id;
			}
		}
	}

	int find(int i) {
		while (root_[i] != i) { root_[i] = root_[root_[i]]; i = root_[i]; }
		return i;
	}

	// Segments shorter than COLLAPSE become a single node (union by root).
	void collapse_short() {
		root_.resize(nx_.size());
		for (size_t i = 0; i < root_.size(); i++) root_[i] = int(i);
		for (const auto &e : edges_.items) {
			int a = int(e.first / PAIR), b = int(e.first % PAIR);
			if (std::hypot(nx_[a] - nx_[b], nz_[a] - nz_[b]) >= COLLAPSE) continue;
			int ra = find(a), rb = find(b);
			if (ra != rb) root_[rb] = ra;
		}
		EdgeMap merged;
		for (const auto &e : edges_.items) {
			int a = find(int(e.first / PAIR)), b = find(int(e.first % PAIR));
			if (a == b) continue;
			int64_t kk = a < b ? int64_t(a) * PAIR + b : int64_t(b) * PAIR + a;
			const int *old = merged.get(kk);
			if (!old || rank(e.second) > rank(*old)) merged.set(kk, e.second);
		}
		edges_ = std::move(merged);
	}

	void cap_links() {
		std::vector<std::pair<int64_t, int>> sorted = edges_.items;
		std::stable_sort(sorted.begin(), sorted.end(), [&](const auto &x, const auto &y) { return rank(y.second) < rank(x.second); });
		std::vector<int> deg(nx_.size(), 0);
		EdgeMap kept;
		for (const auto &e : sorted) {
			int a = int(e.first / PAIR), b = int(e.first % PAIR);
			if (deg[a] >= MAX_LINKS || deg[b] >= MAX_LINKS) continue;
			deg[a]++;
			deg[b]++;
			kept.set(e.first, e.second);
		}
		edges_ = std::move(kept);
	}

	void drop_small_components() {
		std::unordered_map<int, std::vector<int>> adj;
		for (const auto &e : edges_.items) {
			int a = int(e.first / PAIR), b = int(e.first % PAIR);
			adj[a].push_back(b);
			adj[b].push_back(a);
		}
		std::unordered_map<int, int> comp;
		std::vector<int> size;
		for (const auto &kv : adj) {
			if (comp.count(kv.first)) continue;
			int id = int(size.size()), count = 0;
			std::vector<int> stack{ kv.first };
			comp[kv.first] = id;
			while (!stack.empty()) {
				int x = stack.back();
				stack.pop_back();
				count++;
				for (int y : adj[x])
					if (comp.emplace(y, id).second) stack.push_back(y);
			}
			size.push_back(count);
		}
		EdgeMap kept;
		for (const auto &e : edges_.items)
			if (size[comp[int(e.first / PAIR)]] >= MIN_COMPONENT) kept.set(e.first, e.second);
		edges_ = std::move(kept);
	}

	void node_heights() {
		static constexpr double PROBES[GROUND_PROBES][2] = { { 0, 0 }, { NODE_PROBE, 0 }, { -NODE_PROBE, 0 }, { 0, NODE_PROBE }, { 0, -NODE_PROBE } };
		ny_.resize(nx_.size());
		for (size_t i = 0; i < nx_.size(); i++) {
			double h = 0;
			for (const auto &d : PROBES) h += terrain_.base_height(nx_[i] + d[0], nz_[i] + d[1]);
			ny_[i] = std::max(TerrainGen::OCEAN_Y + NODE_MIN_Y, h / GROUND_PROBES);
		}
	}

	std::vector<int> exp_nodes_;

	double near_expressway(double x, double z) const {
		double best = NO_ROOM;
		for (int id : exp_nodes_) best = std::min(best, std::fabs(nx_[id] - x) + std::fabs(nz_[id] - z));
		return best;
	}

	int zone_of(int profile, double d, double mx, double mz, Mulberry32 &zr) const {
		if (profile == prof_.expressway) return ZONE_NONE;
		bool avenue = profile != prof_.local;
		if (d < D_COM_HIGH && avenue) return ZONE_COM_HIGH;
		if (d < D_COM || (avenue && d < D_COM_AV)) return ZONE_COM;
		if (d < D_MIX || (avenue && d < D_MIX_AV)) return ZONE_MIXED;
		if (d < D_CITY) {
			bool ind = near_expressway(mx, mz) < IND_NEAR && fbm2(mx / NOISE_IND_SCALE, mz / NOISE_IND_SCALE, seed_ + NOISE_IND, 2) > IND_NOISE;
			return ind ? ZONE_IND : d > D_LOW ? ZONE_RES_LOW : ZONE_RES;
		}
		return zr.next() < OUTSKIRT_RES ? ZONE_RES : ZONE_NONE;
	}

	int variant(int p, int zone, double d, double x, double z) const {
		double n = fbm2(x / NOISE_N_SCALE, z / NOISE_N_SCALE, seed_ + NOISE_N, 2), m = fbm2(x / NOISE_M_SCALE, z / NOISE_M_SCALE, seed_ + NOISE_M, 2);
		if (p == prof_.local) {
			if (zone == ZONE_COM) return n > WIDE_N ? prof_.wide : n > URBAN_N ? prof_.urban : prof_.local;
			if (zone == ZONE_IND) return prof_.concrete;
			if (zone == ZONE_MIXED) return n > BIKE_N ? prof_.bike : n < -BIKE_N ? prof_.green : prof_.local;
			if (d > COBBLE_D && m > COBBLE_M) return prof_.cobble;
			return n > URBAN_LOCAL_N ? prof_.urban : p;
		}
		if (p == prof_.avenue) {
			if (d < BUS_OFFSET && m > BUS_M) return prof_.bus_side;
			if (m < BOULEVARD_M) return prof_.boulevard;
			if (n > AV4_N) return prof_.av4;
		}
		return p;
	}

	void zone_and_split(double centre_r) {
		Mulberry32 zr(seed_ * ZONE_RNG_MUL + ZONE_RNG_ADD);
		for (const auto &e : edges_.items)
			if (e.second == prof_.expressway) exp_nodes_.push_back(int(e.first / PAIR));
		for (const auto &e : edges_.items) {
			int a = int(e.first / PAIR), b = int(e.first % PAIR), profile = e.second;
			double mx = (nx_[a] + nx_[b]) / 2, mz = (nz_[a] + nz_[b]) / 2;
			double d = std::hypot(mx, mz) / centre_r;
			int zone = zone_of(profile, d, mx, mz, zr);
			double L = std::hypot(nx_[b] - nx_[a], nz_[b] - nz_[a]);
			int count = int(std::ceil(L / GEN_SEG_LEN)), pv = variant(profile, zone, d, mx, mz);
			int from = a;
			for (int k = 1; k <= count; k++) {
				int to = b;
				if (k < count) {
					double t = double(k) / count;
					to = int(nx_.size());
					nx_.push_back(nx_[a] + (nx_[b] - nx_[a]) * t);
					nz_.push_back(nz_[a] + (nz_[b] - nz_[a]) * t);
					ny_.push_back(ny_[a] + (ny_[b] - ny_[a]) * t);
				}
				segs_.insert(segs_.end(), { from, to, pv, zone, zone });
				strength_.push_back(rank(profile));
				from = to;
			}
		}
	}

	GeneratedRoads compact(double centre_r) {
		GeneratedRoads out;
		out.centre_r = centre_r;
		std::vector<int> remap(nx_.size(), -1);
		for (size_t i = 0; i < segs_.size(); i += SEG_FIELDS)
			for (int k = 0; k < 2; k++) {
				int id = segs_[i + k];
				if (remap[id] < 0) {
					remap[id] = out.node_count();
					out.nodes.insert(out.nodes.end(), { nx_[id], nz_[id], ny_[id] });
				}
				segs_[i + k] = remap[id];
			}
		out.segs = std::move(segs_);
		return out;
	}
};
} // namespace

GeneratedRoads road_gen::build_graph(const std::vector<Piece> &pieces, double centre_r, uint32_t seed, TerrainGen &terrain, const GenProfiles &prof) {
	return GraphBuilder(pieces, seed, terrain, prof).build(centre_r);
}

} // namespace city
