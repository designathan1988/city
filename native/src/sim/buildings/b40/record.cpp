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

int Data::grown_pick_at(float x, float z, float d, float rot, int zone) const {
	Building rec;
	rec.x = x; rec.z = z; rec.d = d; rec.rot = rot;
	const LotFrame f = lot_frame(rec);
	return grown_pick(*this, kind_of_zone(zone), quarter_of(*this, f.o, f.e1));
}

std::shared_ptr<const Body> record_body(const Data &data, const Building &rec) { return data.made->body(data, rec); }

std::shared_ptr<const Body> record_mass(const Data &data, const Building &rec) { return data.made->mass(data, rec); }

std::shared_ptr<const std::vector<LotElement>> record_elements(const Data &data, const Building &rec) { return data.made->elements(data, rec); }

void make_seed_body(const Data &data, const Building &rec, Body &b) {
	if (is_open_lot(rec.fn)) open_lot_body(data, b, rec.fn, rec.w, rec.d);
	else if (is_civic(rec.fn)) civic_body(data, b, rec.fn, rec.floors, rec.w, rec.d, rec.seed);
	else {
		const LotFrame f = lot_frame(rec);
		make_body(data, b, rec.zone, rec.floors, rec.w, rec.d, rec.seed, quarter_of(data, f.o, f.e1), LotCorner(rec.corner));
		if (rec.site) { // the ground and the outline it grew on (lot_site.h): stepped to the one, fitted to the other
			follow_slope(data, b, *rec.site, rec.w, rec.d);
			fit_to_lot(data, b, *rec.site);
		}
	}
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
	k.corner = r.corner;
	k.site = r.site ? r.site->hash : 0;
	std::memcpy(&k.w, &r.w, sizeof k.w);
	std::memcpy(&k.d, &r.d, sizeof k.d);
	std::memcpy(&k.x, &r.x, sizeof k.x);
	std::memcpy(&k.z, &r.z, sizeof k.z);
	std::memcpy(&k.rot, &r.rot, sizeof k.rot);
	return k;
}

void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out) {
	for (const LotElement &e : b.elements)
		if (e.kind == EL_TREE) out.push_back({ f.at(e.x, e.y), std::max(e.w, e.d) / 2, e.h });
}

} // namespace city::b40
