// A building record's body and its lot's frame (record.h).
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/record.h"

#include "sim/buildings/b40/civic.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace city::b40 {

LotFrame lot_frame(const Building &rec) {
	const float sn = std::sin(rec.rot), cs = std::cos(rec.rot);
	// The record's centre is the lot's middle; its +z (rot 0) faces the street, so the front is half a depth that way.
	return LotFrame{ V2(rec.x + rec.d / 2 * sn, rec.z + rec.d / 2 * cs), V2(cs, -sn), V2(-sn, -cs) };
}

std::vector<V2> ground_corners(const Body &b, const Building &rec, float inset) {
	const LotFrame f = lot_frame(rec);
	std::vector<V2> out;
	for (const Volume &v : b.volumes) {
		if (v.base != 0 || v.mode != BLOCK_SOLID) continue;
		const float ix = std::min(inset, v.w / 2), iy = std::min(inset, v.d / 2);
		const float x0 = v.x + ix, x1 = v.x + v.w - ix, y0 = v.y + iy, y1 = v.y + v.d - iy;
		out.insert(out.end(), { f.at(x0, y0), f.at(x1, y0), f.at(x1, y1), f.at(x0, y1) });
	}
	return out;
}

std::shared_ptr<const Body> record_body(const Data &data, const Building &rec) { return data.made->body(data, rec); }

std::shared_ptr<const Body> record_mass(const Data &data, const Building &rec) { return data.made->mass(data, rec); }

void make_seed_body(const Data &data, const Building &rec, Body &b) {
	if (is_open_lot(rec.fn)) open_lot_body(data, b, rec.fn, rec.w, rec.d);
	else if (is_civic(rec.fn)) civic_body(data, b, rec.fn, rec.floors, rec.w, rec.d, rec.seed);
	else make_body(data, b, rec.zone, rec.floors, rec.w, rec.d, rec.seed);
}

BodyKey body_key(const Building &r) {
	BodyKey k;
	k.fn = r.fn;
	k.seed = r.seed; // an edited body's purpose reads it too (interior.h purpose_of)
	if (r.body) { // an edited body is its own, whatever the rest of the record says
		k.stamp = r.body.stamp();
		return k;
	}
	k.zone = r.zone;
	k.floors = r.floors;
	std::memcpy(&k.w, &r.w, sizeof k.w);
	std::memcpy(&k.d, &r.d, sizeof k.d);
	return k;
}

void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out) {
	for (const LotElement &e : b.elements)
		if (e.kind == EL_TREE) out.push_back({ f.at(e.x, e.y), std::max(e.w, e.d) / 2, e.h });
}

} // namespace city::b40
