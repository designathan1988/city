// The blocks of a 3D model (survey_blocks.h): its faces that look up, grouped into tops that merge, each top a block from the
// ground with its hull.
#include "sim/commands/survey_blocks.h"
#include "core/plane_geometry.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace city {

using namespace b40;

namespace {
constexpr float LEAST_AREA = 2e-4f; // a triangle smaller than this (m2) is noise of the model
// The hull is rounded to centimetres (HULL_STEP a metre) with HULL_EPS of slack.
constexpr float HULL_STEP = 100.f, HULL_EPS = 1e-6f;

struct Top {
	float x0, x1, y0, y1, z;
	SurveyHull pts;
};

bool meets(const SurveyShape &K, const Top &a, const Top &b) {
	return std::fabs(a.z - b.z) < K.touch_m && b.x0 <= a.x1 + K.touch_m && b.x1 >= a.x0 - K.touch_m && b.y0 <= a.y1 + K.touch_m && b.y1 >= a.y0 - K.touch_m;
}

void absorb(Top &a, const Top &b) {
	a.x0 = std::min(a.x0, b.x0);
	a.x1 = std::max(a.x1, b.x1);
	a.y0 = std::min(a.y0, b.y0);
	a.y1 = std::max(a.y1, b.y1);
	a.pts.insert(a.pts.end(), b.pts.begin(), b.pts.end());
}

SurveyHull convex_hull(SurveyHull p) {
	for (auto &q : p) q = { std::round(q[0] * HULL_STEP) / HULL_STEP, std::round(q[1] * HULL_STEP) / HULL_STEP };
	std::sort(p.begin(), p.end());
	p.erase(std::unique(p.begin(), p.end()), p.end());
	auto cross = [](const std::array<float, 2> &o, const std::array<float, 2> &a, const std::array<float, 2> &b) { return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]); };
	return monotone_hull(p, cross, HULL_EPS);
}

// The tops of the model filed by a uniform grid over its plan: each top in every cell its box, grown by TOUCH_M, covers, so
// what can meet a box is what the cells under it hold. A top's box only grows (it absorbs others), so it is filed again in the
// cells it reaches anew; one absorbed stays in its cells, dead. The grid's cell is a share of the model's extent.
class TopGrid {
public:
	TopGrid(const SurveyShape &k, float x0, float y0, float x1, float y1) : K(k), x0_(x0), y0_(y0) {
		constexpr float GRID_CELLS = 256.f; // the model's extent in this many cells, each at least least_cell_m
		cell_ = std::max({ (x1 - x0) / GRID_CELLS, (y1 - y0) / GRID_CELLS, K.least_cell_m });
	}
	// Files top `id`, whose box was `was` (nothing when new), in the cells its box `now` reaches anew.
	void file(int id, const Top *was, const Top &now) {
		int i0, j0, i1, j1, oi0 = 0, oj0 = 0, oi1 = -1, oj1 = -1;
		span(now.x0 - K.touch_m, now.y0 - K.touch_m, now.x1 + K.touch_m, now.y1 + K.touch_m, i0, j0, i1, j1);
		if (was) span(was->x0 - K.touch_m, was->y0 - K.touch_m, was->x1 + K.touch_m, was->y1 + K.touch_m, oi0, oj0, oi1, oj1);
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++)
				if (i < oi0 || i > oi1 || j < oj0 || j > oj1) cells_[key(i, j)].push_back(id);
	}
	// fn(id) once for every top filed in a cell the box covers (alive or not: the caller tells).
	template <typename F>
	void near(float x0, float y0, float x1, float y1, F &&fn) {
		int i0, j0, i1, j1;
		span(x0, y0, x1, y1, i0, j0, i1, j1);
		stamp_++;
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++) {
				const auto it = cells_.find(key(i, j));
				if (it == cells_.end()) continue;
				for (int id : it->second) {
					if (size_t(id) >= seen_.size()) seen_.resize(size_t(id) + 1, 0);
					if (seen_[size_t(id)] == stamp_) continue;
					seen_[size_t(id)] = stamp_;
					fn(id);
				}
			}
	}

private:
	const SurveyShape &K; // the survey's sizes (buildings.json shapes.survey)
	static constexpr int ROW_BITS = 32; // a cell's key: its column over the row's 32 bits
	static int64_t key(int i, int j) { return (int64_t(i) << ROW_BITS) ^ int64_t(uint32_t(j)); }
	void span(float x0, float y0, float x1, float y1, int &i0, int &j0, int &i1, int &j1) const {
		i0 = int(std::floor((x0 - x0_) / cell_));
		j0 = int(std::floor((y0 - y0_) / cell_));
		i1 = int(std::floor((x1 - x0_) / cell_));
		j1 = int(std::floor((y1 - y0_) / cell_));
	}
	float x0_, y0_, cell_ = 1;
	std::unordered_map<int64_t, std::vector<int>> cells_;
	std::vector<uint32_t> seen_;
	uint32_t stamp_ = 0;
};
} // namespace

// The model's blocks, from the faces that look up. Each face joins the first-made top it meets (a top keeps the height of the
// face that made it), then tops that meet merge, always the first pair in the order they were made, the earlier taking the
// later, until none meet. The grid finds what a box can meet; the order of the merges is the one the pairs give.
std::vector<SurveyBlock> survey_blocks(const SurveyShape &K, const std::vector<SurveyTriangle> &tris) {
	std::vector<Top> tops;
	float mx0 = tris.empty() ? 0 : tris[0][0].x, my0 = tris.empty() ? 0 : tris[0][0].y, mx1 = mx0, my1 = my0;
	for (const SurveyTriangle &t : tris)
		for (const SurveyPoint &p : t) {
			mx0 = std::min(mx0, p.x);
			my0 = std::min(my0, p.y);
			mx1 = std::max(mx1, p.x);
			my1 = std::max(my1, p.y);
		}
	TopGrid grid(K, mx0, my0, mx1, my1);
	std::vector<char> alive;
	for (const SurveyTriangle &t : tris) {
		const SurveyPoint &a = t[0], &b = t[1], &c = t[2];
		const float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z, vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
		const float nx = uy * vz - uz * vy, ny = uz * vx - ux * vz, nz = ux * vy - uy * vx, length = std::sqrt(nx * nx + ny * ny + nz * nz);
		if (length < LEAST_AREA || std::fabs(nz) / length < K.flat_normal) continue; // either winding: models are not always consistent
		Top top{ std::min({ a.x, b.x, c.x }), std::max({ a.x, b.x, c.x }), std::min({ a.y, b.y, c.y }), std::max({ a.y, b.y, c.y }), (a.z + b.z + c.z) / 3, { { a.x, a.y }, { b.x, b.y }, { c.x, c.y } } };
		int first = -1;
		grid.near(top.x0, top.y0, top.x1, top.y1, [&](int id) {
			if ((first < 0 || id < first) && meets(K, tops[size_t(id)], top)) first = id;
		});
		if (first >= 0) {
			const Top was = { tops[size_t(first)].x0, tops[size_t(first)].x1, tops[size_t(first)].y0, tops[size_t(first)].y1, 0, {} };
			absorb(tops[size_t(first)], top);
			grid.file(first, &was, tops[size_t(first)]);
		} else {
			tops.push_back(std::move(top));
			alive.push_back(1);
			grid.file(int(tops.size()) - 1, nullptr, tops.back());
		}
	}
	// a top grown by later triangles can now touch another: the first meeting pair merges, the earlier top taking the later
	const auto first_meeting = [&](int c, bool below) {
		int found = -1;
		const Top &t = tops[size_t(c)];
		grid.near(t.x0, t.y0, t.x1, t.y1, [&](int id) {
			if (!alive[size_t(id)] || id == c || (below ? id > c : id < c) || (found >= 0 && id > found)) return;
			if (meets(K, tops[size_t(c)], tops[size_t(id)])) found = id;
		});
		return found;
	};
	const auto merge = [&](int into, int from) {
		const Top was = { tops[size_t(into)].x0, tops[size_t(into)].x1, tops[size_t(into)].y0, tops[size_t(into)].y1, 0, {} };
		absorb(tops[size_t(into)], tops[size_t(from)]);
		alive[size_t(from)] = 0;
		SurveyHull().swap(tops[size_t(from)].pts);
		grid.file(into, &was, tops[size_t(into)]);
	};
	for (int c = 0; c < int(tops.size());) {
		if (!alive[size_t(c)]) {
			c++;
			continue;
		}
		const int lower = first_meeting(c, true); // no pair of earlier tops meets: only one with `c`, which grew, may
		if (lower >= 0) {
			merge(lower, c);
			c = lower;
			continue;
		}
		const int higher = first_meeting(c, false);
		if (higher >= 0) merge(c, higher);
		else c++;
	}
	std::vector<SurveyBlock> out;
	for (size_t ti = 0; ti < tops.size(); ti++) {
		if (!alive[ti]) continue;
		const Top &t = tops[ti];
		if (t.z <= K.least_top_z || t.x1 - t.x0 <= K.least_top_side || t.y1 - t.y0 <= K.least_top_side) continue;
		const float cx = (t.x0 + t.x1) / 2, cy = (t.y0 + t.y1) / 2;
		float foot = 0;
		grid.near(cx, cy, cx, cy, [&](int id) {
			const Top &q = tops[size_t(id)];
			if (alive[size_t(id)] && q.z < t.z - K.touch_m && cx >= q.x0 && cx <= q.x1 && cy >= q.y0 && cy <= q.y1) foot = std::max(foot, q.z);
		});
		SurveyBlock b;
		b.x0 = t.x0;
		b.x1 = t.x1;
		b.y0 = t.y0;
		b.y1 = t.y1;
		b.z0 = foot;
		b.z1 = t.z;
		b.hull = convex_hull(t.pts);
		out.push_back(std::move(b));
	}
	std::sort(out.begin(), out.end(), [](const SurveyBlock &a, const SurveyBlock &b) { return a.z1 < b.z1; });
	return out;
}

} // namespace city
