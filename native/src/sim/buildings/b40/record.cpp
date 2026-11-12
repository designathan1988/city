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

std::shared_ptr<const Body> record_body(const Data &data, const Building &rec) { return data.made->body(data, rec); }

std::shared_ptr<const Body> record_mass(const Data &data, const Building &rec) { return data.made->mass(data, rec); }

std::shared_ptr<const std::vector<LotElement>> record_elements(const Data &data, const Building &rec) { return data.made->elements(data, rec); }

void make_seed_body(const Data &data, const Building &rec, Body &b) {
	if (is_open_lot(rec.fn)) open_lot_body(data, b, rec.fn, rec.w, rec.d);
	else if (is_civic(rec.fn)) civic_body(data, b, rec.fn, rec.cap, rec.w, rec.d, rec.seed, rec.edges);
	else make_body(data, b, rec.zone, rec.cap, rec.w, rec.d, rec.seed, rec.edges);
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
	k.cap = r.cap;
	std::memcpy(&k.w, &r.w, sizeof k.w);
	std::memcpy(&k.d, &r.d, sizeof k.d);
	static_assert(sizeof r.edges.from + sizeof r.edges.to == sizeof k.shut, "the key holds every stretch's bits");
	std::memcpy(k.shut, r.edges.from, sizeof r.edges.from);
	std::memcpy(k.shut + BodyKey::SHUT_WORDS / 2, r.edges.to, sizeof r.edges.to);
	k.street = r.edges.street;
	return k;
}

void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out) {
	for (const LotElement &e : b.elements)
		if (e.kind == EL_TREE) out.push_back({ f.at(e.x, e.y), std::max(e.w, e.d) / 2, e.h });
}

} // namespace city::b40
