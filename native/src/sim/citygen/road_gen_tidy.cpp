// The generated graph must open clean (cityGen.ts tidyGraph): no node where two roads leave closer than MIN_LEG_ANGLE
// (the weaker leg goes) and no dead end lying inside another road (it joins the road, ends on its node or goes).
#include "core/math.h"
#include "sim/citygen/road_gen.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace city {

namespace {
constexpr double MIN_LEG_ANGLE = 25; // degrees (roads/impossible.ts)
constexpr double END_GAP = 4, MIN_PIECE = 9, CELL = 64, PAD = 12, ON_ROAD_MIN = 1.5, ON_ROAD_DEFAULT = 3, ON_ROAD_DY = 2;
constexpr int64_t CELL_OFF = 4000, CELL_ROW = 8192;
constexpr int MAX_PASSES = 20, MAX_LEGS = 6, KEEPS_LEGS = 3;
constexpr double FULL_TURN = 360, HALF_TURN = 180;

class Tidy {
public:
	Tidy(std::vector<double> &nx, std::vector<double> &nz, std::vector<double> &ny, TerrainGen &t, const GenProfiles &p) :
			nx_(nx), nz_(nz), ny_(ny), terrain_(t), prof_(p) {}

	void run(std::vector<int> &segs, const std::vector<int> &strength) {
		for (size_t i = 0; i < segs.size(); i += SEG_FIELDS)
			add(segs[i], segs[i + 1], segs[i + 2], segs[i + 3], segs[i + 4], strength[i / SEG_FIELDS]);
		bool changed = true;
		for (int pass = 0; changed && pass < MAX_PASSES; pass++) {
			changed = false;
			std::vector<int> nodes = leg_order_;
			for (int n : nodes)
				for (auto p = sharp_pair(n); p.first >= 0; p = sharp_pair(n)) { remove(weaker(n, p.first, p.second)); changed = true; }
			nodes = leg_order_;
			for (int n : nodes) changed = fix_dead_end(n) || changed;
		}
		segs.clear();
		for (size_t s = 0; s < a_.size(); s++)
			if (alive_[s]) segs.insert(segs.end(), { a_[s], b_[s], prof_s_[s], zl_[s], zr_[s] });
	}

private:
	std::vector<double> &nx_, &nz_, &ny_;
	TerrainGen &terrain_;
	const GenProfiles &prof_;
	std::vector<int> a_, b_, prof_s_, zl_, zr_, rk_;
	std::vector<bool> alive_;
	std::unordered_map<int, std::vector<int>> legs_;
	std::vector<int> leg_order_; // nodes in first-seen order (JavaScript Map key order)
	std::unordered_map<int64_t, std::vector<int>> cells_;

	static int64_t ck(int64_t i, int64_t j) { return (i + CELL_OFF) * CELL_ROW + (j + CELL_OFF); }
	std::vector<int> &legs_of(int n) {
		auto it = legs_.find(n);
		if (it != legs_.end()) return it->second;
		leg_order_.push_back(n);
		return legs_[n];
	}
	int other(int s, int n) const { return a_[s] == n ? b_[s] : a_[s]; }
	double heading(int n, int m) const { return std::atan2(nz_[m] - nz_[n], nx_[m] - nx_[n]) * HALF_TURN / PI_D; }
	static double gap_between(double h1, double h2) {
		double d = std::fmod(std::fabs(h1 - h2), FULL_TURN);
		return std::min(d, FULL_TURN - d);
	}
	double seg_len(int s) const { return std::hypot(nx_[a_[s]] - nx_[b_[s]], nz_[a_[s]] - nz_[b_[s]]); }

	int add(int sa, int sb, int p, int l, int r, int k) {
		int s = int(a_.size());
		a_.push_back(sa); b_.push_back(sb); prof_s_.push_back(p); zl_.push_back(l); zr_.push_back(r); rk_.push_back(k); alive_.push_back(true);
		legs_of(sa).push_back(s);
		legs_of(sb).push_back(s);
		int i0 = int(std::floor((std::min(nx_[sa], nx_[sb]) - PAD) / CELL)), i1 = int(std::floor((std::max(nx_[sa], nx_[sb]) + PAD) / CELL));
		int j0 = int(std::floor((std::min(nz_[sa], nz_[sb]) - PAD) / CELL)), j1 = int(std::floor((std::max(nz_[sa], nz_[sb]) + PAD) / CELL));
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++) cells_[ck(i, j)].push_back(s);
		return s;
	}
	void remove(int s) {
		alive_[s] = false;
		for (int n : { a_[s], b_[s] }) {
			auto &l = legs_[n];
			l.erase(std::find(l.begin(), l.end(), s));
		}
	}

	std::pair<int, int> sharp_pair(int n) {
		const auto &l = legs_[n];
		if (l.size() < 2) return { -1, -1 };
		std::vector<std::pair<int, double>> hs;
		for (int s : l) hs.push_back({ s, heading(n, other(s, n)) });
		std::stable_sort(hs.begin(), hs.end(), [](const auto &x, const auto &y) { return x.second < y.second; });
		std::pair<int, int> best{ -1, -1 };
		double gap = MIN_LEG_ANGLE;
		for (size_t i = 0; i < hs.size(); i++) {
			size_t j = (i + 1) % hs.size();
			double g = j == 0 ? hs[0].second + FULL_TURN - hs[i].second : hs[j].second - hs[i].second;
			if (g < gap) { gap = g; best = { hs[i].first, hs[j].first }; }
		}
		return best;
	}

	int weaker(int n, int s, int t) {
		if (rk_[s] != rk_[t]) return rk_[s] < rk_[t] ? s : t;
		bool ks = legs_of(other(s, n)).size() >= KEEPS_LEGS, kt = legs_of(other(t, n)).size() >= KEEPS_LEGS;
		if (ks != kt) return ks ? s : t;
		return seg_len(s) <= seg_len(t) ? s : t;
	}

	// The road a dead end lies inside: segment and position along it, or -1.
	int inside(int n, int own, double &t_out) const {
		double x = nx_[n], z = nz_[n], bd = 1e300;
		int best = -1;
		auto it = cells_.find(ck(int(std::floor(x / CELL)), int(std::floor(z / CELL))));
		if (it == cells_.end()) return -1;
		for (int s : it->second) {
			if (!alive_[s] || s == own || a_[s] == n || b_[s] == n) continue;
			double dx = nx_[b_[s]] - nx_[a_[s]], dz = nz_[b_[s]] - nz_[a_[s]], L = std::hypot(dx, dz);
			if (L < 1e-6) continue;
			double t = std::max(0.0, std::min(1.0, ((x - nx_[a_[s]]) * dx + (z - nz_[a_[s]]) * dz) / (L * L)));
			double d = std::hypot(x - nx_[a_[s]] - dx * t, z - nz_[a_[s]] - dz * t);
			size_t p = size_t(prof_s_[s]);
			double asph = p < prof_.asph_right.size() ? prof_.asph_right[p] : ON_ROAD_DEFAULT;
			if (d > std::max(ON_ROAD_MIN, asph) || d >= bd) continue;
			if (t * L < END_GAP || (1 - t) * L < END_GAP) continue;
			if (std::fabs(ny_[a_[s]] + (ny_[b_[s]] - ny_[a_[s]]) * t - ny_[n]) > ON_ROAD_DY) continue;
			best = s; t_out = t; bd = d;
		}
		return best;
	}

	bool fix_dead_end(int n) {
		auto &l = legs_[n];
		if (l.size() != 1) return false;
		int own = l[0];
		double t = 0;
		int s = inside(n, own, t);
		if (s < 0) return false;
		int far = other(own, n);
		double x = nx_[a_[s]] + (nx_[b_[s]] - nx_[a_[s]]) * t, z = nz_[a_[s]] + (nz_[b_[s]] - nz_[a_[s]]) * t;
		double h = std::atan2(nz_[far] - z, nx_[far] - x) * HALF_TURN / PI_D, hr = heading(a_[s], b_[s]);
		double L = seg_len(s);
		if (t * L < MIN_PIECE || (1 - t) * L < MIN_PIECE) {
			int e = t * L < MIN_PIECE ? a_[s] : b_[s];
			const auto &le = legs_of(e);
			bool linked = std::any_of(le.begin(), le.end(), [&](int k) { return other(k, e) == far; });
			bool free = le.size() < MAX_LEGS && road_gen::land(terrain_, nx_[e], nz_[e]) && !linked;
			remove(own);
			if (free && std::hypot(nx_[far] - nx_[e], nz_[far] - nz_[e]) >= MIN_PIECE)
				add(a_[own] == n ? e : a_[own], a_[own] == n ? b_[own] : e, prof_s_[own], zl_[own], zr_[own], rk_[own]);
			return true;
		}
		if (gap_between(h, hr) < MIN_LEG_ANGLE || gap_between(h, hr + HALF_TURN) < MIN_LEG_ANGLE ||
				std::hypot(nx_[far] - x, nz_[far] - z) < MIN_PIECE || !road_gen::land(terrain_, x, z)) {
			remove(own);
			return true;
		}
		int m = int(nx_.size());
		nx_.push_back(x);
		nz_.push_back(z);
		ny_.push_back(ny_[a_[s]] + (ny_[b_[s]] - ny_[a_[s]]) * t);
		remove(s);
		add(a_[s], m, prof_s_[s], zl_[s], zr_[s], rk_[s]);
		add(m, b_[s], prof_s_[s], zl_[s], zr_[s], rk_[s]);
		remove(own);
		if (a_[own] == n) add(m, b_[own], prof_s_[own], zl_[own], zr_[own], rk_[own]);
		else add(a_[own], m, prof_s_[own], zl_[own], zr_[own], rk_[own]);
		return true;
	}
};
} // namespace

void road_gen::tidy_graph(std::vector<double> &nx, std::vector<double> &nz, std::vector<double> &ny, std::vector<int> &segs,
		const std::vector<int> &strength, TerrainGen &terrain, const GenProfiles &prof) {
	Tidy(nx, nz, ny, terrain, prof).run(segs, strength);
}

} // namespace city
