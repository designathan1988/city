// What a blow does to the ground: the crater (recorded, and dug into the terrain on bare earth), the street furniture it
// knocks out (poles, signal posts, hydrants, benches, bins, trees: DamageState::gone), the utility poles the player built
// (taken down through the journal, the next pole pulled over by its wires now and then) with the wires they leave hanging,
// and the roads it cuts.
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/street/pole_arms.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace city {

namespace {
constexpr float FURNITURE_REACH_M = 200.f; // furniture is looked for this far (the segments the blow could touch)
constexpr float MIN_NORMAL_M = 1.f;
constexpr float ARC_DRAW_SCALE = 8.f; // the arc a piece stands at, as the draw's identity
constexpr uint32_t SALT_CRATER = 0x63726174U;
constexpr uint32_t SALT_THROW = 0x7468726fU;
constexpr uint32_t SALT_SNAP = 0x736e6170U;
constexpr uint32_t SALT_PULL = 0x70756c6cU;
constexpr uint32_t POLE_WHO_BASE = 0x40000000U; // a utility pole's draws, apart from the street furniture's (by its arc)

// How a pole at `d` from the burst goes (`who` its draw): one pulled over from past the reach falls whole; near the burst it
// bursts into splinters now and then; else it snaps or falls whole (actionsWiring.ts explodeAt).
float pole_fate(const Blow &b, float d, float reach, uint32_t who) {
	const Tuning &t = b.s.catalog.tune;
	if (d > reach) return FURNITURE_WHOLE;
	if (d < reach * t.strike_pole_splinter_share && b.roll(SALT_THROW, who) < t.strike_pole_splinter_chance) return FURNITURE_SPLINTERS;
	return b.roll(SALT_SNAP, who) < t.strike_pole_snap_chance ? FURNITURE_SNAPPED : FURNITURE_WHOLE;
}

// A wire torn loose, hanging from the pole at (x, z) that stands (its seat line `arm`) to the fallen one at (tx, tz).
void hang_wire(const Blow &b, float x, float z, ArmDir arm, float tx, float tz, bool rural) {
	DamageEvent e;
	e.kind = DamageEvent::WIRE;
	e.x = x; e.z = z; e.y = b.s.terrain.height_at(x, z);
	e.dx = arm.x; e.dz = arm.z;
	e.b = tx; e.c = tz; e.d = b.s.terrain.height_at(tx, tz);
	e.a = std::atan2(tz - b.z, tx - b.x); // the way out from the burst at the fallen end, which the wire is thrown
	e.u = rural ? 1u : 0u;
	b.emit(e);
}

// The wires of a lamp row cut at the stations knocked out: each span to a neighbour left standing hangs from it.
void hang_row_wires(const Blow &b, int sid, const std::vector<FurnitureSpot> &spots, const std::vector<char> &knocked) {
	for (size_t i = 0; i < spots.size(); i++) {
		const FurnitureSpot &f = spots[i];
		if (!knocked[i] || f.kind != FurnitureKind::LAMP || f.row != 0) continue;
		float tx, tz;
		if (!traffic::furniture_xz(b.s, sid, f, tx, tz)) continue;
		for (size_t j = 0; j < spots.size(); j++) {
			const FurnitureSpot &o = spots[j];
			if (knocked[j] || o.kind != FurnitureKind::LAMP || o.row != 0 || std::abs(o.station - f.station) != 1) continue;
			float x, z, fx, fz;
			if (traffic::furniture_facing(b.s, sid, o, x, z, fx, fz)) hang_wire(b, x, z, ArmDir{ fx, fz }, tx, tz, false);
		}
	}
}

// Poles, posts, trees and the rest within the overpressure that topples them.
void knock_out_furniture(Blow &b, float reach) {
	std::vector<FurnitureSpot> spots;
	std::vector<char> knocked;
	b.s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(b.s, sid, FURNITURE_ALL, spots, FurnitureRead::PLACED); // the blow knocks pieces out as it goes
		knocked.assign(spots.size(), 0);
		for (size_t i = 0; i < spots.size(); i++) {
			const FurnitureSpot &f = spots[i];
			float x, z;
			if (!knockable(f.kind) || !traffic::furniture_xz(b.s, sid, f, x, z)) continue;
			const float d = std::hypot(x - b.x, z - b.z);
			if (d > reach) continue;
			knocked[i] = 1;
			j_add_gone(b.s, { uint8_t(gone_kind(f.kind)), x, z });
			DamageEvent e;
			e.kind = DamageEvent::FURNITURE;
			e.x = x; e.z = z; e.y = b.s.terrain.height_at(x, z);
			e.dx = (x - b.x) / std::max(d, MIN_NORMAL_M);
			e.dz = (z - b.z) / std::max(d, MIN_NORMAL_M);
			e.u = uint32_t(gone_kind(f.kind));
			e.a = pole_fate(b, d, reach, uint32_t(f.v * ARC_DRAW_SCALE));
			e.v = f.kind == FurnitureKind::LAMP && f.lit ? 1u : 0u;
			b.emit(e);
			if (f.kind != FurnitureKind::HYDRANT) continue;
			DamageEvent g; // a hydrant broken off: a jet of water
			g.kind = DamageEvent::GEYSER;
			g.x = x; g.z = z; g.y = e.y;
			b.emit(g);
		}
		hang_row_wires(b, sid, spots, knocked);
	});
}

// The utility poles the player built (sim/street): those within the reach break, and the wires torn off a broken pole pull the
// next one over in its turn, now and then, out to a share past the reach (actionsWiring.ts explodeAt). Each span left with one
// end standing hangs from that end; the poles go through the journal, their spans with them, so undoing the blow stands them again.
void knock_out_poles(Blow &b, float reach) {
	const Tuning &t = b.s.catalog.tune;
	const StreetWorks &w = b.s.street;
	std::vector<int> down;
	w.poles_in(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		const UtilityPole &p = w.poles.items[size_t(id)];
		if (p.alive && std::hypot(p.x - b.x, p.z - b.z) <= reach) down.push_back(id);
	});
	if (down.empty()) return;
	std::sort(down.begin(), down.end());
	std::unordered_set<int> broken(down.begin(), down.end());
	const float pull = reach * t.strike_pole_pull_reach_k;
	for (size_t k = 0; k < down.size(); k++) { // the queue grows as poles are pulled over
		std::vector<int> spans = w.poles.items[size_t(down[k])].spans;
		std::sort(spans.begin(), spans.end());
		for (int span : spans) {
			const UtilitySpan &sp = w.spans.items[size_t(span)];
			const int other = sp.a == down[k] ? sp.b : sp.a;
			const UtilityPole &o = w.poles.items[size_t(other)];
			if (broken.count(other) || std::hypot(o.x - b.x, o.z - b.z) > pull || b.roll(SALT_PULL, uint32_t(span)) >= t.strike_pole_pull_chance) continue;
			broken.insert(other);
			down.push_back(other);
		}
	}
	for (int id : down) {
		const UtilityPole &p = w.poles.items[size_t(id)];
		for (int span : p.spans) {
			const UtilitySpan &sp = w.spans.items[size_t(span)];
			const int other = sp.a == id ? sp.b : sp.a;
			if (broken.count(other)) continue;
			const UtilityPole &o = w.poles.items[size_t(other)];
			const size_t at = size_t(std::find(o.spans.begin(), o.spans.end(), span) - o.spans.begin());
			hang_wire(b, o.x, o.z, seat_arm(pole_stand(w, b.s.roads, b.s.catalog.furniture, other), at), p.x, p.z, o.kind == PoleKind::RURAL);
		}
		const float d = std::hypot(p.x - b.x, p.z - b.z);
		DamageEvent e;
		e.kind = DamageEvent::FURNITURE;
		e.x = p.x; e.z = p.z; e.y = b.s.terrain.height_at(p.x, p.z);
		e.dx = (p.x - b.x) / std::max(d, MIN_NORMAL_M);
		e.dz = (p.z - b.z) / std::max(d, MIN_NORMAL_M);
		e.u = uint32_t(GoneKind::POLE);
		e.a = pole_fate(b, d, reach, POLE_WHO_BASE + uint32_t(id));
		e.v = p.lamp ? 1u : 0u;
		b.emit(e);
	}
	for (int id : down) j_remove_pole(b.s, id);
}
} // namespace

void strike_ground(Blow &b) {
	const BlastParams &p = b.p;
	const float reach = std::min(blast::range_at_pressure(p, b.kg, p.pole_kpa), FURNITURE_REACH_M);
	knock_out_furniture(b, reach);
	knock_out_poles(b, reach);
	if (b.on_building) return; // a burst on a roof digs no crater
	const float radius = blast::crater_radius(p, b.kg, b.soil), depth = blast::crater_depth(p, b.kg, b.soil);
	j_add_crater(b.s, { b.x, b.z, radius, depth, uint8_t(b.soil == blast::Surface::HARD), b.bits(SALT_CRATER, 0) });
	if (b.soil == blast::Surface::EARTH) {
		BrushDab d;
		d.x = b.x; d.z = b.z; d.radius = radius; d.strength = depth; d.dt = 1.f; d.mode = BrushMode::LOWER;
		j_terrain_touch(b.s, b.s.terrain.brush_rect(d.x, d.z, d.radius));
		b.s.terrain.brush(d);
		mark_ground(b.s, b.x - radius, b.z - radius, b.x + radius, b.z + radius);
	}
	cut_roads(b, radius);
}

} // namespace city
