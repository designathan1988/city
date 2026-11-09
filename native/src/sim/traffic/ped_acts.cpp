// What somebody waiting at a kerb does (citizenGait.ts standingKey): a while there, some look about, some take out a phone, put on headphones or
// go through the bag; a child only looks. A party talks, one at a time, the turn passing round, in its own mood: one party in eleven quarrels
// (the speaker argues, the others listen angry), and in one in four of the rest the speaker laughs on one beat in five. The act is eased in
// and out, as the walk is into standing, and only changes by way of nothing.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr uint32_t PHONE_MASK = 3, PHONE_HASH = 0, LOOK_HASH = 1; // a quarter of the people each (the look hash's low bits)
constexpr uint32_t HANDS_MASK = 7, HEADPHONES_HASH = 5, BAG_HASH = 6; // an eighth each
constexpr int PARTY_SPREAD = 7; // parties start their turns apart
// standingKey: a party's mood is its hash over 11, 3 the quarrel; the laugh where the mood over 4 is 1, on beat 2 of 5 beats of 4.3 s
constexpr uint32_t MOODS = 11, QUARREL = 3, LAUGH_MOODS = 4, LAUGH_MOOD = 1;
constexpr int BEATS = 5, LAUGH_BEAT = 2;
constexpr float BEAT_S = 4.3f;

PedAct wanted(const TrafficState &t, const TrafficParams &p, const Pedestrian &w, const PedParty &party) {
	if (t.time < w.held_until) return w.held_act == PedAct::CROUCH && w.rest < 1.f ? PedAct::NONE : w.held_act; // the crouch starts from standing
	const Pedestrian &l = *party.leader;
	if (l.mode != PedMode::WAIT) return PedAct::NONE;
	const float waited = l.wait;
	if (party.size > 1) {
		if (waited <= p.ped_talk_after_s) return PedAct::NONE;
		const int turn = int((waited + float(party.id % PARTY_SPREAD) * p.ped_talk_stagger_s) / p.ped_talk_turn_s) % party.size;
		const uint32_t mood = l.look % MOODS;
		if (turn != party.rank) return mood == QUARREL ? PedAct::ANGRY : PedAct::LISTEN;
		const int beat = int(waited / BEAT_S) % BEATS;
		return mood == QUARREL ? PedAct::ARGUE : mood % LAUGH_MOODS == LAUGH_MOOD && beat == LAUGH_BEAT ? PedAct::LAUGH : PedAct::TALK;
	}
	const bool adult = w.age != PedAge::CHILD;
	if (adult && waited > p.ped_hands_after_s && (w.look & HANDS_MASK) == HEADPHONES_HASH) return PedAct::HEADPHONES;
	if (adult && waited > p.ped_hands_after_s && (w.look & HANDS_MASK) == BAG_HASH) return PedAct::BAG;
	if ((w.look & PHONE_MASK) == LOOK_HASH && waited > p.ped_look_after_s && waited <= p.ped_look_after_s + p.ped_look_hold_s) return PedAct::LOOK;
	if (adult && (w.look & PHONE_MASK) == PHONE_HASH && waited > p.ped_phone_after_s) return PedAct::PHONE;
	return PedAct::NONE;
}

} // namespace

void step_act(const TrafficState &t, const TrafficParams &p, Pedestrian &w, float dt) {
	if (w.mode != PedMode::WAIT) w.wait = 0;
	const PedAct want = wanted(t, p, w, party_of(t, w));
	const float step = dt / p.ped_act_fade_s;
	if (want == w.act) {
		w.act_share = want == PedAct::NONE ? 0.f : std::min(1.f, w.act_share + step);
		return;
	}
	w.act_share -= step;
	if (w.act_share > 0) return;
	w.act_share = 0;
	w.act = want;
	w.act_from = t.time;
}

} // namespace traffic
} // namespace city
