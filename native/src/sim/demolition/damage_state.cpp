// Queries of the damage state.
#include "sim/demolition/damage_state.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>
#include <cstddef>

namespace city {

namespace {
constexpr double INDEX_ROUNDING_ULPS = 32;
bool identity_ok(float x, float z) { return !std::isnan(x) && !std::isnan(z); }
Bounds2 region(float x, float z, double halfx, double halfz) {
	const double error = INDEX_ROUNDING_ULPS * std::numeric_limits<float>::epsilon() *
		(1 + std::abs(double(x)) + std::abs(double(z)) + std::abs(halfx) + std::abs(halfz));
	return {double(x) - halfx - error, double(z) - halfz - error, double(x) + halfx + error, double(z) + halfz + error};
}
Bounds2 ruin_bounds(const Ruin &r) {
	const float c = std::cos(r.rot), s = std::sin(r.rot);
	const double norm2 = double(c) * c + double(s) * s;
	const double hx = (std::abs(double(c)) * r.w + std::abs(double(s)) * r.d) * 0.5 / norm2;
	const double hz = (std::abs(double(s)) * r.w + std::abs(double(c)) * r.d) * 0.5 / norm2;
	if (!std::isfinite(hx) || !std::isfinite(hz) || !std::isfinite(r.x) || !std::isfinite(r.z)) {
		const double inf = std::numeric_limits<double>::infinity();
		return {-inf, -inf, inf, inf};
	}
	return region(r.x, r.z, std::max(hx, 0.0), std::max(hz, 0.0));
}
}

void DamageState::rebuild_ruins() {
	ruin_index_.reset();
	for (size_t i = 0; i < records_.ruins.size(); i++) ruin_index_.insert(int(i), ruin_bounds(records_.ruins[i]));
}
void DamageState::rebuild_hurt() {
	hurt_rows_.clear();
	for (size_t i = 0; i < records_.hurt.size(); i++) {
		const Hurt &h = records_.hurt[i];
		if (identity_ok(h.x, h.z)) hurt_rows_[{h.seed, h.x, h.z}].push_back(int(i));
	}
}
void DamageState::restore(DamageRecords records) {
	assert(records.ruin_bodies.size() == records.ruins.size());
	records_ = std::move(records);
	rebuild_ruins(); rebuild_hurt();
	scar_rows_.clear();
	for (size_t i = 0; i < records_.scars.size(); i++) {
		const Scar &c = records_.scars[i];
		if (identity_ok(c.bx, c.bz)) scar_rows_[{c.seed, c.bx, c.bz}].push_back(int(i));
	}
	for (auto &index : gone_index_) index.clear();
	for (size_t i = 0; i < records_.gone.size(); i++) {
		const Gone &g = records_.gone[i];
		if (g.kind < gone_index_.size() && std::isfinite(g.x) && std::isfinite(g.z))
			gone_index_[g.kind].insert(int(i), region(g.x, g.z, 0, 0));
	}
	ruins_revision = next_generation(); scars_revision = next_generation();
}
void DamageState::insert_ruin(size_t at, const Ruin &ruin, b40::BodyRef body) {
	at = std::min(at, records_.ruins.size());
	const bool append = at == records_.ruins.size();
	records_.ruins.insert(records_.ruins.begin() + std::ptrdiff_t(at), ruin);
	records_.ruin_bodies.insert(records_.ruin_bodies.begin() + std::ptrdiff_t(at), std::move(body));
	if (append) ruin_index_.insert(int(at), ruin_bounds(ruin)); else rebuild_ruins();
	ruins_revision = next_generation();
}
void DamageState::remove_ruin(size_t at) {
	if (at >= records_.ruins.size()) return;
	records_.ruins.erase(records_.ruins.begin() + std::ptrdiff_t(at));
	records_.ruin_bodies.erase(records_.ruin_bodies.begin() + std::ptrdiff_t(at));
	rebuild_ruins(); ruins_revision = next_generation();
}
void DamageState::add_gone(const Gone &g) {
	const int id = int(records_.gone.size());
	records_.gone.push_back(g);
	if (g.kind < gone_index_.size() && std::isfinite(g.x) && std::isfinite(g.z))
		gone_index_[g.kind].insert(id, region(g.x, g.z, 0, 0));
}
void DamageState::pop_gone() {
	if (records_.gone.empty()) return;
	const Gone &g = records_.gone.back();
	if (g.kind < gone_index_.size() && std::isfinite(g.x) && std::isfinite(g.z))
		gone_index_[g.kind].erase(int(records_.gone.size() - 1), region(g.x, g.z, 0, 0));
	records_.gone.pop_back();
}
void DamageState::add_scar(const Scar &c) {
	const int id = int(records_.scars.size());
	records_.scars.push_back(c);
	if (identity_ok(c.bx, c.bz)) scar_rows_[{c.seed, c.bx, c.bz}].push_back(id);
	scars_revision = next_generation();
}
void DamageState::pop_scar() {
	if (records_.scars.empty()) return;
	const Scar &c = records_.scars.back();
	if (identity_ok(c.bx, c.bz)) {
		const auto it = scar_rows_.find({c.seed, c.bx, c.bz});
		assert(it != scar_rows_.end());
		it->second.pop_back(); if (it->second.empty()) scar_rows_.erase(it);
	}
	records_.scars.pop_back(); scars_revision = next_generation();
}
const std::vector<int> &DamageState::scar_ids(uint32_t seed, float x, float z) const {
	static const std::vector<int> none;
	if (!identity_ok(x, z)) return none;
	const auto it = scar_rows_.find({seed, x, z});
	return it == scar_rows_.end() ? none : it->second;
}
int DamageState::hurt_of(uint32_t seed, float x, float z) const {
	if (!identity_ok(x, z)) return -1;
	const auto it = hurt_rows_.find({seed, x, z});
	return it == hurt_rows_.end() ? -1 : it->second.front();
}
void DamageState::set_hurt(const Hurt &h, float amount) {
	const int at = hurt_of(h.seed, h.x, h.z);
	if (amount <= 0) {
		if (at >= 0) { records_.hurt.erase(records_.hurt.begin() + at); rebuild_hurt(); }
		return;
	}
	Hurt next = h; next.amount = amount;
	if (at >= 0) records_.hurt[size_t(at)] = next;
	else {
		const int id = int(records_.hurt.size());
		records_.hurt.push_back(next);
		if (identity_ok(h.x, h.z)) hurt_rows_[{h.seed, h.x, h.z}].push_back(id);
	}
}
std::vector<int> DamageState::ruins_near(float x, float z, float reach) const {
	std::vector<int> ids;
	ruin_index_.query(region(x, z, std::abs(double(reach)), std::abs(double(reach))), [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	return ids;
}
int DamageState::ruin_at(float x, float z, float pad) const {
	for (int i : ruins_near(x, z, 2 * std::abs(pad))) {
		const Ruin &r = records_.ruins[size_t(i)];
		const float c = std::cos(r.rot), sn = std::sin(r.rot), dx = x - r.x, dz = z - r.z;
		if (std::fabs(dx * c - dz * sn) < r.w / 2 + pad && std::fabs(dx * sn + dz * c) < r.d / 2 + pad) return i;
	}
	return -1;
}
bool DamageState::is_gone(GoneKind kind, float x, float z, float reach) const {
	if (size_t(kind) >= gone_index_.size() || reach < 0) return false;
	const Bounds2 box = region(x, z, reach, reach);
	bool found = false;
	std::vector<BoundsIndex::OrderedVisit> scratch;
	gone_index_[size_t(kind)].query_ordered([&](const Bounds2 &b) { return b.overlaps(box); }, [&](int id) {
		const Gone &g = records_.gone[size_t(id)];
		found = std::fabs(g.x - x) <= reach && std::fabs(g.z - z) <= reach;
		return !found;
	}, scratch);
	return found;
}

void damage_step(CityState &s, double dt) {
	DamageState &d = s.damage;
	d.now += dt;
	d.fires.erase(std::remove_if(d.fires.begin(), d.fires.end(), [&](const Fire &f) { return d.now >= f.until; }), d.fires.end());
	bleed_step(s, dt);
}

} // namespace city
