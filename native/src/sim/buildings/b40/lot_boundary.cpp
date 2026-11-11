// The lot's boundary, made whole: every edge is closed once along its whole length, by its run (a wall, fence, railing or hedge),
// by its gates or by a ground block of the lot standing on it (within buildings.json lots.block_closes_m: the block's wall closes
// it), never by two of them. open_boundary plans it before the zones lay their parts: the front takes one of its kind's styles (buildings.json
// lots.boundary), the sides and back are party walls (a works' fence), low beside a front garden, and a side a street runs along is
// closed as the front is (LotEdges: a corner lot's second street sees no blind wall); the gates are laid (the people's at the door, a
// car's on every drive, the car pad's and the works visitors'; with no front zone the side strips' own), none on a stretch of the front a
// junction's kerb takes (LotEdges: the plan and the entrance keep the drives and the door off it, the other gates are not laid there),
// each standing open with the room of its leaves held, and every edge's band is held, so no part is put where the boundary or a leaf will
// stand, its pieces kept from the lot's budget. lay_boundary lays the
// runs last, when every block is known (a garage on the drive's strip closes its stretch of the side): each edge less its gates less
// the stretches its blocks close.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float GATE_POST = 0.2f; // a car gate is its drive less a post each side; an open leaf's room across, at the opening's side
constexpr float GATE_GAP = 0.05f; // a run stops this short of a gate
constexpr float SHORTEST_RUN = 0.3f; // shorter stretches are left to the posts and the corners
constexpr float WALK_TRIM = 0.2f; // a side path's gate is the path less this
constexpr int LATE_BLOCK_PIECES = 2; // a block laid after the plan (a garage) cuts one run of a side or the back in two, each split at the front zone's end
constexpr float HALF = 0.5f;

using Span = std::pair<float, float>;
enum LotEdge { EDGE_FRONT, EDGE_BACK, EDGE_LEFT, EDGE_RIGHT, EDGES };

float thickness(Element k) { return k == EL_HEDGE ? HEDGE_THICK : k == EL_WALL ? BOUNDARY_WALL_T : FENCE_THICK; }

// [a, b] less the cuts, the stretches at least SHORTEST_RUN long.
std::vector<Span> runs(float a, float b, std::vector<Span> cuts) {
	std::sort(cuts.begin(), cuts.end());
	std::vector<Span> out;
	float at = a;
	for (const Span &c : cuts) {
		if (c.first - at >= SHORTEST_RUN) out.push_back({ at, std::min(c.first, b) });
		at = std::max(at, c.second);
	}
	if (b - at >= SHORTEST_RUN) out.push_back({ at, b });
	return out;
}

struct Gate {
	float x0, x1;
	bool car;
};

std::vector<Gate> planned_gates(LotSpace &s); // where the people's, the cars' and the works visitors' gates go

// The gates of the lot's front, each opening between the side runs' bands (a side strip's walk or drive reaches the lot's edge, where the
// side wall stands): a gate and its open leaves never stand in a side wall.
std::vector<Gate> front_gates(LotSpace &s) {
	std::vector<Gate> g = planned_gates(s);
	const float lo = s.L + s.bounds.band(0), hi = s.R - s.bounds.band(1);
	for (Gate &one : g) {
		one.x0 = std::max(one.x0, lo);
		one.x1 = std::min(one.x1, hi);
	}
	g.erase(std::remove_if(g.begin(), g.end(), [](const Gate &one) { return one.x1 - one.x0 < SHORTEST_RUN; }), g.end());
	return g;
}

std::vector<Gate> planned_gates(LotSpace &s) {
	const bool house = s.b.kind == KIND_HOUSE;
	const LotRules &l = s.rules;
	std::vector<Gate> g;
	if (s.F > 0) {
		float pg = l.person_gate[house ? 0 : 1] * HALF;
		g.push_back({ s.gate_x - pg, s.gate_x + pg, false });
		for (const Strip &st : s.strips)
			if (!st.drive.empty()) g.push_back({ st.drive.x0 + GATE_POST, st.drive.x1 - GATE_POST, true });
		Rect pad = car_pad(s);
		if (!pad.empty()) g.push_back({ pad.x0, pad.x1, true });
		if (s.plan.front == FRONT_APRON && s.F >= l.row_depth(1)) {
			// The visitors' gate onto the apron, as wide as its aisle, on the side away from the door.
			float x = s.door_x > 0 ? s.L + l.aisle * HALF + GATE_POST : s.R - l.aisle * HALF - GATE_POST;
			Gate v{ x - l.aisle * HALF, x + l.aisle * HALF, true };
			bool free = !s.plan.edges.shut(LotEdges::CAR, v.x0, v.x1);
			for (const Gate &o : g) free = free && (v.x1 < o.x0 || o.x1 < v.x0);
			if (free) g.push_back(v);
		}
		return g;
	}
	for (const Strip &st : s.strips) {
		if (!st.drive.empty()) g.push_back({ st.drive.x0 + GATE_POST, st.drive.x1 - GATE_POST, true });
		const Gate walk{ st.walk.x0 + WALK_TRIM * HALF, st.walk.x1 - WALK_TRIM * HALF, false };
		if (!st.walk.empty() && !s.plan.edges.shut(LotEdges::WALK, walk.x0, walk.x1)) g.push_back(walk); // onto a crosswalk's landing: closed
	}
	return g;
}

// The stretches of edge `e` the lot's ground blocks close: every block whose face stands within the lot rules' block_closes of the
// edge, its span along it (x along the front and the back, y along the sides).
std::vector<Span> closed_by_blocks(const LotSpace &s, LotEdge e) {
	std::vector<Span> out;
	for (const Rect &f : s.built) {
		const float in = e == EDGE_FRONT ? f.y0 : e == EDGE_BACK ? s.D - f.y1 : e == EDGE_LEFT ? f.x0 - s.L : s.R - f.x1;
		if (in < s.rules.block_closes) out.push_back(e == EDGE_FRONT || e == EDGE_BACK ? Span{ f.x0, f.x1 } : Span{ f.y0, f.y1 });
	}
	return out;
}

// An edge's runs: the edge (the back between the sides' bands) less its gates and less the stretches its blocks close.
std::vector<Span> edge_runs(const LotSpace &s, LotEdge e) {
	std::vector<Span> cuts = closed_by_blocks(s, e);
	if (e == EDGE_FRONT) {
		cuts.insert(cuts.end(), s.bounds.gates.begin(), s.bounds.gates.end());
		return runs(s.L, s.R, cuts);
	}
	if (e == EDGE_BACK) return runs(s.L + s.bounds.band(0), s.R - s.bounds.band(1), cuts);
	return runs(0, s.D, cuts);
}

// The pieces the runs take: a side's run is two where it passes the front zone's end (low beside it, a party wall behind).
int pieces_of(const LotSpace &s) {
	int n = 0;
	for (int e = EDGE_FRONT; e < EDGES; e++)
		for (const Span &r : edge_runs(s, LotEdge(e))) n += (e == EDGE_LEFT || e == EDGE_RIGHT) && r.first < s.F && r.second > s.F ? 2 : 1;
	return n;
}
} // namespace

void open_boundary(LotSpace &s) {
	const Kind kind = s.b.kind;
	const bool house = kind == KIND_HOUSE, homes = house || kind == KIND_FLATS || kind == KIND_TOWER, works = kind == KIND_WORKS;
	const LotRules &l = s.rules;
	BoundaryPlan &p = s.bounds;
	const std::vector<BoundaryStyle> &styles = l.boundary[kind];
	p.front = styles.size() > 1 ? styles[size_t(s.r.irange(0, int(styles.size()) - 1))] : styles.front();
	const int wall = s.paint(s.data.lot_walls[size_t(s.b.palette) % s.data.lot_walls.size()]);
	p.front_paint = p.front.kind == EL_WALL ? wall : s.paint(p.front.kind == EL_HEDGE ? "hedge" : "fence");
	const int gate_paint = s.paint("gate");
	p.side = works ? p.front.kind : EL_WALL;
	p.side_paint = works ? p.front_paint : wall;
	p.front_t = thickness(p.front.kind);
	p.side_t = thickness(p.side);
	p.party = works ? p.front.h : l.party_wall[house ? 0 : 1];
	p.low = homes ? std::min(l.front_low, p.party) : p.party;
	for (int side = 0; side < 2; side++) p.street[side] = s.plan.edges.on_street(side);
	const float top = p.front.h + p.front.base;
	for (const Gate &g : front_gates(s)) {
		// As deep as the front's band, so its leaves stand open past the run (a hedge's 0.7 m, a wall's 0.2 m), each in a strip a post's width
		// along the opening's side, as far as it reaches: held, so no part the zones put stands in a leaf (render/buildings/lot_emit.cpp draws them).
		s.edge(g.car ? EL_CAR_GATE : EL_GATE, { g.x0, 0, g.x1, p.front_t }, top, gate_paint);
		p.gates.push_back({ g.x0 - GATE_GAP, g.x1 + GATE_GAP });
		const int leaves = l.gate_leaves(g.x1 - g.x0);
		const float reach = (g.x1 - g.x0) / float(leaves);
		s.hold({ g.x0, p.front_t, g.x0 + GATE_POST, p.front_t + reach });
		if (leaves > 1) s.hold({ g.x1 - GATE_POST, p.front_t, g.x1, p.front_t + reach });
	}
	// The bands the runs will stand on, held from every part the zones lay meanwhile.
	s.hold({ s.L, 0, s.R, p.front_t });
	s.hold({ s.L, 0, s.L + p.band(0), s.D });
	s.hold({ s.R - p.band(1), 0, s.R, s.D });
	s.hold({ s.L, s.D - p.side_t, s.R, s.D });
	p.pieces = pieces_of(s) + LATE_BLOCK_PIECES;
}

void lay_boundary(LotSpace &s) {
	BoundaryPlan &p = s.bounds;
	p.pieces = 0; // the budget kept for the runs is theirs now
	for (const Span &r : edge_runs(s, EDGE_FRONT)) s.edge(p.front.kind, { r.first, 0, r.second, p.front_t }, p.front.h, p.front_paint, p.front.base);
	for (LotEdge e : { EDGE_LEFT, EDGE_RIGHT }) {
		const int side = e == EDGE_LEFT ? 0 : 1;
		const float x0 = e == EDGE_LEFT ? s.L : s.R - p.band(side), x1 = x0 + p.band(side);
		for (const Span &r : edge_runs(s, e)) {
			if (p.street[side]) { // along a street: closed as the front is
				s.edge(p.front.kind, { x0, r.first, x1, r.second }, p.front.h, p.front_paint, p.front.base);
				continue;
			}
			if (r.first < s.F) s.edge(p.side, { x0, r.first, x1, std::min(r.second, s.F) }, p.low, p.side_paint);
			if (r.second > s.F) s.edge(p.side, { x0, std::max(r.first, s.F), x1, r.second }, p.party, p.side_paint);
		}
	}
	for (const Span &r : edge_runs(s, EDGE_BACK)) s.edge(p.side, { r.first, s.D - p.side_t, r.second, s.D }, p.party, p.side_paint);
}

} // namespace city::b40
