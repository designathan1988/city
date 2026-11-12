// The demolition block of a save (save_demolition.h).
#include "sim/persistence/save_demolition.h"

#include "sim/persistence/save_format.h"
#include "sim/persistence/body_codec.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
constexpr uint32_t SAVED_ENTRIES_MAX = 1000000; // sanity bound on any saved table of the block

void put_block(ByteWriter &w, const DamageState &d) {
	w.put<uint32_t>(d.strikes);
	w.put<uint32_t>(d.shots);
	w.put<double>(std::max(0.0, d.growth_hold_until - d.now)); // what is left to wait: the clock starts over on load
	w.put<uint32_t>(uint32_t(d.craters.size()));
	for (const Crater &c : d.craters) {
		for (float v : { c.x, c.z, c.radius, c.depth }) w.put<float>(v);
		w.put<uint8_t>(c.hard);
		w.put<uint32_t>(c.seed);
	}
	w.put<uint32_t>(uint32_t(d.ruins().size()));
	for (const Ruin &r : d.ruins()) {
		for (float v : { r.x, r.z, r.w, r.d, r.rot }) w.put<float>(v);
		w.put<uint32_t>(r.seed);
		put_building(w, r.record);
	}
	w.put<uint32_t>(uint32_t(d.gone().size()));
	for (const Gone &g : d.gone()) {
		w.put<uint8_t>(g.kind);
		w.put<float>(g.x);
		w.put<float>(g.z);
	}
	w.put<uint32_t>(uint32_t(d.hurt().size()));
	for (const Hurt &h : d.hurt()) {
		w.put<uint32_t>(h.seed);
		for (float v : { h.x, h.z, h.amount }) w.put<float>(v);
	}
	for (const Ruin &r : d.ruins())
		for (float v : { r.blow_x, r.blow_y, r.blow_z, r.blow_force }) w.put<float>(v);
	w.put<uint32_t>(uint32_t(d.scars().size()));
	for (const Scar &c : d.scars()) {
		w.put<uint32_t>(c.seed);
		for (float v : { c.bx, c.bz, c.x, c.y, c.z, c.force }) w.put<float>(v);
	}
	// Optional tail: old readers ignore it; old files end after scars. Procedural-only damage keeps the same bytes.
	uint32_t authored = 0;
	for (size_t i = 0; i < d.ruins().size(); i++)
		if (i < d.ruin_bodies().size() && d.ruin_bodies()[i]) authored++;
	if (authored == 0) return;
	w.put<uint32_t>(authored);
	for (uint32_t i = 0; i < d.ruins().size(); i++) {
		if (i >= d.ruin_bodies().size() || !d.ruin_bodies()[i]) continue;
		w.put<uint32_t>(i);
		write_body(w, *d.ruin_bodies()[i]);
	}
}

bool get_block(ByteReader &r, DamageState &d, b40::BodyTable &bodies) {
	d.clear();
	DamageRecords records;
	const auto finish = [&]() {
		if (r.ok) d.restore(std::move(records));
		return r.ok;
	};
	d.strikes = r.get<uint32_t>();
	d.shots = r.get<uint32_t>();
	d.growth_hold_until = r.get<double>();
	uint32_t n = r.get<uint32_t>();
	if (!r.require_items(n, 4 * sizeof(float) + sizeof(uint8_t) + sizeof(uint32_t), SAVED_ENTRIES_MAX)) return false;
	d.craters.resize(n);
	for (Crater &c : d.craters) {
		c.x = r.get<float>(); c.z = r.get<float>(); c.radius = r.get<float>(); c.depth = r.get<float>();
		c.hard = r.get<uint8_t>();
		c.seed = r.get<uint32_t>();
	}
	n = r.get<uint32_t>();
	if (!r.require_items(n, 5 * sizeof(float) + sizeof(uint32_t) + SAVED_BUILDING_BYTES, SAVED_ENTRIES_MAX)) return false;
	records.ruins.resize(n);
	records.ruin_bodies.resize(n);
	for (Ruin &u : records.ruins) {
		u.x = r.get<float>(); u.z = r.get<float>(); u.w = r.get<float>(); u.d = r.get<float>(); u.rot = r.get<float>();
		u.seed = r.get<uint32_t>();
		if (!get_building(r, u.record)) return false;
	}
	n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(uint8_t) + 2 * sizeof(float), SAVED_ENTRIES_MAX)) return false;
	records.gone.resize(n);
	for (Gone &g : records.gone) {
		g.kind = r.get<uint8_t>();
		g.x = r.get<float>();
		g.z = r.get<float>();
	}
	n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(uint32_t) + 3 * sizeof(float), SAVED_ENTRIES_MAX)) return false;
	records.hurt.resize(n);
	for (Hurt &h : records.hurt) {
		h.seed = r.get<uint32_t>();
		h.x = r.get<float>(); h.z = r.get<float>(); h.amount = r.get<float>();
	}
	for (Ruin &u : records.ruins) { // a save from before this has none: the block ends here, and each ruin falls from its middle
		if (r.o >= r.n) {
			u.blow_x = u.x;
			u.blow_z = u.z;
			continue;
		}
		u.blow_x = r.get<float>(); u.blow_y = r.get<float>(); u.blow_z = r.get<float>(); u.blow_force = r.get<float>();
	}
	if (r.o >= r.n) return finish(); // a save from before scars: every standing building is whole
	n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(uint32_t) + 6 * sizeof(float), SAVED_ENTRIES_MAX)) return false;
	records.scars.resize(n);
	for (Scar &c : records.scars) {
		c.seed = r.get<uint32_t>();
		c.bx = r.get<float>(); c.bz = r.get<float>(); c.x = r.get<float>(); c.y = r.get<float>(); c.z = r.get<float>(); c.force = r.get<float>();
	}
	if (r.o >= r.n) return finish();
	n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(uint32_t), records.ruins.size())) return false;
	for (uint32_t k = 0; k < n; k++) {
		const uint32_t index = r.get<uint32_t>();
		b40::Body body;
		if (!r.ok || index >= records.ruins.size() || records.ruin_bodies[index] || !read_body(r, body)) return false;
		const uint32_t id = bodies.add(std::move(body));
		records.ruin_bodies[index] = bodies.get(id);
		records.ruins[index].record.body = id;
	}
	return finish();
}
} // namespace

bool write_damage_block(ByteWriter &w, const DamageState &d) {
	if (d.craters.empty() && d.ruins().empty() && d.gone().empty() && d.hurt().empty() && d.strikes == 0 && d.shots == 0) return false;
	put_block(w, d);
	return true;
}

bool read_damage_block(ByteReader &r, DamageState &d, b40::BodyTable &bodies) { return get_block(r, d, bodies); }

} // namespace city
