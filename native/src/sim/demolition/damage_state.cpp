// Queries of the damage state.
#include "sim/demolition/damage_state.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

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
}

} // namespace city
