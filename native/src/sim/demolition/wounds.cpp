// The wounded bleed: each second their health drops by what the wound takes, and when it is gone they die where they stand
// or lie (run by the damage clock under the exclusive map lock, like the blows and the shots).
#include "sim/demolition/damage_event.h"
#include "sim/demolition/damage_state.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

namespace city {

DamageEvent walker_casualty(const Pedestrian &w, int id, uint32_t flags) {
	DamageEvent e;
	e.kind = DamageEvent::CASUALTY;
	e.id = id;
	e.x = w.cur.x; e.y = w.cur.y; e.z = w.cur.z;
	e.a = std::atan2(w.cur.fz, w.cur.fx);
	e.u = w.look;
	e.v = flags;
	return e;
}

void bleed_step(CityState &s, double dt) {
	TrafficState &t = s.traffic;
	for (int id = 0; id < int(t.peds.size()); id++) {
		Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float rate = s.catalog.blast.drips_per_s;
		if (w.lost != 0 && std::floor(s.damage.now * rate) != std::floor((s.damage.now - dt) * rate)) { // a trail behind who lost a limb, as the 4.0's onBleed
			DamageEvent drop;
			drop.kind = DamageEvent::DRIP;
			drop.x = w.cur.x; drop.y = w.cur.y; drop.z = w.cur.z;
			s.log.damage_events.push_back(drop);
		}
		if (w.bleed <= 0.f) continue;
		w.hp -= w.bleed * float(dt);
		if (w.hp > 0.f) continue;
		// CAS_BLED: the body already lies on the ground, the render adds only the blood
		DamageEvent e = walker_casualty(w, id, CAS_DEAD | CAS_BLED | (uint32_t(w.lost) << CAS_LOST_SHIFT));
		e.b = w.cur.x; e.c = w.cur.z;
		s.log.damage_events.push_back(e);
		traffic::add_shock(s, w.cur.x, w.cur.z, s.catalog.traffic.ped_body_shock_m);
		traffic::lay_dead(s, id, e); // their body lies on where they lay
	}
}

} // namespace city
