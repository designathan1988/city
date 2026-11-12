// What a placed body's lot draws against its own lines and its open gates: each element drawn alone as lot_emit.cpp draws it, its vertices
// held against the lot's lines and against its gate's passage; the lot's parts and blocks held against the strips the open leaves stand in;
// each ground cover's mesh against the area its outline holds.
#include "render/buildings/lot_census.h"

#include "render/buildings/roof_util.h"
#include "sim/buildings/b40/element_shape.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float PAST_LOT_M = 0.02f; // how far past its lot's line a part may be written and still stand on it
constexpr float LEAF_CLEAR_M = 0.01f; // a hair past a leaf's thickness: a leaf hung at a post stands beside the passage, not in it
// How many of the points lie past the lot's lines of the body placed (its plan's width either side of the front's middle, its depth back).
int64_t past_lines(const b40::Placed &pl, const std::vector<Vector3> &points) {
	const b40::Body &b = *pl.b;
	int64_t past = 0;
	for (const Vector3 &p : points) {
		const Vector3 d = p - pl.lot.o;
		const float x = d.dot(pl.lot.e0), y = d.dot(pl.lot.e1);
		past += std::abs(x) > b.plan.W / 2 + PAST_LOT_M || y < -PAST_LOT_M || y > b.plan.D + PAST_LOT_M ? 1 : 0;
	}
	return past;
}

// The vertices of a body's pitched roofs that lie past its lot's lines: a roof ends at its own lot, whatever its eaves.
int64_t roof_past_lot(const b40::Placed &pl) {
	const b40::Body &b = *pl.b;
	if (b.plan.W <= 0 || b.plan.D <= 0) return 0; // a body with no lot known (a model, a design)
	b40::Shell roofs;
	for (const b40::Volume &v : b.volumes)
		if (!b40::roof_flat(v.roof) && v.outline.empty()) b40::emit_pitched(roofs, pl, v, b.level_z(v.top()));
	return past_lines(pl, roofs.pos);
}

// The vertices of a lot's ground and boundary (its surfaces, walls, fences, railings, hedges and gates, as lot_emit.cpp draws them) that
// lie past its lot's lines: what a lot lays stays on its own land, plinths and copings included, so two lots on one line never stand in
// one another and none stands on the footway. Each element is drawn alone, so the first one past is named: `first` gets its building,
// kind, middle and size in the lot's frame, and how many of its vertices lie past.
int64_t lot_past_lot(const b40::Placed &pl, int id, Array &first) {
	const b40::Body &b = *pl.b;
	if (b.plan.W <= 0 || b.plan.D <= 0) return 0;
	b40::Body one = b;
	b40::Placed only = pl;
	only.b = &one;
	int64_t past = 0;
	for (const b40::LotElement &e : b.elements) {
		if (e.kind != b40::EL_SURFACE && e.kind != b40::EL_WALL && e.kind != b40::EL_FENCE && e.kind != b40::EL_RAILING && e.kind != b40::EL_HEDGE &&
				e.kind != b40::EL_GATE && e.kind != b40::EL_CAR_GATE) continue;
		one.elements.assign(1, e);
		b40::Shell shell, parts;
		b40::emit_lot(shell, parts, only);
		const int64_t out = past_lines(pl, shell.pos) + past_lines(pl, parts.pos);
		if (out > 0 && first.is_empty()) first = Array::make(id, int(e.kind), e.x, e.y, e.w, e.d, out);
		past += out;
	}
	return past;
}

// Whether a lot element stands where an open gate's leaf stands: a part with a body under the gate's top (no flat cover, bay, dock or drain;
// nothing hung over the leaf), the gate itself aside.
bool stands_in_leaf(const b40::LotElement &o, const b40::LotElement &gate) {
	switch (o.kind) {
		case b40::EL_SURFACE: case b40::EL_PAVEMENT: case b40::EL_STALL: case b40::EL_DOCK: case b40::EL_DRAIN: case b40::EL_PARKING: return false;
		default: return &o != &gate && o.z < gate.h;
	}
}

// What stands in the leaves of a lot's open gates: the lot's elements and ground blocks (a garage) whose footprint meets the strip a leaf
// stands in, from the run's inner face into the lot as far as it reaches along the opening's side (data.h LotRules::gate_leaves). `first`
// gets the first one's building, the gate's kind, the blocker's kind (-1 a ground block), middle and size in the lot's frame.
int64_t leaves_blocked(const b40::Placed &pl, const b40::LotElement &e, int id, Array &first) {
	const b40::Body &b = *pl.b;
	const int leaves = pl.data->lots.gate_leaves(e.w);
	const float reach = e.w / float(leaves), bar = pl.look->fence_bar_m, in = e.facing == b40::FACE_BACK ? -1.f : 1.f;
	const float root = e.y + in * e.d / 2, y0 = std::min(root, root + in * reach), y1 = std::max(root, root + in * reach);
	int64_t blocked = 0;
	for (int k = 0; k < leaves; k++) {
		const float x0 = k == 0 ? e.x - e.w / 2 : e.x + e.w / 2 - bar, x1 = x0 + bar;
		// Entering the strip by more than a hair: a wall that ends where the leaf starts touches it.
		const auto meets = [&](float a0, float b0, float a1, float b1, int kind) {
			const float t = LEAF_CLEAR_M;
			const bool in_leaf = a0 < x1 - t && x0 + t < a1 && b0 < y1 - t && y0 + t < b1;
			if (in_leaf && first.is_empty()) first = Array::make(id, int(e.kind), kind, (a0 + a1) / 2, (b0 + b1) / 2, a1 - a0, b1 - b0);
			return in_leaf ? 1 : 0;
		};
		for (const b40::LotElement &o : b.elements)
			if (stands_in_leaf(o, e)) blocked += meets(o.x - o.w / 2, o.y - o.d / 2, o.x + o.w / 2, o.y + o.d / 2, int(o.kind));
		for (const b40::Volume &v : b.volumes)
			if (v.base == 0) blocked += meets(v.x, v.y, v.x + v.w, v.y + v.d, -1);
	}
	return blocked;
}

// The vertices of a gate (as lot_emit.cpp draws it, alone in `only`) standing in its own passage: between its posts clear of a leaf's
// thickness each side, from the run's street face into the lot as far as a leaf reaches. A gate stands open, so a car drives and a person
// walks through it with nothing of it in the way.
int64_t gate_in_passage(const b40::Placed &only, const b40::LotElement &e) {
	b40::Shell shell, parts;
	b40::emit_lot(shell, parts, only);
	const float side = only.look->fence_bar_m + LEAF_CLEAR_M;
	const float x0 = e.x - e.w / 2 + side, x1 = e.x + e.w / 2 - side, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2 + e.w;
	int64_t in = 0;
	for (const Vector3 &p : parts.pos) {
		const Vector3 d = p - only.lot.o;
		const float x = d.dot(only.lot.e0), y = d.dot(only.lot.e1);
		in += x > x0 && x < x1 && y > y0 && y < y1 ? 1 : 0;
	}
	return in;
}
} // namespace

int64_t covers_short(const b40::Placed &pl, Array &first) {
	const b40::Body &b = *pl.b;
	b40::Body one = b;
	b40::Placed only = pl;
	only.b = &one;
	int64_t count = 0;
	for (size_t i = 0; i < b.elements.size(); i++) {
		const b40::LotElement &e = b.elements[i];
		if (e.kind != b40::EL_SURFACE) continue;
		one.elements.assign(1, e);
		b40::Shell shell, parts;
		b40::emit_lot(shell, parts, only);
		double drawn = 0; // twice the triangles' area in the lot's plan
		for (size_t k = 0; k + 2 < shell.idx.size(); k += 3) {
			const Vector3 ab = shell.pos[size_t(shell.idx[k + 1])] - shell.pos[size_t(shell.idx[k])], ac = shell.pos[size_t(shell.idx[k + 2])] - shell.pos[size_t(shell.idx[k])];
			drawn += std::abs(double(ab.dot(pl.lot.e0)) * ac.dot(pl.lot.e1) - double(ab.dot(pl.lot.e1)) * ac.dot(pl.lot.e0));
		}
		// Whole, less what the corners' rounding may take: a band of the geometric tolerance along the outline's sides.
		const b40::Outline o = b40::outline_of(e);
		const float whole = o.area();
		double round_about = 0;
		for (int k = 0; k < o.n; k++) round_about += std::hypot(o.p[(k + 1) % o.n].x - o.p[k].x, o.p[(k + 1) % o.n].y - o.p[k].y);
		if (drawn / 2 >= double(whole) - round_about * b40::EPS) continue;
		count++;
		if (first.is_empty()) first = Array::make(int(i), e.x, e.y, e.w, e.d, drawn / 2, whole);
	}
	return count;
}

void LotCensus::add(const b40::Placed &pl, int id) {
	roof_past += roof_past_lot(pl);
	lot_past += lot_past_lot(pl, id, lot_past_first);
	const b40::Body &b = *pl.b;
	b40::Body one = b;
	b40::Placed only = pl;
	only.b = &one;
	for (const b40::LotElement &e : b.elements) {
		if (e.kind != b40::EL_GATE && e.kind != b40::EL_CAR_GATE) continue;
		gates++;
		blocked += leaves_blocked(pl, e, id, blocked_first);
		one.elements.assign(1, e);
		const int64_t in = gate_in_passage(only, e);
		if (in > 0 && shut_first.is_empty()) shut_first = Array::make(id, int(e.kind), e.x, e.y, e.w, in);
		shut += in;
	}
}

void LotCensus::put(Dictionary &d) const {
	d["roof_past_lot"] = roof_past; // vertices of pitched roofs written past their lot's lines
	d["lot_past_lot"] = lot_past; // vertices of the lots' ground and boundaries written past their lot's lines
	d["lot_past_first"] = lot_past_first; // the first element past: building's seed, kind, middle and size in the lot's frame, vertices past
	d["gates"] = gates; // the lots' gates, a car's and a person's
	d["gates_shut"] = shut; // vertices of the gates standing in their own passage
	d["gates_shut_first"] = shut_first; // the first shut gate: building's seed, kind, middle and width in the lot's frame, vertices in it
	d["leaves_blocked"] = blocked; // the lots' parts and ground blocks standing where an open gate's leaf stands
	d["leaves_blocked_first"] = blocked_first; // the first: building's seed, gate kind, blocker kind (-1 a block), middle and size
}

} // namespace city
