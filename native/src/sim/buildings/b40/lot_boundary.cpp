// The lot's boundary, made whole by construction: along each edge the run is the whole edge less its gates and less the
// stretches where the building itself stands on the edge (its wall closes them). The front takes one of its kind's
// styles (buildings.json lots.boundary); the sides and back are party walls (a works' fence), low beside a front garden.
// Gates: the people's at the door, a car's on every drive, the car pad's and the works visitors'; with no front zone the
// side strips open on the street through their own gates.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

using Span = std::pair<float, float>;

float thickness(const LotBoundaryShape &K, Element k) { return k == EL_HEDGE ? K.hedge_thick : k == EL_WALL ? K.boundary_wall_t : K.fence_thick; }

// [a, b] less the cuts, the stretches at least SHORTEST_RUN long.
std::vector<Span> runs(const LotBoundaryShape &K, float a, float b, std::vector<Span> cuts) {
	std::sort(cuts.begin(), cuts.end());
	std::vector<Span> out;
	float at = a;
	for (const Span &c : cuts) {
		if (c.first - at >= K.shortest_run) out.push_back({ at, std::min(c.first, b) });
		at = std::max(at, c.second);
	}
	if (b - at >= K.shortest_run) out.push_back({ at, b });
	return out;
}

struct Gate {
	float x0, x1;
	bool car;
};

std::vector<Gate> front_gates(LotSpace &s) {
	const LotBoundaryShape &K = s.data.shapes.boundary;
	const bool house = s.b.kind == KIND_HOUSE;
	const LotRules &l = s.rules;
	std::vector<Gate> g;
	if (s.F > 0) {
		float pg = l.person_gate[house ? 0 : 1] * HALF;
		g.push_back({ s.door_x - pg, s.door_x + pg, false });
		for (const Strip &st : s.strips)
			if (!st.drive.empty()) g.push_back({ st.drive.x0 + K.gate_post, st.drive.x1 - K.gate_post, true });
		Rect pad = car_pad(s);
		if (!pad.empty()) g.push_back({ pad.x0, pad.x1, true });
		if (s.plan.front == FRONT_APRON && s.F >= l.row_depth(1)) {
			// The visitors' gate onto the apron, as wide as its aisle, on the side away from the door.
			float x = s.door_x > 0 ? s.L + l.aisle * HALF + K.gate_post : s.R - l.aisle * HALF - K.gate_post;
			Gate v{ x - l.aisle * HALF, x + l.aisle * HALF, true };
			bool free = true;
			for (const Gate &o : g) free = free && (v.x1 < o.x0 || o.x1 < v.x0);
			if (free) g.push_back(v);
		}
		return g;
	}
	for (const Strip &st : s.strips) {
		if (!st.drive.empty()) g.push_back({ st.drive.x0 + K.gate_post, st.drive.x1 - K.gate_post, true });
		if (!st.walk.empty()) g.push_back({ st.walk.x0 + K.walk_trim * HALF, st.walk.x1 - K.walk_trim * HALF, false });
	}
	return g;
}
} // namespace

void lay_boundary(LotSpace &s) {
	const LotBoundaryShape &K = s.data.shapes.boundary;
	const Kind kind = s.b.kind;
	const bool house = kind == KIND_HOUSE, homes = house || kind == KIND_FLATS || kind == KIND_TOWER, works = kind == KIND_WORKS;
	const LotRules &l = s.rules;
	const std::vector<BoundaryStyle> &styles = l.boundary[kind];
	const BoundaryStyle &style = styles.size() > 1 ? styles[size_t(s.r.irange(0, int(styles.size()) - 1))] : styles.front();
	const int wall = s.paint(s.data.lot_walls[size_t(s.b.palette) % s.data.lot_walls.size()]);
	const int front_paint = style.kind == EL_WALL ? wall : s.paint(style.kind == EL_HEDGE ? "hedge" : "fence");
	const int gate_paint = s.paint("gate");
	const float top = style.h + style.base;
	// Front: the gates, then the runs between them (F > 0), or the side strips' gates and garden walls (F == 0).
	std::vector<Gate> gates = front_gates(s);
	std::vector<Span> cuts;
	for (const Gate &g : gates) {
		s.edge(g.car ? EL_CAR_GATE : EL_GATE, { g.x0, 0, g.x1, K.fence_thick }, top, gate_paint);
		cuts.push_back({ g.x0 - K.gate_gap, g.x1 + K.gate_gap });
	}
	const float ft = thickness(K, style.kind);
	if (s.F > 0) {
		for (const Span &run : runs(K, s.L, s.R, cuts)) s.edge(style.kind, { run.first, 0, run.second, ft }, style.h, front_paint, style.base);
	} else {
		for (const Strip &st : s.strips)
			if (st.use == SIDE_GARDEN) s.edge(style.kind, { st.r.x0, 0, st.r.x1, ft }, style.h, front_paint, style.base);
	}
	// Sides and back: party walls (a works' fence), cut where the building stands on the edge.
	const Element side_kind = works ? style.kind : EL_WALL;
	const int side_paint = works ? front_paint : wall;
	const float st = thickness(K, side_kind), party = works ? style.h : l.party_wall[house ? 0 : 1];
	const float low = homes ? std::min(l.front_low, party) : party;
	for (int side = 0; side < 2; side++) {
		const float edge_x = side == 0 ? s.L : s.R;
		std::vector<Span> held;
		for (const Rect &f : s.built)
			if (std::abs((side == 0 ? f.x0 : f.x1) - edge_x) < K.attach) held.push_back({ f.y0, f.y1 });
		const float x0 = side == 0 ? s.L : s.R - st, x1 = side == 0 ? s.L + st : s.R;
		for (const Span &run : runs(K, 0, s.D, held)) {
			if (run.first < s.F) s.edge(side_kind, { x0, run.first, x1, std::min(run.second, s.F) }, low, side_paint);
			if (run.second > s.F) s.edge(side_kind, { x0, std::max(run.first, s.F), x1, run.second }, party, side_paint);
		}
	}
	std::vector<Span> held;
	for (const Rect &f : s.built)
		if (std::abs(f.y1 - s.D) < K.attach) held.push_back({ f.x0, f.x1 });
	for (const Span &run : runs(K, s.L + st, s.R - st, held)) s.edge(side_kind, { run.first, s.D - st, run.second, s.D }, party, side_paint);
}

} // namespace city::b40
