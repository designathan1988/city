// The demolition block of a save (save_demolition.h).
#include "sim/persistence/save_demolition.h"

#include "sim/persistence/save_format.h"
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
	w.put<uint32_t>(uint32_t(d.ruins.size()));
	for (const Ruin &r : d.ruins) {
		for (float v : { r.x, r.z, r.w, r.d, r.rot }) w.put<float>(v);
		w.put<uint32_t>(r.seed);
		put_building(w, r.record);
	}
	w.put<uint32_t>(uint32_t(d.gone.size()));
	for (const Gone &g : d.gone) {
		w.put<uint8_t>(g.kind);
		w.put<float>(g.x);
		w.put<float>(g.z);
	}
	w.put<uint32_t>(uint32_t(d.hurt.size()));
	for (const Hurt &h : d.hurt) {
		w.put<uint32_t>(h.seed);
		for (float v : { h.x, h.z, h.amount }) w.put<float>(v);
	}
}

bool get_block(ByteReader &r, DamageState &d) {
	d.clear();
	d.strikes = r.get<uint32_t>();
	d.shots = r.get<uint32_t>();
	d.growth_hold_until = r.get<double>();
	uint32_t n = r.get<uint32_t>();
	if (!r.ok || n > SAVED_ENTRIES_MAX) return false;
	d.craters.resize(n);
	for (Crater &c : d.craters) {
		c.x = r.get<float>(); c.z = r.get<float>(); c.radius = r.get<float>(); c.depth = r.get<float>();
		c.hard = r.get<uint8_t>();
		c.seed = r.get<uint32_t>();
	}
	n = r.get<uint32_t>();
	if (!r.ok || n > SAVED_ENTRIES_MAX) return false;
	d.ruins.resize(n);
	for (Ruin &u : d.ruins) {
		u.x = r.get<float>(); u.z = r.get<float>(); u.w = r.get<float>(); u.d = r.get<float>(); u.rot = r.get<float>();
		u.seed = r.get<uint32_t>();
		if (!get_building(r, u.record)) return false;
	}
	n = r.get<uint32_t>();
	if (!r.ok || n > SAVED_ENTRIES_MAX) return false;
	d.gone.resize(n);
	for (Gone &g : d.gone) {
		g.kind = r.get<uint8_t>();
		g.x = r.get<float>();
		g.z = r.get<float>();
	}
	n = r.get<uint32_t>();
	if (!r.ok || n > SAVED_ENTRIES_MAX) return false;
	d.hurt.resize(n);
	for (Hurt &h : d.hurt) {
		h.seed = r.get<uint32_t>();
		h.x = r.get<float>(); h.z = r.get<float>(); h.amount = r.get<float>();
	}
	return r.ok;
}
} // namespace

void write_extra_blocks(ByteWriter &w, const CityState &s) {
	const DamageState &d = s.damage;
	if (d.craters.empty() && d.ruins.empty() && d.gone.empty() && d.hurt.empty() && d.strikes == 0 && d.shots == 0) return;
	ByteWriter block;
	put_block(block, d);
	w.put<uint32_t>(SAVE_TAG_DEMOLITION);
	w.put<uint32_t>(uint32_t(block.buf.size()));
	w.put_bytes(block.buf.data(), block.buf.size());
}

bool read_extra_blocks(ByteReader &r, CityState &s) {
	for (;;) {
		const uint32_t tag = r.get<uint32_t>();
		if (!r.ok) return false;
		if (tag == SAVE_END) return true;
		const uint32_t size = r.get<uint32_t>();
		if (!r.ok || r.o + size > r.n) return false;
		if (tag == SAVE_TAG_DEMOLITION) {
			ByteReader block{ r.p + r.o, size };
			if (!get_block(block, s.damage)) return false;
		}
		r.o += size; // a block this reader does not know is skipped by its size
	}
}

} // namespace city
