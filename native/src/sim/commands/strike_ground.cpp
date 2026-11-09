// What a blow does to the ground: the crater (recorded, and dug into the terrain on bare earth), the street furniture it
// knocks out (poles, signal posts, hydrants, benches, bins, trees: DamageState::gone), the player's utility poles it fells with
// their wires, the trees it fells, and the roads it cuts.
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/nature_commands.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float MIN_NORMAL_M = 1.f; // a direction is taken over at least a metre (no division by a near zero)
constexpr float ARC_DRAW_SCALE = 8.f; // the arc a piece stands at, as the draw's identity
constexpr uint32_t SALT_CRATER = 0x63726174U; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_THROW = 0x7468726fU; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_PULL = 0x70756c6cU, SALT_SNAP = 0x736e6170U; // streams' salts: the words they name, in ASCII

GoneKind kind_of(FurnitureKind k) {
	switch (k) {
		case FurnitureKind::SIGNAL: return GoneKind::POST;
		case FurnitureKind::LAMP: return GoneKind::POLE;
		case FurnitureKind::HYDRANT: return GoneKind::HYDRANT;
		case FurnitureKind::BIN: return GoneKind::BIN;
		case FurnitureKind::BENCH: return GoneKind::BENCH;
		default: return GoneKind::TREE;
	}
}

// Poles, posts, trees and the rest within the overpressure that topples them.
void knock_out_furniture(Blow &b) {
	const Tuning &tn = b.s.catalog.tune;
	const BlastParams &p = b.p;
	const float reach = std::min(blast::range_at_pressure(p, b.kg, p.pole_kpa), tn.strike_furniture_reach_m);
	std::vector<FurnitureSpot> spots;
	b.s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(b.s, sid, FURNITURE_ALL, spots, FurnitureRead::PLACED); // the blow knocks pieces out as it goes
		for (const FurnitureSpot &f : spots) {
			float x, z;
			if (!traffic::furniture_xz(b.s, sid, f, x, z)) continue;
			const float d = std::hypot(x - b.x, z - b.z);
			if (d > reach) continue;
			j_add_gone(b.s, { uint8_t(kind_of(f.kind)), x, z });
			DamageEvent e;
			e.kind = DamageEvent::FURNITURE;
			e.x = x; e.z = z; e.y = b.s.terrain.height_at(x, z);
			e.dx = (x - b.x) / std::max(d, MIN_NORMAL_M);
			e.dz = (z - b.z) / std::max(d, MIN_NORMAL_M);
			e.u = uint32_t(kind_of(f.kind));
			e.v = uint32_t(f.kind) + 1; // what it was, for its look (damage_event.h): the gone record keeps the coarser kind
			e.a = d < reach * tn.strike_splinter_share && b.roll(SALT_THROW, uint32_t(f.v * ARC_DRAW_SCALE)) < tn.strike_splinter_chance ? 1.f : 0.f;
			b.emit(e);
			if (f.kind != FurnitureKind::HYDRANT) continue;
			DamageEvent g; // a hydrant broken off: a jet of water
			g.kind = DamageEvent::GEYSER;
			g.x = x; g.z = z; g.y = e.y;
			b.emit(g);
		}
	});
}
// The player's utility poles within the fireball radius fall, and those their spans pull over with them; a span left with one
// pole standing hangs from it as a torn wire (actionsWiring.ts explodeAt, its poles and wires). Each removal is the street's
// journalled one (j_remove_pole, its spans with it): undone with the blow, saved with the street.
void knock_down_poles(Blow &b) {
	const Tuning &tn = b.s.catalog.tune;
	const StreetWorks &w = b.s.street;
	const float reach = blast::fireball_radius(tn, b.strength), pull = reach * tn.strike_pull_share;
	const auto from_blow = [&](const UtilityPole &p) { return std::hypot(p.x - b.x, p.z - b.z); };
	std::vector<uint8_t> down(w.poles.items.size(), 0);
	std::vector<int> felled;
	w.poles_in(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		const UtilityPole &p = w.poles.items[size_t(id)];
		if (p.alive && !down[size_t(id)] && from_blow(p) < reach) {
			down[size_t(id)] = 1;
			felled.push_back(id);
		}
	});
	std::sort(felled.begin(), felled.end()); // the same pulls whatever order the index gave them in
	for (size_t k = 0; k < felled.size(); k++) {
		std::vector<int> spans = w.poles.items[size_t(felled[k])].spans;
		std::sort(spans.begin(), spans.end());
		for (int sid : spans) {
			const UtilitySpan &sp = w.spans.items[size_t(sid)];
			const int other = sp.a == felled[k] ? sp.b : sp.a;
			if (!sp.alive || other < 0 || down[size_t(other)]) continue;
			const UtilityPole &o = w.poles.items[size_t(other)];
			if (!o.alive || from_blow(o) >= pull || b.roll(SALT_PULL, uint32_t(sid)) >= tn.strike_pull_chance) continue;
			down[size_t(other)] = 1;
			felled.push_back(other);
		}
	}
	const float height = b.s.catalog.furniture.poles.height_m;
	for (int id : felled) // each span left with one pole standing: a wire hanging from it, torn off the felled one
		for (int sid : w.poles.items[size_t(id)].spans) {
			const UtilitySpan &sp = w.spans.items[size_t(sid)];
			const int other = sp.a == id ? sp.b : sp.a;
			if (!sp.alive || other < 0 || down[size_t(other)]) continue;
			const UtilityPole &st = w.poles.items[size_t(other)], &fl = w.poles.items[size_t(id)];
			DamageEvent e;
			e.kind = DamageEvent::WIRE;
			e.x = st.x; e.z = st.z; e.y = b.s.terrain.height_at(st.x, st.z);
			e.b = fl.x; e.c = b.s.terrain.height_at(fl.x, fl.z); e.d = fl.z;
			e.a = height;
			b.emit(e);
		}
	for (int id : felled) {
		const UtilityPole p = w.poles.items[size_t(id)];
		const float d = from_blow(p);
		const bool close = d < reach * tn.strike_splinter_share;
		const int broke = d > reach ? FURNITURE_WHOLE
				: close && b.roll(SALT_THROW, uint32_t(id)) < tn.strike_splinter_chance ? FURNITURE_SPLINTERS
				: b.roll(SALT_SNAP, uint32_t(id)) < tn.strike_snap_chance ? FURNITURE_SNAPPED : FURNITURE_WHOLE;
		DamageEvent e;
		e.kind = DamageEvent::FURNITURE;
		e.x = p.x; e.z = p.z; e.y = b.s.terrain.height_at(p.x, p.z);
		e.dx = (p.x - b.x) / std::max(d, MIN_NORMAL_M);
		e.dz = (p.z - b.z) / std::max(d, MIN_NORMAL_M);
		e.u = uint32_t(GoneKind::POLE);
		e.a = float(broke);
		e.b = height;
		e.c = p.lamp ? 1.f : 0.f;
		b.emit(e);
		j_remove_pole(b.s, id);
	}
}

// The trees within the fireball radius are felled (actionsWiring.ts explodeAt: the landscape's trees thrown and gone): a clearing
// cut there through the nature command path (the woods' trees and the planted ones go, undone with the blow), the planted ones
// thrown as trees, and the woods' ones told by the clearing, which the render steps over to find them.
void fell_trees(Blow &b) {
	const float reach = blast::fireball_radius(b.s.catalog.tune, b.strength);
	std::vector<PlantedTree> planted;
	const int clearing = fell_trees_at(b.s, b.x, b.z, reach, &planted);
	if (clearing < 0) return; // the map holds as many clearings as it may: nothing is felled
	for (const PlantedTree &t : planted) {
		const float d = std::hypot(t.x - b.x, t.z - b.z);
		DamageEvent e;
		e.kind = DamageEvent::FURNITURE;
		e.x = t.x; e.z = t.z; e.y = b.s.terrain.height_at(t.x, t.z);
		e.dx = (t.x - b.x) / std::max(d, MIN_NORMAL_M);
		e.dz = (t.z - b.z) / std::max(d, MIN_NORMAL_M);
		e.u = uint32_t(GoneKind::TREE);
		e.b = t.height;
		b.emit(e);
	}
	DamageEvent e;
	e.kind = DamageEvent::TREES;
	e.x = b.x; e.z = b.z; e.y = b.ground;
	e.a = reach;
	e.u = uint32_t(clearing);
	b.emit(e);
}
} // namespace

void strike_ground(Blow &b) {
	const BlastParams &p = b.p;
	knock_out_furniture(b);
	knock_down_poles(b);
	fell_trees(b);
	if (b.on_building) return; // a burst on a roof digs no crater
	const float radius = blast::crater_radius(p, blast::fireball_radius(b.s.catalog.tune, b.strength)), depth = blast::crater_depth(p, b.strength);
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
