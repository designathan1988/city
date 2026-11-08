// What a round does to a walker (Roadcraft walk.ts `shot`, in this game's numbers: blast.json shot): the part struck takes
// its damage (the head kills, a trunk costs a third of the health, a limb a fifth); a limb that has taken enough, or is hit
// where its owner lies, comes off and costs a good share of the blood; the third round in the trunk opens the belly. Dead:
// the body falls. Hurt: a light wound in a leg (or the trunk, half the time) knocks them down for a moment, any other light
// wound only bends them over it; a grave one (half the health gone, or a limb lost) puts them on the ground for good,
// bleeding until they die. Everyone near a gunshot runs.
#include "sim/commands/shoot_internal.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SALT_OPEN = 0x6f70656eU, SALT_KNOCK = 0x6b6e6f63U, SALT_FALL = 0x66616c6cU;
constexpr float OPEN_SECOND_HIT_CHANCE = 0.4f; // the second round in the trunk opens the belly this often (the 4.0: 102 of 256)
constexpr float KNOCK_TRUNK_CHANCE = 0.5f; // a light round in the trunk throws the walker down this often
constexpr float PUSH_KILL = 0.25f, PUSH_KNOCK = 0.1f, PUSH_GRAVE = 0.15f, PUSH_SEVER = 0.35f; // how hard the body is pushed (0..1)
constexpr uint32_t HITS_SALT_SHIFT = 8; // the draw of a hit differs from the same walker's next
constexpr float LIE_FOR_GOOD_S = 600.f; // down until they bleed out
constexpr float BULLET_BACK_M = 10.f; // the push comes from this far behind the walker along the round
constexpr uint32_t LIMB_OF[6] = { CAS_LIMB_HEAD, 0, CAS_LIMB_ARM_L, CAS_LIMB_ARM_R, CAS_LIMB_LEG_L, CAS_LIMB_LEG_R };
constexpr int LIMB_INDEX[6] = { -1, -1, 0, 1, 2, 3 }; // into Pedestrian::hurt_limb

DamageEvent casualty_of(const Round &r, int id, const Pedestrian &w, uint32_t flags, float power, float lie, float hx, float hz) {
	DamageEvent e;
	e.kind = DamageEvent::CASUALTY;
	e.id = id;
	e.x = w.cur.x; e.y = w.cur.y; e.z = w.cur.z;
	e.a = std::atan2(w.cur.fz, w.cur.fx);
	e.b = hx - r.dx * BULLET_BACK_M;
	e.c = hz - r.dz * BULLET_BACK_M;
	e.d = power;
	e.dx = lie;
	e.u = w.look;
	e.v = flags | cas_age(int(w.age));
	return e;
}

} // namespace

void shoot_walker(Round &r, int id, BodyPart part, float hx, float hy, float hz, bool lying) {
	TrafficState &t = r.s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	const BlastParams &p = r.p;
	const int k = int(part);
	const float damage = part == BodyPart::HEAD ? p.damage_head : part == BodyPart::TORSO ? p.damage_torso : p.damage_limb;
	w.hp -= damage;
	w.hits++;
	if (part == BodyPart::HEAD) w.hurt_head += damage;
	else if (part == BodyPart::TORSO) w.hurt_torso += damage;
	else w.hurt_limb[LIMB_INDEX[k]] += damage;
	uint32_t severed = 0;
	if (LIMB_INDEX[k] >= 0 && (w.lost & LIMB_OF[k]) == 0 && (w.hurt_limb[LIMB_INDEX[k]] >= p.sever_hits_hp || lying)) {
		severed = LIMB_OF[k];
		w.lost = uint8_t(w.lost | severed);
		w.hp -= p.sever_hp_cost;
	}
	bool opened = false;
	if (part == BodyPart::TORSO && !w.opened) {
		const int trunk_hits = int(std::lround(w.hurt_torso / p.damage_torso));
		opened = trunk_hits >= p.open_hits || (trunk_hits == p.open_hits - 1 && r.roll(SALT_OPEN, uint32_t(id)) < OPEN_SECOND_HIT_CHANCE);
		w.opened = w.opened || opened;
	}
	DamageEvent wound;
	wound.kind = DamageEvent::WOUND;
	wound.id = id;
	wound.x = hx; wound.y = hy; wound.z = hz;
	wound.dx = r.dx; wound.dz = r.dz;
	wound.u = severed;
	r.emit(wound);
	r.out.hit = ShotResult::WALKER;
	r.out.id = id;
	const uint32_t shared = (uint32_t(w.lost) << CAS_LOST_SHIFT) | (severed << CAS_SEVERED_SHIFT) | ((uint32_t(k) + 1u) << CAS_STRUCK_SHIFT) | (opened ? CAS_OPENED : 0u);
	if (w.hp <= 0.f) {
		r.emit(casualty_of(r, id, w, CAS_DEAD | shared, PUSH_KILL, 0.f, hx, hz));
		r.out.killed = true;
		traffic::mourn_pedestrian(r.s, id);
		traffic::add_shock(r.s, w.cur.x, w.cur.z, r.s.catalog.traffic.ped_body_shock_m);
		traffic::remove_pedestrian(r.s, id);
		return;
	}
	const bool grave = w.hp < p.grave_hp || w.lost != 0;
	const float now = float(t.time);
	if (!lying && !grave) {
		const bool leg = part == BodyPart::LEG_L || part == BodyPart::LEG_R;
		if (leg || (part == BodyPart::TORSO && r.roll(SALT_KNOCK, uint32_t(id) ^ (uint32_t(w.hits) << HITS_SALT_SHIFT)) < KNOCK_TRUNK_CHANCE)) {
			const float lie = p.knock_min_s + p.knock_span_s * r.roll(SALT_FALL, uint32_t(id)); // as long down as a blast's (people.knock_*)
			w.down_until = now + lie;
			r.emit(casualty_of(r, id, w, CAS_KNOCKED | shared, PUSH_KNOCK, lie, hx, hz));
		} else {
			w.flinch_until = now + p.flinch_s;
		}
	} else {
		const bool leg_gone = (w.lost & (CAS_LIMB_LEG_L | CAS_LIMB_LEG_R)) != 0;
		w.down_until = now + LIE_FOR_GOOD_S;
		w.bleed = std::max(w.bleed, leg_gone ? p.bleed_leg : severed != 0 ? p.bleed_limb : p.bleed_hurt);
		r.emit(casualty_of(r, id, w, CAS_KNOCKED | CAS_CRAWL | shared, severed != 0 ? PUSH_SEVER : PUSH_GRAVE, LIE_FOR_GOOD_S, hx, hz));
	}
}

} // namespace city
