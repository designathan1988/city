// The dead bodies lying in the street (ped_system.h lay_dead): a walker who dies leaves the crowd and their body stays where it fell, as the
// unconscious stay down among a crowd that dodges them as inanimate bodies (Cornes, Frank, Dorso 2017, https://arxiv.org/pdf/1701.07358); the
// reference keeps them in its render and shoots them there (actionsWiring.ts shootBody), the render here only reads, so the simulation keeps
// them: a body the rounds meet, a blast throws and the crowd goes round, the first to die going first past blast.shot dead_kept.
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {
// Tells the render (the ChangeLog's damage events, as the wounds do): BODY_DEAD names the body by its new id, BODY_GONE lets it go.
void tell(CityState &s, DamageEvent::Kind kind, int id, uint32_t u) {
	DamageEvent e;
	e.kind = kind;
	e.id = id;
	e.u = u;
	s.log.damage_events.push_back(e);
}
} // namespace

float dead_half(const BlastParams &b, const DeadBody &d) { return b.body_height_m * d.scale * 0.5f; }

V2 dead_middle(const BlastParams &b, const DeadBody &d) { return V2(d.x, d.z) + V2(d.fall_x, d.fall_z) * dead_half(b, d); }

CrowdEntry dead_entry(const BlastParams &b, const DeadBody &d) {
	CrowdEntry e;
	e.sidewalk = d.side >= 0 ? d.seg * 2 + d.side : -1;
	e.s = d.s;
	e.u = d.u;
	e.x = d.x;
	e.z = d.z;
	e.fx = d.fall_x;
	e.fz = d.fall_z;
	e.lie = dead_half(b, d);
	e.lie_x = d.fall_x;
	e.lie_z = d.fall_z;
	return e;
}

void lay_dead(CityState &s, int id, const DamageEvent &e) {
	TrafficState &t = s.traffic;
	if (id < 0 || id >= int(t.peds.size()) || !t.peds[size_t(id)].alive) return;
	const Pedestrian &w = t.peds[size_t(id)];
	DeadBody d;
	d.id = DEAD_BODY_ID_BASE + int(t.dead_serial++ % uint32_t(DEAD_BODY_IDS));
	d.x = w.cur.x;
	d.y = w.cur.y;
	d.z = w.cur.z;
	if (lying(t, w)) { // down already: the body lies as the walker lay
		d.fall_x = w.fall_x;
		d.fall_z = w.fall_z;
	} else {
		fall_way(e, d.fall_x, d.fall_z);
	}
	d.scale = w.scale;
	d.look = w.look;
	d.lost = w.lost;
	const bool sidewalk = w.mode == PedMode::SIDEWALK && w.side >= 0;
	d.seg = sidewalk ? w.seg : -1;
	d.side = sidewalk ? w.side : -1;
	d.s = w.s;
	d.u = w.u;
	d.node = sidewalk ? -1 : w.way_node;
	tell(s, DamageEvent::BODY_DEAD, id, uint32_t(d.id));
	despawn(t, id, false);
	t.dead.push_back(d);
	while (t.dead.size() > size_t(s.catalog.blast.dead_kept)) {
		tell(s, DamageEvent::BODY_GONE, t.dead.front().id, 0);
		t.dead.pop_front();
	}
}

void move_dead_off_seg(CityState &s, int seg) {
	for (DeadBody &d : s.traffic.dead) {
		if (d.seg != seg || d.side < 0) continue;
		Pedestrian probe; // the body as somebody lying there, for the one rule of where they lie on
		probe.cur.x = d.x;
		probe.cur.y = d.y;
		probe.cur.z = d.z;
		probe.scale = d.scale;
		probe.seg = d.seg;
		probe.side = d.side;
		lie_on_nearest(s, probe);
		d.side = probe.side;
		d.seg = probe.side >= 0 ? probe.seg : -1;
		d.s = probe.s;
		d.u = probe.u;
	}
}

} // namespace traffic
} // namespace city
