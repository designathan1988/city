// What a blow does to the ground: the crater (recorded, and dug into the terrain on bare earth), the street furniture it
// knocks out (poles, signal posts, hydrants, benches, bins, trees: DamageState::gone) and the roads it cuts.
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float FURNITURE_REACH_M = 200.f; // furniture is looked for this far (the segments the blow could touch)
constexpr float SPLINTER_SHARE = 0.4f; // within this share of the knock-out range a pole goes to splinters, sometimes
constexpr float SPLINTER_CHANCE = 0.6f;
constexpr float MIN_NORMAL_M = 1.f;
constexpr float ARC_DRAW_SCALE = 8.f; // the arc a piece stands at, as the draw's identity
constexpr uint32_t SALT_CRATER = 0x63726174U;
constexpr uint32_t SALT_THROW = 0x7468726fU;

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
	const BlastParams &p = b.p;
	const float reach = std::min(blast::range_at_pressure(p, b.kg, p.pole_kpa), FURNITURE_REACH_M);
	std::vector<FurnitureSpot> spots;
	b.s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(b.s, sid, FURNITURE_ALL, spots);
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
			e.a = d < reach * SPLINTER_SHARE && b.roll(SALT_THROW, uint32_t(f.v * ARC_DRAW_SCALE)) < SPLINTER_CHANCE ? 1.f : 0.f;
			b.emit(e);
			if (f.kind != FurnitureKind::HYDRANT) continue;
			DamageEvent g; // a hydrant broken off: a jet of water
			g.kind = DamageEvent::GEYSER;
			g.x = x; g.z = z; g.y = e.y;
			b.emit(g);
		}
	});
}
} // namespace

void strike_ground(Blow &b) {
	const BlastParams &p = b.p;
	knock_out_furniture(b);
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
