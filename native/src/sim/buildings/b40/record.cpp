// A building record's body and its lot's frame (record.h).
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/record.h"

#include "sim/buildings/b40/civic.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr uint32_t GOLDEN = 2654435761u;
constexpr uint64_t EDITED_MIX = 0x9e3779b97f4a7c15ull;
constexpr uint64_t SEED_BITS = 32, ZONE_BITS = 8, FLOOR_BITS = 16, SIZE_Q = 10;
} // namespace

LotFrame lot_frame(const Building &rec) {
	const float sn = std::sin(rec.rot), cs = std::cos(rec.rot);
	// The record's centre is the lot's middle; its +z (rot 0) faces the street, so the front is half a depth that way.
	return LotFrame{ V2(rec.x + rec.d / 2 * sn, rec.z + rec.d / 2 * cs), V2(cs, -sn), V2(-sn, -cs) };
}

void record_mass(const Data &data, const Building &rec, Body &b) {
	if (rec.body) {
		const BodyInput source = data.edited->input(rec.body);
		if (source.mass) { b = *source.mass; return; }
	}
	record_body(data, rec, b);
	resolve_blocks(b);
}

void record_body(const Data &data, const Building &rec, Body &b) {
	if (BodyRef edited = rec.body ? data.edited->get(rec.body) : nullptr) {
		b = *edited;
		return;
	}
	if (is_open_lot(rec.fn)) open_lot_body(data, b, rec.fn, rec.w, rec.d);
	else if (is_civic(rec.fn)) civic_body(data, b, rec.fn, rec.floors, rec.w, rec.d, rec.seed);
	else make_body(data, b, rec.zone, rec.floors, rec.w, rec.d, rec.seed);
}

uint64_t body_key(const Building &r) {
	uint64_t k = uint64_t(r.seed) << SEED_BITS;
	k ^= uint64_t(r.zone) | (uint64_t(r.floors) << ZONE_BITS) | (uint64_t(r.fn) << (ZONE_BITS + FLOOR_BITS));
	k ^= uint64_t(std::lround(r.w * SIZE_Q)) * GOLDEN ^ (uint64_t(std::lround(r.d * SIZE_Q)) << ZONE_BITS);
	return k ^ uint64_t(r.body) * EDITED_MIX; // an edited body is its own, whatever its seed says
}

void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out) {
	for (const LotElement &e : b.elements)
		if (e.kind == EL_TREE) out.push_back({ f.at(e.x, e.y), std::max(e.w, e.d) / 2, e.h });
}

} // namespace city::b40
