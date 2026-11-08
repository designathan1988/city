// The streets of a scenic city, see scenic_roads.h.
#include "sim/citygen/scenic_roads.h"

#include "core/units.h"
#include "sim/catalog/catalog.h"
#include "sim/citygen/scenic_rules.h"

#include <cmath>
#include <set>
#include <tuple>

namespace city {

namespace {
constexpr float EPS = 1e-4f, KEY_GRAIN_M = 0.1f; // float rounding; an end's key in steps of this (the same node whatever the rounding)

// A half-plane n.p <= c: a district's cell is the city's square cut by one per other seed (the bisector between the two).
struct HalfPlane {
	V2 n;
	float c;
};

// The cell of district i, grown by `grow` across each bisector (the square's sides stay).
std::vector<HalfPlane> cell_of(const ScenicCity &city, size_t i, float grow) {
	const float h = city.half_m;
	std::vector<HalfPlane> out = { { V2(1, 0), h }, { V2(-1, 0), h }, { V2(0, 1), h }, { V2(0, -1), h } };
	const V2 si = city.districts[i].seed;
	for (size_t j = 0; j < city.districts.size(); j++) {
		const V2 sj = city.districts[j].seed, d = sj - si;
		const float l = d.len();
		if (j == i || l < EPS) continue;
		out.push_back({ d / l, (sj.len2() - si.len2()) / (2 * l) + grow });
	}
	return out;
}

// The part of the segment a-b inside the cell (Cyrus-Beck); false when none is.
bool clip(const std::vector<HalfPlane> &cell, V2 &a, V2 &b) {
	float t0 = 0, t1 = 1;
	const V2 d = b - a;
	for (const HalfPlane &hp : cell) {
		const float den = hp.n.dot(d), num = hp.c - hp.n.dot(a);
		if (std::fabs(den) < EPS) {
			if (num < 0) return false;
			continue;
		}
		const float t = num / den;
		if (den > 0) t1 = std::min(t1, t);
		else t0 = std::max(t0, t);
		if (t0 >= t1) return false;
	}
	const V2 from = a + d * t0;
	b = a + d * t1;
	a = from;
	return true;
}

// The cell as a ring: the square cut by each half-plane in turn (Sutherland-Hodgman).
Polyline cell_ring(const std::vector<HalfPlane> &cell, float h) {
	Polyline ring = { V2(-h, -h), V2(h, -h), V2(h, h), V2(-h, h) };
	for (const HalfPlane &hp : cell) {
		Polyline out;
		for (size_t k = 0; k < ring.size(); k++) {
			const V2 p = ring[k], q = ring[(k + 1) % ring.size()];
			const float dp = hp.n.dot(p) - hp.c, dq = hp.n.dot(q) - hp.c;
			if (dp <= 0) out.push_back(p);
			if ((dp <= 0) != (dq <= 0)) out.push_back(p + (q - p) * (dp / (dp - dq)));
		}
		ring = std::move(out);
		if (ring.empty()) break;
	}
	return ring;
}

class Layout {
public:
	Layout(const GenShapes &s, const ScenicCity &city, const ScenicRules &rules, TerrainGen &land) : S(s), city_(city), rules_(rules), land_(land) {}
	std::vector<road_gen::Piece> pieces;

	// The named roads, each segment of its line walked on its own; a main road bridges water.
	void roads() {
		const auto inside = [&](V2 p) { return std::fabs(p.x) <= city_.half_m && std::fabs(p.y) <= city_.half_m; };
		for (size_t r = 0; r < city_.roads.size(); r++) {
			const Polyline &line = rules_.road_lines()[r];
			const int profile = city_.roads[r].profile;
			for (size_t k = 0; k + 1 < line.size(); k++) walk(line[k], line[k + 1], profile, rules_.rank(profile) > 0, inside);
		}
	}

	// District i's two families of lines, `turn` from the x axis, through its seed; every `collector_every`-th a collector.
	void district(size_t i) {
		const ScenicDistrict &d = city_.districts[i];
		const std::vector<HalfPlane> cell = cell_of(city_, i, city_.streets.seam_overshoot_m);
		float reach = 0;
		for (const V2 &p : cell_ring(cell, city_.half_m)) reach = std::max(reach, (p - d.seed).len());
		const V2 u = v2_from_angle(d.turn_deg * DEG_TO_RAD), v = u.perp();
		lines(i, cell, u, v, d.block_across_m, reach);
		lines(i, cell, v, u, d.block_along_m, reach);
	}

	// The borders between cells, each once; the square's own sides are the city's edge and carry none.
	void seams() {
		std::set<std::tuple<long, long, long, long>> laid;
		const auto grain = [](float v) { return std::lround(v / KEY_GRAIN_M); };
		for (size_t i = 0; i < city_.districts.size(); i++) {
			const Polyline ring = cell_ring(cell_of(city_, i, 0), city_.half_m);
			for (size_t k = 0; k < ring.size(); k++) {
				V2 a = ring[k], b = ring[(k + 1) % ring.size()];
				if (on_edge(a, b)) continue;
				if (std::tie(a.x, a.y) > std::tie(b.x, b.y)) std::swap(a, b);
				if (!laid.insert({ grain(a.x), grain(a.y), grain(b.x), grain(b.y) }).second) continue;
				walk(a, b, city_.streets.seam, false, [&](V2 p) { return street_open(p); });
			}
		}
	}

private:
	const GenShapes &S; // growth.json shapes
	const ScenicCity &city_;
	const ScenicRules &rules_;
	TerrainGen &land_;

	bool on_edge(V2 a, V2 b) const {
		const float h = city_.half_m - EPS;
		return (std::fabs(a.x) >= h && std::fabs(b.x) >= h && a.x * b.x > 0) || (std::fabs(a.y) >= h && std::fabs(b.y) >= h && a.y * b.y > 0);
	}
	bool street_open(V2 p) const { return !rules_.in_park(p) && !rules_.near_highway(p, city_.streets.highway_clear_m); }

	void lines(size_t i, const std::vector<HalfPlane> &cell, V2 along, V2 across, float spacing, float reach) {
		const ScenicDistrict &d = city_.districts[i];
		const int count = int(std::ceil(reach / spacing));
		for (int k = -count; k <= count; k++) {
			V2 a = d.seed + across * (float(k) * spacing) - along * reach, b = a + along * (2 * reach);
			if (!clip(cell, a, b)) continue;
			const int profile = d.collector_every > 0 && k % d.collector_every == 0 ? d.collector : d.local;
			walk(a, b, profile, false, [&](V2 p) { return street_open(p); });
		}
	}

	// Walks a-b in steps and keeps the runs where `open` holds over dry land; with `bridge`, a run carries on over water up to
	// bridge_max_m. Each run is laid as pieces of at most LONGEST_RUN_M (the graph builder files a piece by its bounds).
	template <typename Open>
	void walk(V2 a, V2 b, int profile, bool bridge, Open open) {
		const int n = std::max(1, int(std::ceil((b - a).len() / city_.streets.march_step_m)));
		const float step = (b - a).len() / float(n);
		bool in = false;
		float wet = 0;
		V2 start, last;
		const auto flush = [&]() {
			if (in) lay(start, last, profile);
			in = false;
			wet = 0;
		};
		for (int k = 0; k <= n; k++) {
			const V2 p = a + (b - a) * (float(k) / float(n));
			if (!open(p)) {
				flush();
				continue;
			}
			if (!road_gen::land(S, land_, p.x, p.y)) {
				if (!bridge || !in) flush();
				else if ((wet += step) > city_.streets.bridge_max_m) flush();
				continue;
			}
			wet = 0;
			if (!in) start = p;
			in = true;
			last = p;
		}
		flush();
	}

	void lay(V2 a, V2 b, int profile) {
		const float len = (b - a).len();
		if (len < EPS) return;
		const int parts = std::max(1, int(std::ceil(len / S.scenic.longest_run_m)));
		for (int k = 0; k < parts; k++) {
			const V2 p = a + (b - a) * (float(k) / float(parts)), q = a + (b - a) * (float(k + 1) / float(parts));
			pieces.push_back({ p.x, p.y, q.x, q.y, profile });
		}
	}
};
} // namespace

GeneratedRoads scenic_roads(const ScenicCity &city, const Catalog &catalog, TerrainGen &land) {
	const ScenicRules rules(city, catalog, land);
	Layout layout(catalog.growth.shapes, city, rules, land);
	layout.roads(); // first: their nodes are numbered first, and an edge laid twice keeps the higher rank either way
	for (size_t i = 0; i < city.districts.size(); i++) layout.district(i);
	layout.seams();
	std::vector<double> asph_right;
	for (const Profile &p : catalog.profiles) asph_right.push_back(p.asph_right);
	const GeneratedRoads g = road_gen::build_graph(catalog.growth.shapes, layout.pieces, city.half_m, land, asph_right, rules);
	return road_gen::without_spurs(road_gen::dry_crossings(catalog.growth.shapes, g, land));
}

} // namespace city
