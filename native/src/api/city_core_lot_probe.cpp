// Every lot's edges against what is round it (CityProbes::lot_fronts). Its level: the lot's covers as they are drawn at its front
// (the ground 0.3 m inside its front at a quarter, the middle and three quarters of its frontage, plus the lot's plate: render/buildings
// lot_emit.cpp) held against the footway's top 0.3 m outside (sim/street/footway.h footway_at: the road's level there, plus the
// sidewalk's height where it is raised): a lot meets the footway it faces at the footway's top, no step down and no bank between. Its
// seams: where a footway runs along a lot's front or side, the footway's outer edge lies on the lot's line, no land between and nothing
// of the lot over the footway. Its neighbours: a lot beside it in a row shares its side, no strip of bare land between the two.
// Its edges closed once (CityProbes::lot_bounds): every edge of a lot laid round a building (sim/buildings/b40 lot_boundary.cpp)
// is closed along its whole length by its boundary (a wall, fence, railing, hedge or gate on the line) or by a ground block that
// stands on the edge, never by both (a boundary in front of a block's face, or through it) and never by neither.
#include "api/city_probes.h"

#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cmath>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float FRONT_STEP_M = 0.3f; // the heights and the footways are read this far inside and outside the lot's lines
constexpr float FRONT_SHARES[] = { -0.25f, 0.f, 0.25f }; // along the frontage from its middle, in frontages
constexpr double FOOTWAY_FIND_M = 0.25; // how far off a sidewalk's band the outside point may lie
constexpr float MEET_M = 0.01f; // two levels or two lines this close meet
constexpr float NEIGHBOUR_REACH_M = 2.f; // bare land narrower than this between two lots is a seam between them, not a way
constexpr float HALF = 0.5f;
constexpr float EDGE_LINE_M = 0.02f; // a boundary piece whose outer face stands this near a lot's edge stands on it
constexpr float BLOCK_PAST_M = 0.45f; // a ground block closes its edge (within the lot rules' block_closes) this far past its corners
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
V2 edge_point(int e, float t, float W, float D) {
	return e == EDGE_FRONT ? V2(t, 0) : e == EDGE_BACK ? V2(t, D) : e == EDGE_LEFT ? V2(-W * HALF, t) : V2(W * HALF, t);
}

struct EdgeSpan {
	float a, b;
	int kind; // the piece's Element, -1 a ground block
	V2 at; // its middle
};

// The worst of a set of readings: how many, how many off, and the first worst one as [building, x, z, value].
struct Tally {
	int seen = 0, off = 0;
	float worst = 0;
	Array first;
	void add(int id, const V2 &at, float value, bool bad) {
		seen++;
		if (!bad) return;
		off++;
		if (std::fabs(value) > std::fabs(worst) || first.is_empty()) {
			worst = value;
			first = Array::make(id, at.x, at.y, value);
		}
	}
};

// The sidewalk outside a lot's line at `out` and how far its outer edge stands outside the line at `on` along `away` (the line's
// outward unit): positive, land between the two; negative, the lot reaches over the footway. False where no sidewalk runs there.
bool seam_at(const CityState &s, const V2 &out, const V2 &on, const V2 &away, FootwayHit &hit, float &seam) {
	hit = footway_at(s.roads, out.x, out.y, FOOTWAY_FIND_M);
	if (!hit.ok || hit.side < 0) return false; // no sidewalk there (a corner's mouth, a median)
	const Profile &p = s.roads.profile_of(hit.seg);
	const float edge_u = hit.side == 0 ? p.edges.front().u0 : p.edges.back().u1;
	double ex, ez, ey;
	footway_point(s.roads, hit.seg, hit.v, edge_u, ex, ez, ey);
	seam = (V2(float(ex), float(ez)) - on).dot(away);
	return true;
}
} // namespace

Dictionary CityProbes::lot_fronts() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Tuning &tn = s.catalog.tune;
	Tally level, seams, sides;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const b40::LotFrame f = b40::lot_frame(rec);
		for (float share : FRONT_SHARES) {
			const float x = share * rec.w;
			const V2 out = f.at(x, -FRONT_STEP_M), on = f.at(x, 0), in = f.at(x, FRONT_STEP_M);
			FootwayHit hit;
			float seam;
			if (!seam_at(s, out, on, f.e1 * -1.f, hit, seam)) continue;
			seams.add(id, on, seam, std::fabs(seam) > MEET_M);
			const Profile &p = s.roads.profile_of(hit.seg);
			const Strip &edge = hit.side == 0 ? p.edges.front().strip : p.edges.back().strip;
			double wx, wz, y;
			footway_point(s.roads, hit.seg, hit.v, hit.u, wx, wz, y);
			const float top = float(y) + (edge.flush ? 0.f : tn.street_sidewalk_m);
			const float gap = s.terrain.height_at(in.x, in.y) + tn.lot_plate_m - top;
			level.add(id, in, gap, std::fabs(gap) > MEET_M);
		}
		for (float side : { -1.f, 1.f }) {
			const float edge = rec.w * HALF, mid = rec.d * HALF;
			const V2 on = f.at(side * edge, mid), away = f.e0 * side;
			FootwayHit hit;
			float seam;
			if (seam_at(s, f.at(side * (edge + FRONT_STEP_M), mid), on, away, hit, seam)) {
				seams.add(id, on, seam, std::fabs(seam) > MEET_M);
				continue;
			}
			const V2 next = f.at(side * (edge + MEET_M), mid), beyond = f.at(side * (edge + NEIGHBOUR_REACH_M), mid);
			if (on_road(s.roads, next.x, next.y)) continue; // a street beside it
			float h;
			if (s.terrain.lots.level_at(next.x, next.y, h)) sides.add(id, on, 0.f, false);
			else if (s.terrain.lots.level_at(beyond.x, beyond.y, h)) sides.add(id, on, NEIGHBOUR_REACH_M, true);
		}
	}
	Dictionary d;
	d["fronts"] = level.seen; // points of lot fronts with a sidewalk outside them
	d["off"] = level.off; // of them, those whose lot does not meet the footway's top
	d["worst"] = level.first; // building, x, z, the lot's surface less the footway's top (m)
	d["seams"] = seams.seen; // points of lots' fronts and sides with a sidewalk outside them
	d["seams_off"] = seams.off; // of them, those whose line is not the footway's outer edge
	d["seam_worst"] = seams.first; // building, x, z, how far the footway's edge stands outside the lot's line (m)
	d["sides"] = sides.seen; // lots' sides with another lot beside them, within NEIGHBOUR_REACH_M
	d["sides_apart"] = sides.off; // of them, those with bare land between the two
	d["side_worst"] = sides.first; // building, x, z, the reach the neighbour was found within (m)
	return d;
}

namespace {
// What lot_bounds finds: how many of each, the first of each, and how many by "edge kind lot-kind front-use".
struct BoundsTally {
	int lots = 0, pieces = 0, doubled = 0, open = 0, samples = 0, parts_in = 0;
	Array doubled_first, open_first, parts_first;
	Dictionary doubled_by, open_by, parts_by;
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
			const V2 p = edge_point(k, at, W, D), w = f.at(p.x, p.y);
			t.open_first = Array::make(id, k, w.x, w.y, int(b.plan.kind), b.plan.front_depth);
		}
	}
}
} // namespace

Dictionary CityProbes::lot_bounds() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const float closes = s.buildings.designs.lots.block_closes; // the lot rules' reach, the one lot_boundary.cpp lays by
	BoundsTally t;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive || b40::is_open_lot(rec.fn)) continue; // a square, a park, a court: open by design, laid with no boundary
		const std::shared_ptr<const b40::Body> body = b40::record_body(s.buildings.designs, rec);
		const b40::LotFrame f = b40::lot_frame(rec);
		t.lots++;
		std::vector<EdgeSpan> on[LOT_EDGES], blocks[LOT_EDGES];
		edges_of(*body, closes, on, blocks);
		parts_in_boundary(*body, f, id, t);
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
	d["step_m"] = EDGE_STEP_M;
	return d;
}

} // namespace city
