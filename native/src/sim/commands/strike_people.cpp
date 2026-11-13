// What a blow does to the people in the street: each walker feels the peak overpressure where they stand (Glasstone's
// table 12.38 as log-normal probits, blast_model.h). A draw of the blow's own stream (the same blow, the same dead)
// says whether they die, are thrown down (eardrum), or only run from the bang. The vehicles the blast reaches leave
// the traffic as burning wrecks, their drivers thrown.
#include "sim/commands/strike_internal.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SALT_DEATH = 0x64656174U;
constexpr uint32_t SALT_EAR = 0x65617273U;
constexpr uint32_t SALT_LIMBS = 0x6c696d62U;
constexpr uint32_t SALT_LIE = 0x6c696573U;
constexpr uint32_t SALT_SEVER = 0x73657665U;
constexpr uint32_t SALT_MAIM = 0x6d61696dU;
constexpr uint32_t SALT_BODY = 0x626f6479U;
constexpr int LIMB_KINDS = 5; // head, two arms, two legs
constexpr float THROW_FLOOR = 0.3f; // the least share of the throw a body at the edge of the blast gets: it still moves a little
constexpr float DEATH_REACH_SHARE = 1.1f; // vehicles are blown within this share of the range of their limit
constexpr float FAR_DIE_HEAD = 0.25f; // a killed body further out loses its head with this chance
constexpr uint32_t TORN_BIT = 1;
constexpr float TORN_NEAR_SHARE = 0.6f; // the drivers torn apart sit within this share of the range that blows their car
constexpr int CHANCE_BITS = 8;
constexpr float BYTE_RANGE = 256.f;

float heading_of(const Pose &p) { return std::atan2(p.fz, p.fx); }

uint32_t limb_bit(int k) { return 1u << uint32_t(k); }

// The limbs a killed or torn body loses to the blast.
uint32_t severed_limbs(const Blow &b, uint32_t who, bool torn) {
	const uint32_t roll = b.bits(SALT_LIMBS, who);
	uint32_t mask = 0;
	if (torn) {
		for (int k = 0; k < LIMB_KINDS; k++)
			if (((roll >> uint32_t(k)) & TORN_BIT) != 0) mask |= limb_bit(k);
		return mask | limb_bit(int((roll >> CHANCE_BITS) % uint32_t(LIMB_KINDS)));
	}
	if (b.roll(SALT_SEVER, who) < b.p.sever_chance) {
		mask |= limb_bit(1 + int((roll >> CHANCE_BITS) % uint32_t(LIMB_KINDS - 1))); // an arm or a leg
		if (float(roll & uint32_t(BYTE_RANGE - 1)) / BYTE_RANGE < FAR_DIE_HEAD) mask |= limb_bit(0);
	}
	return mask;
}

DamageEvent casualty(const Blow &b, int id, const Pose &at, uint32_t look, float pressure, uint32_t flags, float lie) {
	DamageEvent e;
	e.kind = DamageEvent::CASUALTY;
	e.id = id;
	e.x = at.x; e.y = at.y; e.z = at.z;
	e.a = heading_of(at);
	e.b = b.x; e.c = b.z;
	e.d = std::min(1.f, std::max(THROW_FLOOR, pressure / b.p.torn_kpa));
	e.dx = lie;
	e.u = look;
	e.v = flags;
	return e;
}

void harm_walker(Blow &b, int id) {
	TrafficState &t = b.s.traffic;
	const Pedestrian &w = t.peds[size_t(id)];
	const float r = std::hypot(w.cur.x - b.x, w.cur.z - b.z), p = b.pressure(r);
	const uint32_t who = uint32_t(id);
	const Pose at = w.cur;
	const uint32_t look = w.look;
	if (b.roll(SALT_DEATH, who) < blast::lethality(b.p, p)) {
		const bool torn = p >= b.p.torn_kpa;
		const uint32_t lost = severed_limbs(b, who, torn);
		const uint32_t flags = (torn ? CAS_TORN : CAS_DEAD) | (lost << CAS_SEVERED_SHIFT) | (lost << CAS_LOST_SHIFT) | CAS_CHARRED;
		const DamageEvent dead = casualty(b, id, at, look, p, flags, 0.f);
		b.emit(dead);
		traffic::lay_dead(b.s, id, dead); // the body lies on where the blast throws it down
		return;
	}
	if (b.roll(SALT_EAR, who) < blast::eardrum(b.p, p)) {
		uint32_t flags = CAS_KNOCKED, lost = 0;
		float lie = b.p.knock_min_s + b.p.knock_span_s * b.roll(SALT_LIE, who);
		if (p >= b.p.ear_half_kpa && b.roll(SALT_MAIM, who) < b.p.maim_chance) { // near the blow some lose an arm or a leg
			// a limb lost is a grave wound, a blast's as a round's (wound_for_good): the walker stays, the limb gone with its blood's cost,
			// down for good, bleeding (a leg the worst) until they die of it, where the body lies (Pwalkers-13, 16: the walker was taken
			// away, or kept the limb)
			lost = limb_bit(1 + int(b.bits(SALT_MAIM, who) % uint32_t(LIMB_KINDS - 1)));
			Pedestrian &m = t.peds[size_t(id)];
			m.lost = uint8_t(m.lost | lost);
			m.hp -= b.p.sever_hp_cost;
			flags |= (uint32_t(m.lost) << CAS_LOST_SHIFT) | (lost << CAS_SEVERED_SHIFT) | CAS_CRAWL;
			const DamageEvent maimed = casualty(b, id, at, look, p, flags, LIE_FOR_GOOD_S);
			wound_for_good(b.s, m, lost, maimed);
			b.emit(maimed);
			return;
		}
		const DamageEvent down = casualty(b, id, at, look, p, flags, lie);
		float fx = 0, fz = 0;
		fall_way(down, fx, fz); // the way the render throws the body
		traffic::lay_down(t, t.peds[size_t(id)], lie, fx, fz);
		b.emit(down);
	}
	traffic::frighten_pedestrian(b.s, id, b.p.panic_s, b.x, b.z, true);
}
} // namespace

void strike_people(Blow &b) {
	const float reach = blast::startle_range(b.p, b.kg);
	const float reach2 = reach * reach;
	TrafficState &t = b.s.traffic;
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float dx = w.cur.x - b.x, dz = w.cur.z - b.z;
		if (dx * dx + dz * dz <= reach2) harm_walker(b, id);
	}
	// the dead lying where the blast throws the living down (the eardrum's threshold): their bodies thrown again, a CASUALTY of the body's
	// own id that the render's body takes (BodyRenderer::strike); where they lie on stays the simulation's record of them
	const float thrown = blast::range_at_pressure(b.p, b.kg, b.p.ear_threshold_kpa);
	for (const DeadBody &d : t.dead) {
		const float r = std::hypot(d.x - b.x, d.z - b.z);
		if (r > thrown) continue;
		Pose at;
		at.x = d.x; at.y = d.y; at.z = d.z;
		at.fx = d.fall_x; at.fz = d.fall_z;
		b.emit(casualty(b, d.id, at, d.look, b.pressure(r), CAS_DEAD | (uint32_t(d.lost) << CAS_LOST_SHIFT), 0.f));
	}
	traffic::add_shock(b.s, b.x, b.z, std::min(reach, b.s.catalog.traffic.ped_blast_shock_m));
}

void strike_traffic(Blow &b) {
	const float reach = blast::range_at_pressure(b.p, b.kg, b.p.vehicle_kpa) * DEATH_REACH_SHARE;
	TrafficState &t = b.s.traffic;
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive) continue;
		const float d = std::hypot(v.cur.x - b.x, v.cur.z - b.z);
		if (d > reach) continue;
		const uint32_t type = uint32_t(v.type) | (uint32_t(v.paint) << VEHICLE_PAINT_SHIFT);
		DamageEvent e;
		e.kind = DamageEvent::VEHICLE_BLOWN;
		e.id = id;
		e.x = v.cur.x; e.y = v.cur.y; e.z = v.cur.z;
		e.a = heading_of(v.cur);
		e.u = type;
		b.emit(e);
		const float p = b.pressure(d);
		const bool torn = b.roll(SALT_BODY, uint32_t(id)) < 0.5f && d < reach * TORN_NEAR_SHARE / DEATH_REACH_SHARE;
		b.emit(casualty(b, VEHICLE_BODY_ID_BASE + id, v.cur, 0, p, (torn ? CAS_TORN : CAS_DEAD) | CAS_CHARRED, 0.f));
		traffic::despawn_vehicle(b.s, id, false);
	}
}

} // namespace city
