// A building record's body and its lot's frame (record.h).
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/record.h"

#include "sim/buildings/b40/civic.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float SIZE_Q = 10; // a key's sizes in decimetres
} // namespace

LotFrame lot_frame(const Building &rec) {
	const float sn = std::sin(rec.rot), cs = std::cos(rec.rot);
	// The record's centre is the lot's middle; its +z (rot 0) faces the street, so the front is half a depth that way.
	return LotFrame{ V2(rec.x + rec.d / 2 * sn, rec.z + rec.d / 2 * cs), V2(cs, -sn), V2(-sn, -cs) };
}

void record_mass(const Data &data, const Building &rec, Body &b) {
	record_body(data, rec, b);
	resolve_blocks(b);
}

void record_body(const Data &data, const Building &rec, Body &b) {
	if (rec.body) {
		b = *rec.body;
		return;
	}
	if (is_open_lot(rec.fn)) open_lot_body(data, b, rec.fn, rec.w, rec.d);
	else if (is_civic(rec.fn)) civic_body(data, b, rec.fn, rec.floors, rec.w, rec.d, rec.seed);
	else make_body(data, b, rec.zone, rec.floors, rec.w, rec.d, rec.seed);
}

BodyKey body_key(const Building &r) {
	BodyKey k;
	k.fn = r.fn;
	if (r.body) { // an edited body is its own, whatever its seed says
		k.stamp = r.body.stamp();
		return k;
	}
	k.seed = r.seed;
	k.zone = r.zone;
	k.floors = r.floors;
	k.w = int32_t(std::lround(r.w * SIZE_Q));
	k.d = int32_t(std::lround(r.d * SIZE_Q));
	return k;
}

void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out) {
	for (const LotElement &e : b.elements)
		if (e.kind == EL_TREE) out.push_back({ f.at(e.x, e.y), std::max(e.w, e.d) / 2, e.h });
}

} // namespace city::b40
