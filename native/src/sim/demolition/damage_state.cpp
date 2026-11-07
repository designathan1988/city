// Queries and changes of the damage state: the revised lists change only here, each change raising their revisions.
#include "sim/demolition/damage_state.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace city {

void DamageState::clear() {
	DamageState fresh;
	fresh.craters.follow(craters);
	fresh.ruins.follow(ruins);
	fresh.scars.follow(scars);
	*this = std::move(fresh);
}

int DamageState::add_ruin(const Ruin &r) {
	ruins.push(r);
	return int(ruins.size()) - 1;
}

void DamageState::add_scar(const Scar &c) {
	scars_by_seed_[c.seed].push_back(uint32_t(scars.size()));
	scars.push(c);
}

void DamageState::drop_last_scar() {
	if (scars.empty()) return;
	const auto it = scars_by_seed_.find(scars[scars.size() - 1].seed);
	if (it != scars_by_seed_.end()) { // the last scar is the last of its building's: the list is in the order of the blows
		it->second.pop_back();
		if (it->second.empty()) scars_by_seed_.erase(it);
	}
	scars.pop();
}

void DamageState::restore(std::vector<Crater> craters_read, std::vector<Ruin> ruins_read, std::vector<Scar> scars_read) {
	craters.replace(std::move(craters_read));
	ruins.replace(std::move(ruins_read));
	scars.replace(std::move(scars_read));
	index_scars();
}

void DamageState::index_scars() {
	scars_by_seed_.clear();
	for (size_t i = 0; i < scars.size(); i++) scars_by_seed_[scars[i].seed].push_back(uint32_t(i));
}

int DamageState::scar_count_of(uint32_t seed, float bx, float bz) const {
	int n = 0;
	each_scar_of(seed, bx, bz, [&](const Scar &) { n++; });
	return n;
}

int DamageState::ruin_at(float x, float z, float pad) const {
	for (int i = 0; i < int(ruins.size()); i++) {
		const Ruin &r = ruins[size_t(i)];
		const float c = std::cos(r.rot), sn = std::sin(r.rot), dx = x - r.x, dz = z - r.z;
		if (std::fabs(dx * c - dz * sn) < r.w / 2 + pad && std::fabs(dx * sn + dz * c) < r.d / 2 + pad) return i;
	}
	return -1;
}

int DamageState::hurt_of(uint32_t seed, float x, float z) const {
	for (int i = 0; i < int(hurt.size()); i++)
		if (hurt[size_t(i)].seed == seed && hurt[size_t(i)].x == x && hurt[size_t(i)].z == z) return i;
	return -1;
}

bool DamageState::is_gone(GoneKind kind, float x, float z, float reach) const {
	for (const Gone &g : gone)
		if (g.kind == uint8_t(kind) && std::fabs(g.x - x) <= reach && std::fabs(g.z - z) <= reach) return true;
	return false;
}

void damage_step(CityState &s, double dt) {
	DamageState &d = s.damage;
	d.now += dt;
	d.fires.erase(std::remove_if(d.fires.begin(), d.fires.end(), [&](const Fire &f) { return d.now >= f.until; }), d.fires.end());
	bleed_step(s, dt);
}

} // namespace city
