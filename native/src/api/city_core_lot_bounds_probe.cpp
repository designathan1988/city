// Every lot's edges closed once (the probe lot_bounds, whose method in city_core_lot_probe.cpp holds the map and calls lots_closed): every edge of a lot laid round a building (sim/buildings/b40 lot_boundary.cpp)
// is closed along its whole length by its boundary (a wall, fence, railing, hedge or gate on the line) or by a ground block that
// stands on the edge, never by both (a boundary in front of a block's face, or through it) and never by neither; no run of the boundary
// stands in a ground block's footprint; a side a street runs along (sim/commands/lot_sites.h lot_edges) is closed as a front, never by
// a party wall; and no part of the lot stands in its boundary nor in another part's footprint.
#include "api/city_probes.h"

#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/lot_layout.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/lot_sites.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float IN_BLOCK_M = 0.01f; // a run this far into a block's footprint stands in it; a wall this much lower than a party wall is none
constexpr float HALF = 0.5f;
constexpr float EDGE_LINE_M = 0.02f; // a boundary piece whose outer face stands this near a lot's edge stands on it
constexpr float BLOCK_PAST_M = 0.45f; // a ground block closes its edge (within a block's reach, lot_layout.h) this far past its corners
constexpr float PIECE_PAST_M = 0.3f; // a boundary piece closes its edge this far past its ends (its posts, the corners)
constexpr float IN_FRONT_M = 0.05f; // a boundary piece along a block's face for more than this stands doubled with it
constexpr float EDGE_STEP_M = 0.25f; // the edges are looked at this often
enum LotEdge { EDGE_FRONT, EDGE_BACK, EDGE_LEFT, EDGE_RIGHT, LOT_EDGES };

// A box of the lot's plan (x from the frontage's middle, y from the street).
struct PlanBox {
	float x0, y0, x1, y1;
};
PlanBox plan_box(const b40::LotElement &e) { return { e.x - e.w * HALF, e.y - e.d * HALF, e.x + e.w * HALF, e.y + e.d * HALF }; }
PlanBox plan_box(const b40::Volume &v) { return { v.x, v.y, v.x + v.w, v.y + v.d }; }

bool boundary_piece(const b40::LotElement &e) {
	const b40::Element k = e.kind;
	return e.z == 0 && (k == b40::EL_WALL || k == b40::EL_FENCE || k == b40::EL_RAILING || k == b40::EL_HEDGE || k == b40::EL_GATE || k == b40::EL_CAR_GATE);
}

// A part that stands on the lot's ground as a body (a post, a bin, a bench, a shed): not a cover laid flat, not a plant (its box
// is its crown), not the boundary itself.
bool standing_part(const b40::LotElement &e) {
	using namespace b40;
	const Element k = e.kind;
	const bool flat = k == EL_PAVEMENT || k == EL_SURFACE || k == EL_STALL || k == EL_DOCK || k == EL_DRAIN || k == EL_PARKING;
	const bool plant = k == EL_TREE || k == EL_SHRUB || k == EL_FLOWERS;
	return e.z == 0 && !flat && !plant && !boundary_piece(e);
}

// How far two plan boxes enter one another: the lesser of their overlaps across and along (0 or less: apart).
float overlap(const PlanBox &p, const PlanBox &q) {
	return std::min(std::min(p.x1, q.x1) - std::max(p.x0, q.x0), std::min(p.y1, q.y1) - std::max(p.y0, q.y0));
}

// How far a box stands inside edge `e` of a lot W wide and D deep, and its span [a, b] along that edge (x along the front and the
// back, y along the sides).
float inside_edge(const PlanBox &r, int e, float W, float D, float &a, float &b) {
	const bool across = e == EDGE_FRONT || e == EDGE_BACK;
	a = across ? r.x0 : r.y0;
	b = across ? r.x1 : r.y1;
	return e == EDGE_FRONT ? r.y0 : e == EDGE_BACK ? D - r.y1 : e == EDGE_LEFT ? r.x0 + W * HALF : W * HALF - r.x1;
}

// The point of edge `e` at `t` along it, in the lot's plan.
V2 point_on_edge(int e, float t, float W, float D) {
	return e == EDGE_FRONT ? V2(t, 0) : e == EDGE_BACK ? V2(t, D) : e == EDGE_LEFT ? V2(-W * HALF, t) : V2(W * HALF, t);
}

struct EdgeSpan {
	float a, b;
	int kind; // the piece's Element, -1 a ground block
	V2 at; // its middle
};

// What lot_bounds finds: how many of each, the first of each, and how many by "edge kind lot-kind front-use".
struct BoundsTally {
	int lots = 0, pieces = 0, doubled = 0, open = 0, samples = 0, parts_in = 0, parts_meet = 0, street_sides = 0, street_walls = 0, runs_in = 0;
	Array doubled_first, open_first, parts_first, meet_first, street_first, runs_first;
	Dictionary doubled_by, open_by, parts_by, meet_by;
	static void count(Dictionary &by, int edge, int kind, const b40::Body &b) {
		const String key = String::num_int64(edge) + " " + String::num_int64(kind) + " " + String::num_int64(int(b.plan.kind)) + " " + String::num_int64(int(b.plan.front));
		by[key] = int(by.get(key, 0)) + 1;
	}
};

// A lot's boundary pieces on each of its edges, and the ground blocks standing within `closes` of each.
void edges_of(const b40::Body &b, float closes, std::vector<EdgeSpan> (&on)[LOT_EDGES], std::vector<EdgeSpan> (&blocks)[LOT_EDGES]) {
	const float W = b.plan.W, D = b.plan.D;
	for (const b40::LotElement &e : b.elements) {
		if (!boundary_piece(e)) continue;
		for (int k = 0; k < LOT_EDGES; k++) {
			float lo, hi;
			if (std::fabs(inside_edge(plan_box(e), k, W, D, lo, hi)) <= EDGE_LINE_M) on[k].push_back({ lo, hi, int(e.kind), V2(e.x, e.y) });
		}
	}
	for (const b40::Volume &v : b.volumes) {
		if (v.base != 0) continue;
		for (int k = 0; k < LOT_EDGES; k++) {
			float lo, hi;
			if (inside_edge(plan_box(v), k, W, D, lo, hi) < closes) blocks[k].push_back({ lo, hi, -1, V2(v.x + v.w * HALF, v.y + v.d * HALF) });
		}
	}
}

// The boundary's runs standing in a ground block's footprint: a wall, a fence or a hedge laid into a building's wall.
void runs_in_blocks(const b40::Body &b, const b40::LotFrame &f, int id, BoundsTally &t) {
	for (const b40::LotElement &e : b.elements) {
		if (!boundary_piece(e) || e.kind == b40::EL_GATE || e.kind == b40::EL_CAR_GATE) continue;
		for (const b40::Volume &v : b.volumes) {
			const float in = v.base == 0 ? overlap(plan_box(e), plan_box(v)) : 0;
			if (in <= IN_BLOCK_M) continue;
			t.runs_in++;
			if (t.runs_first.is_empty()) {
				const V2 w = f.at(e.x, e.y);
				t.runs_first = Array::make(id, int(e.kind), w.x, w.y, in);
			}
			break;
		}
	}
}

// The parts of a lot standing in its boundary: two bodies in one another.
void parts_in_boundary(const b40::Body &b, const b40::LotFrame &f, int id, BoundsTally &t) {
	for (const b40::LotElement &e : b.elements) {
		if (!standing_part(e)) continue;
		for (const b40::LotElement &p : b.elements) {
			if (!boundary_piece(p) || overlap(plan_box(e), plan_box(p)) <= IN_FRONT_M) continue;
			t.parts_in++;
			BoundsTally::count(t.parts_by, -1, int(e.kind), b);
			if (t.parts_first.is_empty()) {
				const V2 w = f.at(e.x, e.y);
				t.parts_first = Array::make(id, int(e.kind), int(p.kind), w.x, w.y, overlap(plan_box(e), plan_box(p)));
			}
			break;
		}
	}
}

// A part's footprint on the ground: its box, a pool's with the deck laid round it (lots.pool_deck_m, lot_yard.cpp).
PlanBox footprint(const b40::LotElement &e, float deck) {
	const PlanBox r = plan_box(e);
	return e.kind == b40::EL_POOL ? PlanBox{ r.x0 - deck, r.y0 - deck, r.x1 + deck, r.y1 + deck } : r;
}

// A slab laid on the ground (a fuel island, a portico's floor) is a platform: what stands on it stands on it, not in it.
bool platform(const b40::LotElement &e) { return e.kind == b40::EL_SLAB && e.z == 0; }

// The lot's standing parts in one another, a pool with its deck among them: nothing solid stands in another's footprint.
void parts_in_parts(const b40::Body &b, const b40::LotFrame &f, int id, float deck, BoundsTally &t) {
	const std::vector<b40::LotElement> &el = b.elements;
	for (size_t i = 0; i < el.size(); i++) {
		if (!standing_part(el[i]) || platform(el[i])) continue;
		for (size_t j = i + 1; j < el.size(); j++) {
			if (!standing_part(el[j]) || platform(el[j])) continue;
			const float in = overlap(footprint(el[i], deck), footprint(el[j], deck));
			if (in <= IN_FRONT_M) continue;
			t.parts_meet++;
			BoundsTally::count(t.meet_by, int(el[i].kind), int(el[j].kind), b);
			if (t.meet_first.is_empty()) {
				const V2 w = f.at(el[i].x, el[i].y);
				t.meet_first = Array::make(id, int(el[i].kind), int(el[j].kind), w.x, w.y, in);
			}
		}
	}
}

// A lot's sides a street runs along (lot_edges, read from the streets as they stand) and the party walls standing on them: a wall of a
// party wall's height (the lot rules' lower one) on a side that faces a street is a blind wall along it.
void street_walls(const CityState &s, const Building &rec, const b40::Body &b, const b40::LotFrame &f, int id, BoundsTally &t) {
	const b40::LotEdges edges = lot_edges(s, rec);
	const b40::LotRules &l = s.buildings.designs.lots;
	const float party = std::min(l.party_wall[0], l.party_wall[1]);
	for (int k : { EDGE_LEFT, EDGE_RIGHT }) {
		if (!edges.on_street(k == EDGE_LEFT ? 0 : 1)) continue;
		t.street_sides++;
		for (const b40::LotElement &e : b.elements) {
			float lo, hi;
			if (e.kind != b40::EL_WALL || e.z != 0 || e.h < party - IN_BLOCK_M) continue;
			if (std::fabs(inside_edge(plan_box(e), k, b.plan.W, b.plan.D, lo, hi)) > EDGE_LINE_M) continue;
			t.street_walls++;
			if (t.street_first.is_empty()) {
				const V2 w = f.at(e.x, e.y);
				t.street_first = Array::make(id, k, w.x, w.y, e.h, int(b.plan.kind));
			}
		}
	}
}

// One edge of a lot: its boundary pieces along a block's face (in front of it or through it), and its points no piece and no
// block closes.
void judge_edge(const b40::Body &b, const b40::LotFrame &f, int id, int k, const std::vector<EdgeSpan> &on, const std::vector<EdgeSpan> &blocks, BoundsTally &t) {
	for (const EdgeSpan &p : on) {
		t.pieces++;
		for (const EdgeSpan &q : blocks) {
			const float along = std::min(p.b, q.b) - std::max(p.a, q.a);
			if (along <= IN_FRONT_M) continue;
			t.doubled++;
			BoundsTally::count(t.doubled_by, k, p.kind, b);
			if (t.doubled_first.is_empty()) {
				const V2 w = f.at(p.at.x, p.at.y);
				t.doubled_first = Array::make(id, k, p.kind, w.x, w.y, along);
			}
			break;
		}
	}
	const float W = b.plan.W, D = b.plan.D;
	const bool across = k == EDGE_FRONT || k == EDGE_BACK;
	const float t0 = across ? -W * HALF : 0, t1 = across ? W * HALF : D;
	for (float at = t0 + EDGE_STEP_M * HALF; at < t1; at += EDGE_STEP_M) {
		t.samples++;
		bool closed = false;
		for (const EdgeSpan &p : on) closed = closed || (at >= p.a - PIECE_PAST_M && at <= p.b + PIECE_PAST_M);
		for (const EdgeSpan &q : blocks) closed = closed || (at >= q.a - BLOCK_PAST_M && at <= q.b + BLOCK_PAST_M);
		if (closed) continue;
		t.open++;
		BoundsTally::count(t.open_by, k, -1, b);
		if (t.open_first.is_empty()) {
			const V2 p = point_on_edge(k, at, W, D), w = f.at(p.x, p.y);
			t.open_first = Array::make(id, k, w.x, w.y, int(b.plan.kind), b.plan.front_depth);
		}
	}
}
} // namespace

// Under the caller's hold of the map (CityProbes::lot_bounds, city_core_lot_probe.cpp).
Dictionary lots_closed(const CityState &s) {
	const float closes = b40::block_reach(s.buildings.designs.lots); // the reach lot_boundary.cpp lays by, asked of its one owner
	BoundsTally t;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive || b40::is_open_lot(rec.fn)) continue; // a square, a park, a court: open by design, laid with no boundary
		const std::shared_ptr<const b40::Body> body = b40::record_body(s.buildings.designs, rec);
		const b40::LotFrame f = b40::lot_frame(rec);
		t.lots++;
		std::vector<EdgeSpan> on[LOT_EDGES], blocks[LOT_EDGES];
		edges_of(*body, closes, on, blocks);
		runs_in_blocks(*body, f, id, t);
		parts_in_boundary(*body, f, id, t);
		parts_in_parts(*body, f, id, s.buildings.designs.lots.pool_deck, t);
		street_walls(s, rec, *body, f, id, t);
		for (int k = 0; k < LOT_EDGES; k++) judge_edge(*body, f, id, k, on[k], blocks[k], t);
	}
	Dictionary d;
	d["lots"] = t.lots; // lots laid round a building (every lot but the open ones)
	d["pieces"] = t.pieces; // boundary pieces on their lots' edges
	d["doubled"] = t.doubled; // of them, those along a ground block's face on the same edge
	d["doubled_first"] = t.doubled_first; // building, edge (front, back, left, right), the piece's kind, x, z, metres along the block
	d["samples"] = t.samples; // points of the lots' edges looked at, EDGE_STEP_M apart
	d["open"] = t.open; // of them, those no piece and no block closes
	d["open_first"] = t.open_first; // building, edge, x, z, the lot's kind and front depth
	d["doubled_by"] = t.doubled_by; // "edge piece-kind lot-kind front-use": doubled pieces
	d["open_by"] = t.open_by; // "edge -1 lot-kind front-use": open points
	d["parts_in"] = t.parts_in; // parts standing on the ground in a boundary piece
	d["parts_first"] = t.parts_first; // building, the part's kind, the piece's kind, x, z, metres in
	d["parts_by"] = t.parts_by; // "-1 part-kind lot-kind front-use": how many
	d["parts_meet"] = t.parts_meet; // pairs of a lot's standing parts in one another (a pool with its deck)
	d["meet_first"] = t.meet_first; // building, the two parts' kinds, x, z, metres in
	d["meet_by"] = t.meet_by; // "kind kind lot-kind front-use": how many
	d["street_sides"] = t.street_sides; // lots' sides a street runs along
	d["street_walls"] = t.street_walls; // party walls standing on them
	d["street_first"] = t.street_first; // building, edge, x, z, the wall's height, the lot's kind
	d["runs_in"] = t.runs_in; // boundary runs (a wall, a fence, a railing, a hedge) standing in a ground block's footprint
	d["runs_first"] = t.runs_first; // building, the run's kind, x, z, metres in
	d["step_m"] = EDGE_STEP_M;
	return d;
}

} // namespace city
