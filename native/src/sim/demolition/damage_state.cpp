// Queries and changes of the damage state: the revised lists change only here, each change raising their revisions, and the
// ruins are filed by place here as they change (ruin_at, ruin_under).
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
	file_ruin(ruins.size() - 1);
	return int(ruins.size()) - 1;
}

// The history replays last in first out, so a ruin comes back at the end and goes from the end: one filed or unfiled, O(1); one
// put back or taken from the middle moves the indices after it, and the ruins are filed again.
void DamageState::insert_ruin(size_t at, const Ruin &r) {
	const size_t i = std::min(at, ruins.size());
	const bool last = i == ruins.size();
	ruins.insert(i, r);
	if (last) file_ruin(i);
	else file_ruins();
}

void DamageState::erase_ruin(size_t at) {
	if (at >= ruins.size()) return;
	if (at + 1 == ruins.size()) unfile_ruin(at);
	ruins.erase(at);
	if (at != ruins.size()) file_ruins();
}

void DamageState::file_ruin(size_t i) {
	if (ruin_cells_.cells.empty()) ruin_cells_.cell = BUILDING_HASH_CELL; // lots, filed as the buildings' are
	float x0, z0, x1, z1;
	BuildingRecords::bounds(ruins[i].record, x0, z0, x1, z1); // the ruin's lot is its fallen record's
	ruin_cells_.insert_aabb(int(i), x0, z0, x1, z1);
}

void DamageState::unfile_ruin(size_t i) {
	float x0, z0, x1, z1;
	BuildingRecords::bounds(ruins[i].record, x0, z0, x1, z1);
	ruin_cells_.remove_aabb(int(i), x0, z0, x1, z1);
}

void DamageState::file_ruins() {
	ruin_cells_.clear();
	for (size_t i = 0; i < ruins.size(); i++) file_ruin(i);
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
	file_ruins();
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
	int found = -1;
	const float reach = pad * std::sqrt(2.f); // a turned lot grown by pad reaches past its box by up to pad times root 2
	ruin_cells_.query_unique(x - reach, z - reach, x + reach, z + reach, [&](int i) {
		if ((found < 0 || i < found) && BuildingRecords::covers(ruins[size_t(i)].record, x, z, pad)) found = i;
	});
	return found;
}

bool DamageState::ruin_under(float x, float z, float w, float d, float rot) const {
	Building lot; // the rectangle asked about, read as a lot is (BuildingRecords::covers and bounds)
	lot.x = x;
	lot.z = z;
	lot.w = w;
	lot.d = d;
	lot.rot = rot;
	float x0, z0, x1, z1;
	BuildingRecords::bounds(lot, x0, z0, x1, z1);
	bool hit = false;
	ruin_cells_.query_unique(x0, z0, x1, z1, [&](int i) {
		const Building &fallen = ruins[size_t(i)].record;
		hit = hit || BuildingRecords::covers(lot, fallen.x, fallen.z, 0.f) || BuildingRecords::covers(fallen, x, z, 0.f);
	});
	return hit;
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
	trip_step(s);
}

} // namespace city
