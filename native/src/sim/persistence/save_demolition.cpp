// The demolition block of a save (save_demolition.h).
#include "sim/persistence/save_demolition.h"

#include "sim/persistence/save_format.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace city {

namespace {
// The bytes of an entry of each table (put_block below writes them): what a count read is checked against.
constexpr size_t CRATER_BYTES = 4 * sizeof(float) + sizeof(uint8_t) + sizeof(uint32_t);
constexpr size_t RUIN_BYTES = 5 * sizeof(float) + sizeof(uint32_t) + BUILDING_SAVE_BYTES;
constexpr size_t GONE_BYTES = sizeof(uint8_t) + 2 * sizeof(float);
constexpr size_t HURT_BYTES = sizeof(uint32_t) + 3 * sizeof(float);
constexpr size_t SCAR_BYTES = sizeof(uint32_t) + 6 * sizeof(float);

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
	for (const Ruin &r : d.ruins)
		for (float v : { r.blow_x, r.blow_y, r.blow_z, r.blow_force }) w.put<float>(v);
	w.put<uint32_t>(uint32_t(d.scars.size()));
	for (const Scar &c : d.scars) {
		w.put<uint32_t>(c.seed);
		for (float v : { c.bx, c.bz, c.x, c.y, c.z, c.force }) w.put<float>(v);
	}
	for (const Crater &c : d.craters) w.put<float>(c.y);
}

bool get_block(ByteReader &r, DamageState &d) {
	d.clear();
	d.strikes = r.get<uint32_t>();
	d.shots = r.get<uint32_t>();
	d.growth_hold_until = r.get<double>();
	uint32_t n = r.get_count(CRATER_BYTES);
	if (!r.ok) return false;
	std::vector<Crater> craters(n); // the revised lists are read aside and handed to DamageState::restore whole
	for (Crater &c : craters) {
		c.x = r.get<float>(); c.z = r.get<float>(); c.radius = r.get<float>(); c.depth = r.get<float>();
		c.hard = r.get<uint8_t>();
		c.seed = r.get<uint32_t>();
	}
	n = r.get_count(RUIN_BYTES);
	if (!r.ok) return false;
	std::vector<Ruin> ruins(n);
	for (Ruin &u : ruins) {
		u.x = r.get<float>(); u.z = r.get<float>(); u.w = r.get<float>(); u.d = r.get<float>(); u.rot = r.get<float>();
		u.seed = r.get<uint32_t>();
		if (!get_building(r, u.record)) return false;
	}
	n = r.get_count(GONE_BYTES);
	if (!r.ok) return false;
	d.gone.resize(n);
	for (Gone &g : d.gone) {
		g.kind = r.get<uint8_t>();
		g.x = r.get<float>();
		g.z = r.get<float>();
	}
	n = r.get_count(HURT_BYTES);
	if (!r.ok) return false;
	d.hurt.resize(n);
	for (Hurt &h : d.hurt) {
		h.seed = r.get<uint32_t>();
		h.x = r.get<float>(); h.z = r.get<float>(); h.amount = r.get<float>();
	}
	for (Ruin &u : ruins) { // a save from before this has none: the block ends here, and each ruin falls from its middle
		if (r.o >= r.n) {
			u.blow_x = u.x;
			u.blow_z = u.z;
			continue;
		}
		u.blow_x = r.get<float>(); u.blow_y = r.get<float>(); u.blow_z = r.get<float>(); u.blow_force = r.get<float>();
	}
	std::vector<Scar> scars;
	if (r.o < r.n) { // a save from before scars ends here: every standing building is whole
		n = r.get_count(SCAR_BYTES);
		if (!r.ok) return false;
		scars.resize(n);
		for (Scar &c : scars) {
			c.seed = r.get<uint32_t>();
			c.bx = r.get<float>(); c.bz = r.get<float>(); c.x = r.get<float>(); c.y = r.get<float>(); c.z = r.get<float>(); c.force = r.get<float>();
		}
	}
	if (r.o < r.n) // a save from before the craters' heights ends here: each lies in the ground under it
		for (Crater &c : craters) c.y = r.get<float>();
	if (!r.ok) return false;
	d.restore(std::move(craters), std::move(ruins), std::move(scars));
	return true;
}
} // namespace

bool write_damage_block(ByteWriter &w, const DamageState &d) {
	if (d.craters.empty() && d.ruins.empty() && d.gone.empty() && d.hurt.empty() && d.strikes == 0 && d.shots == 0) return false;
	put_block(w, d);
	return true;
}

bool read_damage_block(ByteReader &r, DamageState &d) { return get_block(r, d); }

} // namespace city
