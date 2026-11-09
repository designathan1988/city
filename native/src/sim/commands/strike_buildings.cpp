// What a blow does to the buildings: each feels the blast at its standoff (blast_model.h felt_pressure: the peak, in part
// when the blow is over before the structure answers). What it feels adds to what it already took (DamageState::hurt):
// the share of the collapse limit (Glasstone: a house is demolished at 5 psi), so a small bomb must be repeated to bring
// a building down (the 4.0's "keep hitting"); at 1.0 it falls and leaves a ruin; past the structural limit it catches fire.
// A framed block answers at 20 psi, a house at 5 (Glasstone and Dolan 1977, as tabled by HYDESim and DTIC ADA072246). Close to the
// blow part of it breaks off first (destruction.ts `strike`): a scar, and when little would be left standing, the rest comes down.
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LOT_REACH_M = 80.f; // the widest lot: a centre this far outside the reach may still have a wall inside it
constexpr float STANDOFF_DEPTH = 0.5f; // the load on a wall is that at its distance plus half the building's depth
constexpr uint32_t SALT_FIRE = 0x66697265U; // a stream's salt: the word it names, in ASCII
constexpr float SPHERE_VOLUME = 4.18879f; // 4/3 of pi

// What the building feels: the blast at its distance taken to the middle of its depth.
float felt_by(const Blow &b, const Building &bd) {
	const float near = lot_distance(bd, b.x, b.z), depth = std::min(bd.w, bd.d) * STANDOFF_DEPTH;
	return blast::felt_pressure(b.p, blast::airblast(b.p, b.kg, std::hypot(near, depth)));
}

// Where and how hard a blow breaks a building (actionsWiring.ts explodeAt): at full force the one it landed on, else less with the
// distance, at the wall nearest the burst.
struct Aim {
	float x = 0, y = 0, z = 0, force = 0;
	bool breaks = false;
};

Aim aim_at(const Blow &b, const Building &bd, float near) {
	const Tuning &t = b.s.catalog.tune;
	const float radius = blast::fireball_radius(t, b.strength);
	const bool landed = b.on_building && near <= 0.f;
	const float c = std::cos(bd.rot), sn = std::sin(bd.rot), dx = b.x - bd.x, dz = b.z - bd.z;
	const float u = std::clamp(dx * c - dz * sn, -bd.w * 0.5f, bd.w * 0.5f), v = std::clamp(dx * sn + dz * c, -bd.d * 0.5f, bd.d * 0.5f);
	Aim a;
	a.x = landed ? b.x : bd.x + u * c + v * sn;
	a.z = landed ? b.z : bd.z - u * sn + v * c;
	a.y = landed ? b.height : std::max(b.height, b.ground + t.strike_scar_lift_m);
	a.force = landed ? b.strength : std::max(1.f, b.strength * (1.f - near / radius) * t.strike_scar_force_gain);
	a.breaks = near <= radius && (landed || near < radius * t.strike_scar_near_share || a.force >= t.strike_scar_force_least);
	return a;
}

// The share of a building its scars would leave: a cut across all of it takes everything above, a hole only what it reaches.
float standing_share(const Blow &b, const Building &bd) {
	const float height = std::max(1.f, float(bd.floors) * b.s.catalog.tune.strike_storey_m), half = std::min(bd.w, bd.d) * 0.5f;
	const float ground = b.s.terrain.height_at(bd.x, bd.z);
	float left = 1.f;
	b.s.damage.each_scar_of(bd.seed, bd.x, bd.z, [&](const Scar &c) {
		const float r = scar_reach_m(c.force);
		const float share = r >= half ? (c.y - r - ground) / height : 1.f - r * r * r * SPHERE_VOLUME / (bd.w * bd.d * height);
		left = std::min(left, std::max(share, 0.f));
	});
	return left;
}

int scars_on(const CityState &s, const Building &bd) {
	return s.damage.scar_count_of(bd.seed, bd.x, bd.z);
}

void bring_down(Blow &b, int id, const Aim &aim) {
	const Building rec = b.s.buildings.items[size_t(id)];
	Ruin ruin;
	ruin.x = rec.x; ruin.z = rec.z; ruin.w = rec.w; ruin.d = rec.d; ruin.rot = rec.rot; ruin.seed = rec.seed;
	ruin.record = rec;
	ruin.blow_x = aim.x;
	ruin.blow_y = aim.y;
	ruin.blow_z = aim.z;
	ruin.blow_force = aim.force;
	j_add_ruin(b.s, ruin);
	const uint32_t ruin_index = uint32_t(b.s.damage.ruins.size() - 1); // the event names the ruin it brought (damage_event.h)
	j_remove_building(b.s, id);
	j_set_hurt(b.s, rec.seed, rec.x, rec.z, 0.f);
	auto &fires = b.s.damage.fires;
	fires.erase(std::remove_if(fires.begin(), fires.end(), [&](const Fire &f) { return f.building == id; }), fires.end());
	b.result.buildings_removed++;
	DamageEvent e;
	e.kind = DamageEvent::BUILDING_DOWN;
	e.id = id;
	e.x = rec.x; e.z = rec.z; e.y = b.ground;
	e.a = b.strength;
	e.u = ruin_index;
	b.emit(e);
}

void ignite(Blow &b, int id, const Building &rec) {
	auto &fires = b.s.damage.fires;
	if (int(fires.size()) >= b.p.fire_max) return;
	for (const Fire &f : fires)
		if (f.building == id) return;
	fires.push_back({ id, rec.x, rec.z, b.s.damage.now + double(b.p.fire_min_s + b.p.fire_span_s * b.roll(SALT_FIRE, uint32_t(id))) });
	DamageEvent e;
	e.kind = DamageEvent::IGNITE;
	e.id = id;
	e.x = rec.x; e.z = rec.z;
	b.emit(e);
}

// Soot and wear: the building looks older by as much as the blow aged it.
void wear(Blow &b, int id, float pressure) {
	const BlastParams &p = b.p;
	const float share = std::min(1.f, (pressure - p.light_kpa) / (p.structural_kpa - p.light_kpa));
	Building after = b.s.buildings.items[size_t(id)];
	after.built -= double(p.wear_gain * share) * double(b.s.catalog.growth.decay_span_min);
	j_edit_building(b.s, id, after);
}
} // namespace

void strike_buildings(Blow &b) {
	const BlastParams &p = b.p;
	const float reach = blast::range_at_pressure(p, b.kg, p.light_kpa) + LOT_REACH_M;
	std::vector<int> ids;
	b.s.buildings.query(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	for (int id : ids) {
		const Building bd = b.s.buildings.items[size_t(id)];
		if (!bd.alive || bd.floors == 0) continue; // an empty lot has nothing to break
		const float felt = felt_by(b, bd);
		if (felt < p.light_kpa) continue;
		const Tuning &t = b.s.catalog.tune;
		const int at = b.s.damage.hurt_of(bd.seed, bd.x, bd.z);
		const float limit = bd.floors >= t.strike_frame_floors ? t.strike_frame_collapse_kpa : p.collapse_kpa;
		const float taken = (at >= 0 ? b.s.damage.hurt[size_t(at)].amount : 0.f) + felt / limit;
		const Aim aim = aim_at(b, bd, lot_distance(bd, b.x, b.z));
		const bool scarred = aim.breaks && scars_on(b.s, bd) < t.strike_scars_most;
		if (scarred) j_add_scar(b.s, Scar{ bd.seed, bd.x, bd.z, aim.x, aim.y, aim.z, aim.force });
		if (taken >= 1.f || standing_share(b, bd) < t.strike_standing_least) {
			bring_down(b, id, aim);
			continue;
		}
		j_set_hurt(b.s, bd.seed, bd.x, bd.z, taken);
		if (scarred) {
			DamageEvent cut;
			cut.kind = DamageEvent::BUILDING_SCAR;
			cut.id = id;
			b.emit(cut);
		}
		DamageEvent e;
		e.kind = DamageEvent::BUILDING_HIT;
		e.id = id;
		e.x = std::clamp(b.x, bd.x - bd.w, bd.x + bd.w); e.z = std::clamp(b.z, bd.z - bd.d, bd.z + bd.d); e.y = b.ground;
		e.a = felt;
		e.b = taken;
		b.emit(e);
		wear(b, id, felt);
		if (felt >= p.structural_kpa) ignite(b, id, b.s.buildings.items[size_t(id)]);
	}
}

} // namespace city
